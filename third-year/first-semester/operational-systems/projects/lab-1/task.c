#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

void random_delay(const int max_seconds)
{
    srand(time(NULL) ^ getpid());
    const int delay = rand() % max_seconds + 1;
    printf("Процесс %d ждёт %d секунд\n", getpid(), delay);
    sleep(delay);
}

pid_t pid_0 = -1, pid_1 = -1, pid_2 = -1, pid_3 = -1, pid_4 = -1, pid_5 = -1, pid_6 = -1;

int main()
{
    pid_0 = getpid();

    printf("Запустили корневой процесс %d\n", pid_0);

    pid_1 = fork();

    if (pid_1 == 0)
    {
        printf("Запустили процесс 1 с ID %d, потомок %d\n", getpid(), getppid());

        pid_2 = fork();

        if (pid_2 == 0)
        {
            printf("Запустили процесс 2 с ID %d, потомок %d\n", getpid(), getppid());
        }
        else
        {
            pid_3 = fork();

            if (pid_3 == 0)
            {
                printf("Запустили процесс 3 с ID %d, потомок %d\n", getpid(), getppid());
            }
        }
    }
    else
    {
        pid_4 = fork();

        if (pid_4 == 0)
        {
            printf("Запустили процесс 4 с ID %d, потомок %d\n", getpid(), getppid());

            pid_5 = fork();

            if (pid_5 == 0)
            {
                printf("Запустили процесс 5 с ID %d, потомок %d\n", getpid(), getppid());
            }
            else
            {
                pid_6 = fork();

                if (pid_6 == 0)
                {
                    printf("Запустили процесс 6 с ID %d, потомок %d\n", getpid(), getppid());
                }
            }
        }
    }

    random_delay(8);

    exit(0);

    while (wait(NULL) > 0)
    {
    }

    printf("Процесс %d завершился\n", getpid());

    return 0;
}
