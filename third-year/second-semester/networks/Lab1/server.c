#include <stdio.h>

#include "headers/server.h"

#define OK 0
#define DRIVER_IS_NOT_INSTALLED 1

int main()
{
    if (IPX_IsDriverOk() == false)
    {
        printf("IPX driver is not installed\n");

        return DRIVER_IS_NOT_INSTALLED;
    }

    Server_start();

    return OK;
}
