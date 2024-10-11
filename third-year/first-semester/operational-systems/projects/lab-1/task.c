#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int sharedMemoryId;

void initPids()
{
    pid_t* pids = shmat(sharedMemoryId, NULL, 0);

    for (int i = 0; i < 7; i++)
    {
        pids[i] = -1;
    }

    shmdt(pids);
}

void disposePids()
{
    shmctl(sharedMemoryId, IPC_RMID, NULL);
}

void setPid(int index, pid_t pid)
{
    pid_t* pids = shmat(sharedMemoryId, NULL, 0);

    pids[index] = pid;

    shmdt(pids);
}

pid_t getPid(int index)
{
    pid_t* pids = shmat(sharedMemoryId, NULL, 0);

    pid_t result = pids[index];

    shmdt(pids);

    return result;
}

void printProcessesBinaryTree()
{
    printf("0. ﹂ %d\n", getPid(0));
    printf("1.    ﹂ %d\n", getPid(1));
    printf("2.       ﹂ %d\n", getPid(2));
    printf("3.       ﹂ %d\n", getPid(3));
    printf("4.    ﹂ %d\n", getPid(4));
    printf("5.       ﹂ %d\n", getPid(5));
    printf("6.       ﹂ %d\n", getPid(6));
    printf("\n");
}

int status;

int main()
{
    sharedMemoryId = shmget(IPC_PRIVATE, sizeof(pid_t) * 7, IPC_CREAT | 0666);

    initPids();

    setPid(0, getpid());

    printf("Запустили корневой процесс %d\n", getpid());

    pid_t process1 = fork();

    if (process1 == 0)
    {
        setPid(1, getpid());

        printf("Запустили процесс 1 с ID %d который является потомком %d\n", getpid(), getppid());

        pid_t process2 = fork();

        if (process2 != 0)
        {
            setPid(2, getpid());

            printf("Запустили процесс 2 с ID %d который является потомком %d\n", getpid(), getppid());

            pause();
        }
        else
        {
            pid_t process3 = fork();

            if (process3 == 0)
            {
                setPid(3, process3);

                printf("Запустили процесс 3 с ID %d который является потомком %d\n", getpid(), getppid());

                pause();
            }
        }

        pause();
    }
    else
    {
        pid_t process4 = fork();

        if (process4 == 0)
        {
            setPid(4, process4);

            printf("Запустили процесс 4 с ID %d который является потомком %d\n", getpid(), getppid());

            pid_t process5 = fork();

            if (process5 == 0)
            {
                setPid(5, getpid());

                pid_t process6 = fork();

                if (process6 == 0)
                {
                    setPid(6, process6);

                    printf("Запустили процесс 6 с ID %d который является потомком %d\n", getpid(), getppid());

                    printf("\nДерево процессов выглядит следующим образом\n");

                    printProcessesBinaryTree();

                    process6 = getPid(6);

                    setPid(6, getPid(0));

                    printf("Трансформировали дерево процессов в граф, заменив процесс %d на корневой процесс %d\n",
                           process6, getPid(0));

                    printf("\nТеперь дерево процессов выглядит следующим образом\n");

                    printProcessesBinaryTree();

                    printf("Процесс 6 завершил свою работу\n");

                    exit(0);
                }
            }
            else
            {
                setPid(5, process5);

                printf("Запустили процесс 5 с ID %d который является потомком %d\n", getpid(), getppid());

                printf("Усыпляем процесс 5 на 8 секунд\n");

                sleep(8);

                printf("Процесс 5 завершил свою работу\n");

                exit(0);
            }

            printf("Процесс 4 ожидает завершения процесса 5 с ID %d\n", getPid(5));

            waitpid(getPid(5), &status, 0);

            if (WIFEXITED(status))
            {
                printf("Процесс 4 завершил свою работу\n");
            }

            exit(0);
        }
    }

    if (getpid() == getPid(0))
    {
        pause();
    }

    disposePids();

    return 0;
}