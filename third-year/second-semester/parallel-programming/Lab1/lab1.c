#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sched.h>

#define NUM_THREADS 2 // Количество потоков
#define N 50000000

// Функция для вычисления подвыражений суммы
void *compute_partial_sum(void *arg)
{
    long thread_id = (long)arg;
    double *local_sum = malloc(sizeof(double)); // Выделяем память для результата
    *local_sum = 0.0;

    // Разделение индексов между потоками
    for (long i = thread_id + 1; i <= N; i += NUM_THREADS)
    {
        double numerator = cos(pow(i, 3)) + pow(i, 4) * exp(-i) + log(i + 1);
        double denominator = sqrt(i * i + tan(i) + 1) + tgamma(i + 1);

        // Проверяем на допустимость знаменатель
        if (isnan(denominator) || denominator == 0.0 || isnan(numerator))
        {
            printf("Недопустимое значение при i = %ld\n", i);
            continue; // Пропускаем текущую итерацию
        }

        *local_sum += numerator / denominator;
    }

    return local_sum; // Возвращаем локальную сумму
}

// Функция для привязки потока к определенному ядру
void pin_thread_to_core(int core_id)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    double total_sum = 0.0;

    // Проверяем количество аргументов
    if (argc != 2)
    {
        fprintf(stderr, "Использование: %s <режим>\n", argv[0]);
        fprintf(stderr, "Режимы: 1 - Потоки на разных ядрах, 2 - Все потоки на одном ядре\n");
        return EXIT_FAILURE;
    }

    // Парсим режим из аргумента
    int mode = atoi(argv[1]);
    if (mode != 1 && mode != 2)
    {
        fprintf(stderr, "Некорректный режим. Используйте 1 или 2.\n");
        return EXIT_FAILURE;
    }

    if (mode == 1)
    {
        printf("Режим: Потоки на разных ядрах\n");
    }
    else
    {
        printf("Режим: Все потоки на одном ядре\n");
    }

    // Создаем потоки
    for (long t = 0; t < NUM_THREADS; t++)
    {
        if (pthread_create(&threads[t], NULL, compute_partial_sum, (void *)t))
        {
            fprintf(stderr, "Ошибка создания потока\n");
            return EXIT_FAILURE;
        }

        // Привязка потока к ядру
        if (mode == 1)
        {
            pin_thread_to_core(t % NUM_THREADS); // Каждый поток к своему ядру
        }
        else if (mode == 2)
        {
            pin_thread_to_core(0); // Все потоки на ядре 0
        }
    }

    // Собираем результаты от потоков
    for (int t = 0; t < NUM_THREADS; t++)
    {
        double *local_sum;
        pthread_join(threads[t], (void **)&local_sum); // Получаем результат потока
        total_sum += *local_sum;
        free(local_sum); // Освобождаем память, выделенную потоком
    }

    // Вывод результата
    printf("Результат суммы S: %.10f\n", total_sum);

    return EXIT_SUCCESS;
}
