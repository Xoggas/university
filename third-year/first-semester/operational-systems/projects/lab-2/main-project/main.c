#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/wait.h>
#include <pthread.h>

#define STORAGE_CAPACITY 5000
#define NUM_HEADS 3
#define msleep(ms) usleep(ms * 1000 * 100)

typedef enum Product
{
    None,
    Cake,
    Brownie,
    Candy,
    Gingerbread
} Product;

char *productNamesLookup[5] = {
    "пусто",
    "торт",
    "пирожное",
    "конфета",
    "пряник"
};

typedef struct Storage
{
    Product products[STORAGE_CAPACITY];
    int length;
} Storage;

void initStorage(Storage *storage)
{
    if (storage == NULL)
    {
        return;
    }

    for (int i = 0; i < STORAGE_CAPACITY; i++)
    {
        storage->products[i] = None;
    }

    storage->length = 0;
}

void addProduct(Storage *storage, const Product product)
{
    if (product == None)
    {
        return;
    }

    if (storage->length < STORAGE_CAPACITY)
    {
        storage->products[storage->length++] = product;

        printf("Добавили %s в хранилище\n", productNamesLookup[product]);
    }
    else
    {
        printf("Хранилище заполнено!\n");
    }
}

void removeProduct(Storage *storage, const Product product)
{
    if (storage->length > 0)
    {
        for (int i = 0; i < storage->length; i++)
        {
            if (storage->products[i] == product)
            {
                printf("Забрали продукт %s\n", productNamesLookup[product]);

                storage->products[i] = None;

                return;
            }
        }

        printf("Не получилось найти продукт!\n");
    }

    printf("Хранилище пусто!\n");
}

typedef struct FactorySettings
{
    Storage *storage;
    Product product;
    int delayInMs;
    int batchSize;
} FactorySettings;

void makeProduct(const FactorySettings *factory_settings)
{
    while (1)
    {
        printf("Фабрика начала производство %d %s\n", factory_settings->batchSize,
               productNamesLookup[factory_settings->product]);

        msleep(factory_settings->delayInMs);

        for (int i = 0; i < factory_settings->batchSize; i++)
        {
            addProduct(factory_settings->storage, factory_settings->product);
        }
    }
}

pthread_t runFactoryThread(Storage *storage, const Product product, const int delayInMs, const int batchSize)
{
    pthread_t factoryThread;

    FactorySettings *cakeSettings = malloc(sizeof(FactorySettings));
    cakeSettings->storage = storage;
    cakeSettings->product = product;
    cakeSettings->delayInMs = delayInMs;
    cakeSettings->batchSize = batchSize;

    if (pthread_create(&factoryThread, NULL, makeProduct, cakeSettings) != 0)
    {
        perror("Ошибка создания потока для фабрики\n");

        exit(1);
    }

    return factoryThread;
}

int main()
{
    Storage *storage = malloc(sizeof(Storage));
    initStorage(storage);

    const pthread_t cakeThread = runFactoryThread(storage, Cake, 70, 1);
    const pthread_t brownieThread = runFactoryThread(storage, Brownie, 50, 2);
    const pthread_t candyThread = runFactoryThread(storage, Candy, 40, 3);
    const pthread_t gingerbreadThread = runFactoryThread(storage, Gingerbread, 60, 1);

    pthread_join(cakeThread, NULL);
    pthread_join(brownieThread, NULL);
    pthread_join(candyThread, NULL);
    pthread_join(gingerbreadThread, NULL);

    return 0;
}
