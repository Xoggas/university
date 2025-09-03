#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 2048

const char* html_page =
"<html><head><title>Test Page</title></head>"
"<body><h1>Hello from HTTP server!</h1></body></html>";

const char* http_200_template =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html\r\n"
"Content-Length: %d\r\n"
"Connection: close\r\n\r\n%s";

const char* http_200_head_template =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html\r\n"
"Content-Length: %d\r\n"
"Connection: close\r\n\r\n";

const char* http_404 =
"HTTP/1.1 404 Not Found\r\n"
"Content-Length: 0\r\n"
"Connection: close\r\n\r\n";

int main()
{
    WSADATA wsaData;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_len;
    char buffer[BUFFER_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed.\n");
        return 1;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET)
    {
        printf("Socket creation failed.\n");
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Binding failed.\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 5) < 0)
    {
        printf("Listening failed.\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("HTTP server is running on port %d...\n", PORT);

    while (1)
    {
        client_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);

        if (client_socket == INVALID_SOCKET)
        {
            printf("Failed to accept client.\n");
            continue;
        }

        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0)
        {
            closesocket(client_socket);
            continue;
        }

        buffer[bytes_received] = '\0';

        printf("Received request:\n%s\n", buffer);

        if (strncmp(buffer, "GET ", 4) == 0 || strncmp(buffer, "HEAD ", 5) == 0)
        {
            char method[8], path[256];
            sscanf(buffer, "%s %s", method, path);

            if (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0)
            {
                char response[BUFFER_SIZE];
                if (strcmp(method, "GET") == 0)
                {
                    sprintf(response, http_200_template, (int)strlen(html_page), html_page);
                }
                else // HEAD
                {
                    sprintf(response, http_200_head_template, (int)strlen(html_page));
                }
                send(client_socket, response, (int)strlen(response), 0);
            }
            else
            {
                send(client_socket, http_404, (int)strlen(http_404), 0);
            }
        }
        else
        {
            send(client_socket, http_404, (int)strlen(http_404), 0);
        }

        closesocket(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
