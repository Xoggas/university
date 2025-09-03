#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

void PrintArpTable()
{
    PMIB_IPNETTABLE pIpNetTable = nullptr;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    GetIpNetTable(nullptr, &dwSize, 0);
    pIpNetTable = (MIB_IPNETTABLE*)malloc(dwSize);

    if ((dwRetVal = GetIpNetTable(pIpNetTable, &dwSize, 0)) == NO_ERROR)
    {
        std::cout << "ARP Table:\n";

        for (DWORD i = 0; i < pIpNetTable->dwNumEntries; i++)
        {
            MIB_IPNETROW row = pIpNetTable->table[i];
            char ipStr[INET_ADDRSTRLEN] = { 0 };

            IN_ADDR ipAddr;
            ipAddr.S_un.S_addr = row.dwAddr;
            InetNtopA(AF_INET, &ipAddr, ipStr, sizeof(ipStr));

            std::cout << "IP: " << ipStr << "\tMAC: ";

            for (int j = 0; j < (int)row.dwPhysAddrLen; j++)
            {
                printf("%02X", (int)row.bPhysAddr[j]);
                if (j < (int)row.dwPhysAddrLen - 1)
                {
                    std::cout << "-";
                }
            }

            std::cout << "\tType: ";
            switch (row.dwType)
            {
            case 3: std::cout << "dynamic"; break;
            case 4: std::cout << "static"; break;
            case 2: std::cout << "invalid"; break;
            default: std::cout << "unknown"; break;
            }
            std::cout << std::endl;
        }
    }
    else
    {
        std::cerr << "GetIpNetTable error: " << dwRetVal << std::endl;
    }

    if (pIpNetTable)
    {
        free(pIpNetTable);
    }
}

void AddArpEntry(const std::string& ipStr, const std::string& macStr, const std::string& interfaceIp)
{
    MIB_IPNETROW row;
    ZeroMemory(&row, sizeof(row));

    IN_ADDR ipAddr;
    if (InetPtonA(AF_INET, ipStr.c_str(), &ipAddr) != 1)
    {
        std::cerr << "Invalid IP address.\n";
        return;
    }

    row.dwAddr = ipAddr.S_un.S_addr;
    row.dwPhysAddrLen = 6;

    int values[6] = { 0 };
    if (sscanf_s(macStr.c_str(), "%x-%x-%x-%x-%x-%x",
        &values[0], &values[1], &values[2],
        &values[3], &values[4], &values[5]) != 6)
    {
        std::cerr << "Invalid MAC address.\n";
        return;
    }

    for (int i = 0; i < 6; i++)
    {
        row.bPhysAddr[i] = (BYTE)values[i];
    }

    row.dwType = 4; // static

    PMIB_IPADDRTABLE pTable;
    DWORD size = 0;
    GetIpAddrTable(nullptr, &size, 0);
    pTable = (MIB_IPADDRTABLE*)malloc(size);
    GetIpAddrTable(pTable, &size, 0);

    for (DWORD i = 0; i < pTable->dwNumEntries; i++)
    {
        IN_ADDR addr;
        addr.S_un.S_addr = pTable->table[i].dwAddr;
        char addrStr[INET_ADDRSTRLEN];
        InetNtopA(AF_INET, &addr, addrStr, sizeof(addrStr));

        if (interfaceIp == addrStr)
        {
            row.dwIndex = pTable->table[i].dwIndex;
            break;
        }
    }

    free(pTable);

    DWORD result = CreateIpNetEntry(&row);

    if (result == NO_ERROR)
    {
        std::cout << "Entry successfully added.\n";
    }
    else
    {
        std::cerr << "Error adding entry: " << result << std::endl;
    }
}

void DeleteArpEntry(const std::string& ipStr, const std::string& interfaceIp)
{
    MIB_IPNETROW row;
    ZeroMemory(&row, sizeof(row));

    IN_ADDR ipAddr;
    if (InetPtonA(AF_INET, ipStr.c_str(), &ipAddr) != 1)
    {
        std::cerr << "Invalid IP address.\n";
        return;
    }

    row.dwAddr = ipAddr.S_un.S_addr;

    PMIB_IPADDRTABLE pTable;
    DWORD size = 0;
    GetIpAddrTable(nullptr, &size, 0);
    pTable = (MIB_IPADDRTABLE*)malloc(size);
    GetIpAddrTable(pTable, &size, 0);

    for (DWORD i = 0; i < pTable->dwNumEntries; i++)
    {
        IN_ADDR addr;
        addr.S_un.S_addr = pTable->table[i].dwAddr;
        char addrStr[INET_ADDRSTRLEN];
        InetNtopA(AF_INET, &addr, addrStr, sizeof(addrStr));

        if (interfaceIp == addrStr)
        {
            row.dwIndex = pTable->table[i].dwIndex;
            break;
        }
    }

    free(pTable);

    DWORD result = DeleteIpNetEntry(&row);

    if (result == NO_ERROR)
    {
        std::cout << "Entry successfully deleted.\n";
    }
    else
    {
        std::cerr << "Error deleting entry: " << result << std::endl;
    }
}

void GetMacByIP(const std::string& ipStr)
{
    IN_ADDR ipAddr;
    if (InetPtonA(AF_INET, ipStr.c_str(), &ipAddr) != 1)
    {
        std::cerr << "Invalid IP address.\n";
        return;
    }

    ULONG macAddr[2];
    ULONG macLen = 6;

    if (SendARP(ipAddr.S_un.S_addr, 0, macAddr, &macLen) == NO_ERROR)
    {
        BYTE* mac = (BYTE*)macAddr;
        printf("MAC address for IP %s: %02X-%02X-%02X-%02X-%02X-%02X\n",
            ipStr.c_str(), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    else
    {
        std::cerr << "Failed to get MAC address for IP " << ipStr << std::endl;
    }
}

int main()
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    int choice;
    std::string ip, mac, iface;

    while (true)
    {
        std::cout << "\nMenu:\n";
        std::cout << "1. Show ARP table\n";
        std::cout << "2. Add entry\n";
        std::cout << "3. Delete entry\n";
        std::cout << "4. Get MAC by IP\n";
        std::cout << "0. Exit\n";
        std::cout << "Your choice: ";
        std::cin >> choice;

        switch (choice)
        {
        case 1:
        {
            PrintArpTable();
            break;
        }
        case 2:
        {
            std::cout << "Enter IP: ";
            std::cin >> ip;
            std::cout << "Enter MAC (e.g. 00-11-22-33-44-55): ";
            std::cin >> mac;
            std::cout << "Enter interface IP (e.g. 192.168.0.6): ";
            std::cin >> iface;
            AddArpEntry(ip, mac, iface);
            break;
        }
        case 3:
        {
            std::cout << "Enter IP: ";
            std::cin >> ip;
            std::cout << "Enter interface IP: ";
            std::cin >> iface;
            DeleteArpEntry(ip, iface);
            break;
        }
        case 4:
        {
            std::cout << "Enter IP: ";
            std::cin >> ip;
            GetMacByIP(ip);
            break;
        }
        case 0:
        {
            WSACleanup();
            return 0;
        }
        default:
        {
            std::cout << "Invalid choice!\n";
            break;
        }
        }
    }

    WSACleanup();
    return 0;
}
