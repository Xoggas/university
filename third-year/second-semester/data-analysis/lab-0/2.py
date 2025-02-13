import numpy as np
import matplotlib.pyplot as plt

# Параметры
lambda_value = 6 / 4
n = 10000  # количество случайных чисел

# Генерация псевдослучайных чисел из экспоненциального распределения
random_data = np.random.exponential(1 / lambda_value, n)

# Строим гистограмму
plt.hist(random_data, bins=30, density=True, alpha=0.6, color='g', label='Гистограмма')

# Строим теоретическую плотность распределения
x = np.linspace(0, np.max(random_data), 1000)
pdf = lambda_value * np.exp(-lambda_value * x)
plt.plot(x, pdf, 'r-', lw=2, label='Теоретическая плотность')

# Настройки графика
plt.title('Гистограмма и теоретическая плотность для экспоненциального распределения')
plt.xlabel('Значение')
plt.ylabel('Плотность вероятности')
plt.legend()
plt.show()

# Среднее арифметическое
mean_value = np.mean(random_data)
print(f"Среднее арифметическое выборки: {mean_value:.4f}")

# Разбиение выборки на интервалы для проверки на соответствие распределению
intervals = np.linspace(0, np.max(random_data), 11)  # 10 интервалов
observed_counts, _ = np.histogram(random_data, bins=intervals)

# Ожидаемые частоты для показательному распределению
expected_counts = n * (np.exp(-lambda_value * intervals[1:]) - np.exp(-lambda_value * intervals[:-1]))

# Статистика хи-квадрат
chi2_stat = np.sum((observed_counts - expected_counts) ** 2 / expected_counts)

# Вывод статистики хи-квадрат
print(f"Статистика хи-квадрат: {chi2_stat:.4f}")

# Критическое значение хи-квадрат для α=0.05 и 9 степеней свободы (для 10 интервалов)
critical_value = 16.919  # Из таблицы хи-квадрат для df = 9 и α = 0.05

# Проверка гипотезы
if chi2_stat < critical_value:
    print("На уровне значимости 0.05 гипотеза о соответствии данным показателю не отвергается.")
else:
    print("На уровне значимости 0.05 гипотеза о соответствии данным показателю отвергается.")
