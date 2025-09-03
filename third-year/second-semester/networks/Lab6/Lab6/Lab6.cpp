#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

void ResolveHostnameToIP()
{
    char hostname[256];

    printf("Enter hostname: ");
    scanf("%255s", hostname);

    struct addrinfo hints = { 0 };
    struct addrinfo* result = NULL;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, NULL, &hints, &result) != 0)
    {
        printf("Failed to resolve hostname.\n");
        return;
    }

    printf("IP addresses:\n");

    for (struct addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        char ipstr[INET6_ADDRSTRLEN];

        void* addr;

        if (ptr->ai_family == AF_INET)
        {
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)ptr->ai_addr;
            addr = &(ipv4->sin_addr);
        }
        else
        {
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)ptr->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        inet_ntop(ptr->ai_family, addr, ipstr, sizeof(ipstr));
        printf("  %s\n", ipstr);
    }

    freeaddrinfo(result);
}

void ResolveIPToHostname()
{
    char ip[INET6_ADDRSTRLEN];

    printf("Enter IP address: ");
    scanf("%45s", ip);

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;

    if (inet_pton(AF_INET, ip, &sa.sin_addr) != 1)
    {
        printf("Invalid IP address.\n");
        return;
    }

    char hostname[NI_MAXHOST];

    if (getnameinfo((struct sockaddr*)&sa, sizeof(sa), hostname, sizeof(hostname), NULL, 0, 0) != 0)
    {
        printf("Failed to resolve IP address.\n");
        return;
    }

    printf("Hostname: %s\n", hostname);
}

void ShowDHCPInfo()
{
    DWORD dwRetVal;
    ULONG outBufLen = sizeof(IP_ADAPTER_INFO);
    PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO*)malloc(outBufLen);

    if (GetAdaptersInfo(pAdapterInfo, &outBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(outBufLen);
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &outBufLen)) != NO_ERROR)
    {
        printf("GetAdaptersInfo failed: %d\n", dwRetVal);
        free(pAdapterInfo);
        return;
    }

    PIP_ADAPTER_INFO pAdapter = pAdapterInfo;

    while (pAdapter)
    {
        if (pAdapter->DhcpEnabled)
        {
            printf("Adapter: %s\n", pAdapter->Description);
            printf("  DHCP Server: %s\n", pAdapter->DhcpServer.IpAddress.String);
        }

        pAdapter = pAdapter->Next;
    }

    free(pAdapterInfo);
}

void ReleaseAndRenewIP()
{
    DWORD ret;

    ret = system("ipconfig /release");
    if (ret != 0)
    {
        printf("Failed to release IP address.\n");
    }
    else
    {
        printf("IP address released.\n");
    }

    ret = system("ipconfig /renew");
    if (ret != 0)
    {
        printf("Failed to renew IP address.\n");
    }
    else
    {
        printf("IP address renewed.\n");
    }
}

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed.\n");
        return 1;
    }

    int choice;

    do
    {
        printf("\nSelect option:\n");
        printf("  1 - Resolve hostname to IP\n");
        printf("  2 - Resolve IP to hostname\n");
        printf("  3 - Show DHCP server info\n");
        printf("  4 - Release and renew IP\n");
        printf("  0 - Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
        {
            ResolveHostnameToIP();
            break;
        }
        case 2:
        {
            ResolveIPToHostname();
            break;
        }
        case 3:
        {
            ShowDHCPInfo();
            break;
        }
        case 4:
        {
            ReleaseAndRenewIP();
            break;
        }
        case 0:
        {
            break;
        }
        default:
        {
            printf("Invalid option.\n");
            break;
        }
        }

    } while (choice != 0);

    WSACleanup();
    return 0;
}
