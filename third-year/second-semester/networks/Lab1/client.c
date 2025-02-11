#include <stdio.h>

#include "headers/client.h"

#define OK 0
#define DRIVER_IS_NOT_INSTALLED 1

#define SEND_MESSAGES 1
#define RECEIVE_MESSAGES 2

void print_current_directory()
{
    char path[128];

    if (getcwd(path, sizeof(path)) != NULL)
    {
        printf("Current dir: %s\n", path);
    }
    else
    {
        printf("Could'n retrieve current dir!\n");
    }
}

int main()
{
    print_current_directory();

    if (IPX_IsDriverOk() == false)
    {
        printf("IPX driver is not installed\n");

        return DRIVER_IS_NOT_INSTALLED;
    }

    Client_start();

    return OK;
}
