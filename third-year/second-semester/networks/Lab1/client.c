#include <stdio.h>

#include "headers/client.h"

#define OK 0
#define DRIVER_IS_NOT_INSTALLED 1

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
