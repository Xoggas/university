import React, { useMemo, useRef, useState, useEffect } from "react";
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from "recharts";
import { motion } from "framer-motion";

// === Utility helpers ===
function randomInt(min, max) {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

function clone(arr) {
  return arr.slice();
}

// Energy (number of attacking pairs). Lower is better; 0 means solved.
function energyFromBoard(board) {
  // board[c] = r (row index for queen in column c)
  const n = board.length;
  const rowCount = new Map();
  const diag1 = new Map(); // r - c
  const diag2 = new Map(); // r + c

  let conflicts = 0;

  for (let c = 0; c < n; c++) {
    const r = board[c];
    rowCount.set(r, (rowCount.get(r) || 0) + 1);
    diag1.set(r - c, (diag1.get(r - c) || 0) + 1);
    diag2.set(r + c, (diag2.get(r + c) || 0) + 1);
  }

  const addPairs = (k) => (k >= 2 ? (k * (k - 1)) / 2 : 0);
  for (const k of rowCount.values()) conflicts += addPairs(k);
  for (const k of diag1.values()) conflicts += addPairs(k);
  for (const k of diag2.values()) conflicts += addPairs(k);

  return conflicts;
}

function randomBoard(n) {
  // One queen per column, random row
  const b = new Array(n);
  for (let c = 0; c < n; c++) b[c] = randomInt(0, n - 1);
  return b;
}

function Board({ board, highlightBest }) {
  const n = board.length;
  const maxSize = 640; // px
  const cell = Math.max(6, Math.floor(maxSize / n));
  const size = Math.min(maxSize, cell * n);

  return (
    <div className="flex flex-col items-center">
      <div className="mb-2 text-sm text-gray-600">Размер доски: {n} × {n}</div>
      <div
        className="relative rounded-2xl overflow-hidden shadow-md border border-gray-200"
        style={{ width: size, height: size }}
      >
        {/* Cells */}
        {Array.from({ length: n * n }).map((_, idx) => {
          const r = Math.floor(idx / n);
          const c = idx % n;
          const dark = (r + c) % 2 === 1;
          return (
            <div
              key={idx}
              style={{
                position: "absolute",
                left: c * cell,
                top: r * cell,
                width: cell,
                height: cell,
                background: dark ? "#b58863" : "#f0d9b5",
              }}
            />
          );
        })}

        {/* Queens */}
        {board.map((r, c) => (
          <motion.div
            key={c}
            initial={{ scale: 0.6, opacity: 0 }}
            animate={{ scale: 1, opacity: 1 }}
            transition={{ type: "spring", stiffness: 200, damping: 20 }}
            style={{
              position: "absolute",
              left: c * cell,
              top: r * cell,
              width: cell,
              height: cell,
              display: "grid",
              placeItems: "center",
            }}
          >
            <div
              className={`w-[70%] h-[70%] rounded-full shadow ${
                highlightBest ? "bg-emerald-500" : "bg-slate-800"
              }`}
            />
          </motion.div>
        ))}
      </div>
    </div>
  );
}

export default function NQueensAnnealingApp() {
  const [n, setN] = useState(50); // supports N > 20
  const [tMax, setTMax] = useState(10);
  const [tMin, setTMin] = useState(0.001);
  const [alpha, setAlpha] = useState(0.95);
  const [stepsPerTemp, setStepsPerTemp] = useState(200);

  const [board, setBoard] = useState(() => randomBoard(50));
  const [currentEnergy, setCurrentEnergy] = useState(() => energyFromBoard(board));

  const [bestBoard, setBestBoard] = useState(board);
  const [bestEnergy, setBestEnergy] = useState(currentEnergy);

  const [running, setRunning] = useState(false);
  const [temperature, setTemperature] = useState(tMax);

  // Metrics for charts
  const [history, setHistory] = useState([]); // array of { tick, temp, bestEnergy, badAccepted }
  const tickRef = useRef(0);

  // Internal loop control
  const loopRef = useRef(null);

  // Reset when N changes
  useEffect(() => {
    const b = randomBoard(n);
    const e = energyFromBoard(b);
    setBoard(b);
    setCurrentEnergy(e);
    setBestBoard(b);
    setBestEnergy(e);
    setHistory([]);
    tickRef.current = 0;
    setTemperature(tMax);
    setRunning(false);
  }, [n]);

  const start = () => {
    if (running) return;
    setRunning(true);
    setTemperature(Number(tMax));

    // Ensure best resets for a fair run
    const e = energyFromBoard(board);
    setCurrentEnergy(e);
    setBestBoard(board);
    setBestEnergy(e);
    setHistory([]);
    tickRef.current = 0;

    // Kick loop
    stepCoolingCycle();
  };

  const stop = () => {
    setRunning(false);
    if (loopRef.current) {
      clearTimeout(loopRef.current);
      loopRef.current = null;
    }
  };

  const runToEnd = () => {
    stop();
    let b = board.slice();
    let e = energyFromBoard(b);
    let T = Number(tMax);
    let localHistory = [];
    let bestB = b.slice();
    let bestE = e;

    while (T > Number(tMin) && e > 0) {
      let localBadAccepted = 0;
      for (let k = 0; k < Number(stepsPerTemp); k++) {
        const c = randomInt(0, n - 1);
        const newR = randomInt(0, n - 1);
        if (newR === b[c]) continue;
        const oldR = b[c];
        b[c] = newR;
        const eNew = energyFromBoard(b);
        const delta = eNew - e;

        if (delta <= 0) {
          e = eNew;
        } else {
          const accept = Math.random() < Math.exp(-delta / T);
          if (accept) {
            e = eNew;
            localBadAccepted++;
          } else {
            b[c] = oldR;
          }
        }

        if (e < bestE) {
          bestE = e;
          bestB = b.slice();
        }

        tickRef.current += 1;
      }

      localHistory.push({
        tick: tickRef.current,
        temp: T,
        bestEnergy: bestE,
        badAccepted: localBadAccepted,
      });

      T *= Number(alpha);
    }

    setBoard(b);
    setCurrentEnergy(e);
    setBestBoard(bestB);
    setBestEnergy(bestE);
    setHistory(localHistory);
    setTemperature(T);
    setRunning(false);
  };

  function stepCoolingCycle() {
    // Perform stepsPerTemp Metropolis steps at current temperature
    let b = board.slice();
    let e = energyFromBoard(b);
    let localBadAccepted = 0;
    const T = temperature;

    for (let k = 0; k < Number(stepsPerTemp); k++) {
      const c = randomInt(0, n - 1);
      const newR = randomInt(0, n - 1);
      if (newR === b[c]) {
        continue;
      }
      const oldR = b[c];
      b[c] = newR;
      const eNew = energyFromBoard(b);
      const delta = eNew - e;

      if (delta <= 0) {
        e = eNew;
      } else {
        const accept = Math.random() < Math.exp(-delta / T);
        if (accept) {
          e = eNew;
          localBadAccepted++;
        } else {
          b[c] = oldR;
        }
      }

      if (e < bestEnergy) {
        setBestEnergy(e);
        setBestBoard(b.slice());
      }

      tickRef.current += 1;
    }

    setBoard(b);
    setCurrentEnergy(e);

    setHistory((h) => [
      ...h,
      {
        tick: tickRef.current,
        temp: T,
        bestEnergy: Math.min(bestEnergy, e),
        badAccepted: localBadAccepted,
      },
    ]);

    const Tnext = T * Number(alpha);
    setTemperature(Tnext);

    const solved = e === 0;
    if (!running || Tnext <= Number(tMin) || solved) {
      setRunning(false);
      return;
    }

    loopRef.current = setTimeout(stepCoolingCycle, 0);
  }

  const onParam = (setter) => (e) => setter(e.target.value);

  const resetBoard = () => {
    stop();
    const b = randomBoard(n);
    const e = energyFromBoard(b);
    setBoard(b);
    setCurrentEnergy(e);
    setBestBoard(b);
    setBestEnergy(e);
    setHistory([]);
    tickRef.current = 0;
    setTemperature(Number(tMax));
  };

  return (
    <div className="p-6 max-w-7xl mx-auto">
      <h1 className="text-2xl font-semibold mb-2">N‑ферзей: имитация отжига</h1>
      <p className="text-gray-600 mb-6">Решение задачи N‑ферзей (N &gt; 20) методом имитации отжига. Введите параметры, запустите поиск, наблюдайте доску и графики метрик.</p>

      {/* Controls */}
      <div className="grid md:grid-cols-5 gap-4 mb-6">
        <label className="flex flex-col gap-1">
          <span className="text-sm text-gray-600">Количество ферзей (N)</span>
          <input type="number" min={4} value={n} onChange={(e)=> setN(Math.max(4, parseInt(e.target.value||"0",10)))} className="px-3 py-2 rounded-xl border" />
        </label>
        <label className="flex flex-col gap-1">
          <span className="text-sm text-gray-600">Макс. температура</span>
          <input type="number" step="0.1" value={tMax} onChange={onParam(setTMax)} className="px-3 py-2 rounded-xl border" />
        </label>
        <label className="flex flex-col gap-1">
          <span className="text-sm text-gray-600">Мин. температура</span>
          <input type="number" step="0.0001" value={tMin} onChange={onParam(setTMin)} className="px-3 py-2 rounded-xl border" />
        </label>
        <label className="flex flex-col gap-1">
          <span className="text-sm text-gray-600">Коэффициент охлаждения (α)</span>
          <input type="number" step="0.001" value={alpha} onChange={onParam(setAlpha)} className="px-3 py-2 rounded-xl border" />
        </label>
        <label className="flex flex-col gap-1">
          <span className="text-sm text-gray-600">Шагов на температуре</span>
          <input type="number" min={1} value={stepsPerTemp} onChange={onParam(setStepsPerTemp)} className="px-3 py-2 rounded-xl border" />
        </label>
      </div>

      <div className="flex flex-wrap items-center gap-3 mb-6">
        <button onClick={start} disabled={running} className={`px-4 py-2 rounded-2xl shadow ${running?"bg-gray-300":"bg-emerald-600 hover:bg-emerald-700 text-white"}`}>
          Запустить
        </button>
        <button onClick={stop} disabled={!running} className={`px-4 py-2 rounded-2xl shadow ${!running?"bg-gray-300":"bg-rose-600 hover:bg-rose-700 text-white"}`}>
          Остановить
        </button>
        <button onClick={runToEnd} className="px-4 py-2 rounded-2xl shadow bg-blue-600 hover:bg-blue-700 text-white">
          Запустить до конца
        </button>
        <button onClick={resetBoard} className="px-4 py-2 rounded-2xl shadow bg-slate-700 hover:bg-slate-800 text-white">
          Сбросить доску
        </button>
        <div className="text-sm text-gray-700 ml-auto">
          Температура: <span className="font-mono">{temperature.toFixed(4)}</span>{" "}
          | Текущая энергия: <span className="font-mono">{currentEnergy}</span>{" "}
          | Лучшая энергия: <span className="font-mono">{bestEnergy}</span>
        </div>
      </div>

      <div className="grid lg:grid-cols-2 gap-8 items-start">
        <Board board={bestBoard} highlightBest={true} />

        {/* Charts */}
        <div className="w-full h-[420px] rounded-2xl border bg-white shadow-sm p-3">
          <div className="text-sm text-gray-600 mb-2">Графики: принятые плохие решения (за шаг охлаждения), лучшая энергия, температура</div>
          <ResponsiveContainer width="100%" height="100%">
            <LineChart data={history} margin={{ top: 10, right: 20, bottom: 10, left: 0 }}>
              <CartesianGrid strokeDasharray="3 3" />
              <XAxis dataKey="tick" tick={{ fontSize: 12 }} />
              <YAxis yAxisId="left" tick={{ fontSize: 12 }} />
              <YAxis yAxisId="right" orientation="right" tick={{ fontSize: 12 }} />
              <Tooltip />
              <Legend />
              <Line yAxisId="left" type="monotone" dataKey="badAccepted" name="Плохие принятые" dot={false} strokeWidth={2} />
              <Line yAxisId="left" type="monotone" dataKey="bestEnergy" name="Лучшая энергия" dot={false} strokeWidth={2} />
              <Line yAxisId="right" type="monotone" dataKey="temp" name="Температура" dot={false} strokeWidth={2} />
            </LineChart>
          </ResponsiveContainer>
        </div>
      </div>

      <div className="mt-6 text-sm text-gray-600 space-y-2">
        <p>
          <span className="font-semibold">Энергия</span> — это число конфликтующих пар ферзей: пары на одной строке и на диагоналях. Цель — довести энергию до нуля.
        </p>
        <p>
          <span className="font-semibold">Правило Метрополиса</span>: ухудшения принимаются с вероятностью exp(-ΔE/T), где ΔE — рост энергии, T — температура.
        </p>
        <p>
          <span className="font-semibold">Охлаждение</span>: T ← α·T на каждом этапе; при низкой T ухудшения почти не принимаются.
        </p>
      </div>
    </div>
  );
}
