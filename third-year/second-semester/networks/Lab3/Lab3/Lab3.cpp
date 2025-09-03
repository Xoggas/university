#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 5150
#define BUFFER_SIZE 512

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    int role = 0;
    std::cout << "Enter 1 for Server, 2 for Client: ";
    std::cin >> role;

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    if (role == 1)
    {
        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(SERVER_PORT);

        if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            WSACleanup();
            return 1;
        }

        char buffer[BUFFER_SIZE];
        sockaddr_in clientAddr{};
        int clientAddrSize = sizeof(clientAddr);

        std::cout << "Server waiting for messages..." << std::endl;

        while (true)
        {
            int bytesReceived = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0, (sockaddr*)&clientAddr, &clientAddrSize);
            if (bytesReceived == SOCKET_ERROR)
            {
                std::cerr << "recvfrom failed: " << WSAGetLastError() << std::endl;
                break;
            }
            buffer[bytesReceived] = '\0';
            std::cout << "Received message: " << buffer << std::endl;
        }
    }
    else if (role == 2)
    {
        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(SERVER_PORT);

        if (InetPton(AF_INET, L"127.0.0.1", &serverAddr.sin_addr) != 1)
        {
            std::cerr << "InetPton failed." << std::endl;
            closesocket(sock);
            WSACleanup();
            return 1;
        }

        std::cin.ignore();
        char message[BUFFER_SIZE];

        while (true)
        {
            std::cout << "Enter message to send (or type 'exit' to quit): ";
            std::cin.getline(message, BUFFER_SIZE);

            if (strlen(message) == 0 || strcmp(message, "exit") == 0)
            {
                break;
            }

            int bytesSent = sendto(sock, message, strlen(message), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
            if (bytesSent == SOCKET_ERROR)
            {
                std::cerr << "sendto failed: " << WSAGetLastError() << std::endl;
            }
            else
            {
                std::cout << "Message sent." << std::endl;
            }
        }
    }
    else
    {
        std::cout << "Invalid option." << std::endl;
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
