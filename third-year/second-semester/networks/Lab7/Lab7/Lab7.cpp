#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

// === SMTP SETTINGS ===
#define SMTP_SERVER     "localmail.test"
#define SMTP_PORT       25

// base64-encoded login and password
const char* SMTP_LOGIN_B64 = "dGVzdEBsb2NhbG1haWwudGVzdA==";
const char* SMTP_PASSWORD_B64 = "MTIzNA==";  

const char* EMAIL_FROM = "test@localmail.test";
const char* EMAIL_TO = "test@localmail.test";

// === POP3 SETTINGS ===
#define POP3_SERVER     "localmail.test"
#define POP3_PORT       110

const char* POP3_USER = "test@localmail.test";
const char* POP3_PASS = "1234";

// === NETWORK UTILS ===
void sendLine(SOCKET sock, const char* data)
{
    send(sock, data, strlen(data), 0);
    send(sock, "\r\n", 2, 0);
    printf(">> %s\n", data);
}

void recvLine(SOCKET sock)
{
    char buffer[1024] = { 0 };
    recv(sock, buffer, sizeof(buffer) - 1, 0);
    printf("<< %s\n", buffer);
}

// === SEND EMAIL ===
void smtpSend()
{
    struct hostent* host;
    struct sockaddr_in server;
    SOCKET sock;

    printf("Connecting to SMTP server...\n");
    host = gethostbyname(SMTP_SERVER);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    server.sin_family = AF_INET;
    server.sin_port = htons(SMTP_PORT);
    server.sin_addr.s_addr = *((unsigned long*)host->h_addr);

    connect(sock, (struct sockaddr*)&server, sizeof(server));
    recvLine(sock);

    sendLine(sock, "HELO example.com");
    recvLine(sock);

    sendLine(sock, "AUTH LOGIN");
    recvLine(sock);

    sendLine(sock, SMTP_LOGIN_B64);
    recvLine(sock);

    sendLine(sock, SMTP_PASSWORD_B64);
    recvLine(sock);

    char buf[256];
    sprintf(buf, "MAIL FROM:<%s>", EMAIL_FROM);
    sendLine(sock, buf);
    recvLine(sock);

    sprintf(buf, "RCPT TO:<%s>", EMAIL_TO);
    sendLine(sock, buf);
    recvLine(sock);

    sendLine(sock, "DATA");
    recvLine(sock);

    sendLine(sock, "Subject: Test Email");
    sendLine(sock, "From: sender@example.com");
    sendLine(sock, "To: receiver@example.com");
    sendLine(sock, "");
    sendLine(sock, "Hello from a C program!");
    sendLine(sock, ".");
    recvLine(sock);

    sendLine(sock, "QUIT");
    recvLine(sock);

    closesocket(sock);
}

// === RECEIVE EMAILS ===
void pop3Receive()
{
    struct hostent* host;
    struct sockaddr_in server;
    SOCKET sock;

    printf("Connecting to POP3 server...\n");
    host = gethostbyname(POP3_SERVER);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    server.sin_family = AF_INET;
    server.sin_port = htons(POP3_PORT);
    server.sin_addr.s_addr = *((unsigned long*)host->h_addr);

    connect(sock, (struct sockaddr*)&server, sizeof(server));
    recvLine(sock);

    char buf[256];
    sprintf(buf, "USER %s", POP3_USER);
    sendLine(sock, buf);
    recvLine(sock);

    sprintf(buf, "PASS %s", POP3_PASS);
    sendLine(sock, buf);
    recvLine(sock);

    sendLine(sock, "LIST");
    recvLine(sock);

    printf("Retrieving message #1...\n");
    sendLine(sock, "RETR 1");

    char buffer[1024];
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        int len = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0) break;
        printf("%s", buffer);
        if (strstr(buffer, "\r\n.\r\n")) break;
    }

    sendLine(sock, "QUIT");
    recvLine(sock);
    closesocket(sock);
}

// === MAIN ===
int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    int choice;
    printf("Choose an action:\n");
    printf("1 - Send an email\n");
    printf("2 - Receive emails\n");
    printf("Enter a number: ");
    scanf("%d", &choice);

    if (choice == 1)
    {
        smtpSend();
    }
    else if (choice == 2)
    {
        pop3Receive();
    }
    else
    {
        printf("Invalid choice.\n");
    }

    WSACleanup();
    return 0;
}
