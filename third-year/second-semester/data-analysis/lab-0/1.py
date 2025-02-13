import random

# Исходные данные
i = 2
j = 10
k = 8

# Вероятности
p0 = i / (2 * (i + j + k))
p1 = (j + k) / (2 * (i + j + k))
p2 = (i + k) / (2 * (i + j + k))
p3 = j / (2 * (i + j + k))

# Проверяем, что сумма вероятностей равна 1
assert abs(p0 + p1 + p2 + p3 - 1) < 1e-6, "Сумма вероятностей должна быть равна 1"

# Генерация случайных чисел
n = 10000  # Количество испытаний
values = [0, 1, 2, 3]  # Возможные значения
probabilities = [p0, p1, p2, p3]  # Соответствующие вероятности


# Функция для выбора случайного значения на основе вероятностей
def weighted_choice(values, probabilities):
    cumulative_probabilities = []
    cumulative = 0

    # Вычисляем кумулятивные вероятности
    for p in probabilities:
        cumulative += p
        cumulative_probabilities.append(cumulative)

    rnd = random.random()  # Генерируем случайное число от 0 до 1

    # Используем кумулятивные вероятности для выбора значения
    for i, cumulative_prob in enumerate(cumulative_probabilities):
        if rnd < cumulative_prob:
            return values[i]


# Генерация выборки
generated_data = [weighted_choice(values, probabilities) for _ in range(n)]

# Подсчёт частот
frequencies = {value: 0 for value in values}
for num in generated_data:
    frequencies[num] += 1

# Проверка соответствия распределению с помощью критерия сравнения долей
expected_counts = [p * n for p in probabilities]
observed_counts = [frequencies[value] for value in values]

# Расчёт статистики хи-квадрат вручную
chi2_stat = sum((obs - exp) ** 2 / exp for obs, exp in zip(observed_counts, expected_counts))

# Для 3 степеней свободы критическое значение хи-квадрат на уровне 0.05 равно 7.8
critical_value = 7.8

# Вывод результатов
print("Частоты появления значений:")
for value, count in frequencies.items():
    print(f"Значение {value}: {count}")

print("\nОжидаемые частоты:")
for value, expected in zip(values, expected_counts):
    print(f"Значение {value}: {expected:.2f}")

print(f"\nСтатистика хи-квадрат: {chi2_stat:.4f}")
if chi2_stat < critical_value:
    print("На уровне значимости 0.05 нет оснований отвергнуть гипотезу о соответствии распределению ξ.")
else:
    print("На уровне значимости 0.05 гипотеза о соответствии распределению ξ отвергается.")
