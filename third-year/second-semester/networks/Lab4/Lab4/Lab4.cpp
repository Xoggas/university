#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 5150
#define BUFFER_SIZE 512

void run_udp_server();
void run_udp_client();
void run_tcp_server();
void run_tcp_client();

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    int role = 0;
    int protocol = 0;

    std::cout << "Enter 1 for Server, 2 for Client: ";
    std::cin >> role;

    std::cout << "Enter 1 for TCP, 2 for UDP: ";
    std::cin >> protocol;

    if (role == 1 && protocol == 1)
    {
        run_tcp_server();
    }
    else if (role == 1 && protocol == 2)
    {
        run_udp_server();
    }
    else if (role == 2 && protocol == 1)
    {
        run_tcp_client();
    }
    else if (role == 2 && protocol == 2)
    {
        run_udp_client();
    }
    else
    {
        std::cout << "Invalid option." << std::endl;
    }

    WSACleanup();
    return 0;
}

void run_udp_server()
{
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "UDP Socket creation failed: " << WSAGetLastError() << std::endl;
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        return;
    }

    char buffer[BUFFER_SIZE];
    sockaddr_in clientAddr{};
    int clientAddrSize = sizeof(clientAddr);

    std::cout << "UDP Server waiting for messages..." << std::endl;

    while (true)
    {
        int bytesReceived = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0, (sockaddr*)&clientAddr, &clientAddrSize);
        if (bytesReceived == SOCKET_ERROR)
        {
            std::cerr << "recvfrom failed: " << WSAGetLastError() << std::endl;
            break;
        }
        buffer[bytesReceived] = '\0';
        std::cout << "Received (UDP): " << buffer << std::endl;
    }

    closesocket(sock);
}

void run_udp_client()
{
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "UDP Socket creation failed: " << WSAGetLastError() << std::endl;
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (InetPton(AF_INET, L"127.0.0.1", &serverAddr.sin_addr) != 1)
    {
        std::cerr << "InetPton failed." << std::endl;
        closesocket(sock);
        return;
    }

    std::cin.ignore();
    char message[BUFFER_SIZE];

    while (true)
    {
        std::cout << "Enter message to send (UDP) (or 'exit'): ";
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
    }

    closesocket(sock);
}

void run_tcp_server()
{
    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSock == INVALID_SOCKET)
    {
        std::cerr << "TCP Socket creation failed: " << WSAGetLastError() << std::endl;
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSock);
        return;
    }

    if (listen(listenSock, 1) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSock);
        return;
    }

    std::cout << "TCP Server waiting for connection..." << std::endl;

    SOCKET clientSock = accept(listenSock, NULL, NULL);
    if (clientSock == INVALID_SOCKET)
    {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSock);
        return;
    }

    char buffer[BUFFER_SIZE];

    while (true)
    {
        int bytesReceived = recv(clientSock, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0)
        {
            break;
        }
        buffer[bytesReceived] = '\0';
        std::cout << "Received (TCP): " << buffer << std::endl;
    }

    closesocket(clientSock);
    closesocket(listenSock);
}

void run_tcp_client()
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "TCP Socket creation failed: " << WSAGetLastError() << std::endl;
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (InetPton(AF_INET, L"127.0.0.1", &serverAddr.sin_addr) != 1)
    {
        std::cerr << "InetPton failed." << std::endl;
        closesocket(sock);
        return;
    }

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        return;
    }

    std::cin.ignore();
    char message[BUFFER_SIZE];

    while (true)
    {
        std::cout << "Enter message to send (TCP) (or 'exit'): ";
        std::cin.getline(message, BUFFER_SIZE);

        if (strlen(message) == 0 || strcmp(message, "exit") == 0)
        {
            break;
        }

        int bytesSent = send(sock, message, strlen(message), 0);
        if (bytesSent == SOCKET_ERROR)
        {
            std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        }
    }

    closesocket(sock);
}
