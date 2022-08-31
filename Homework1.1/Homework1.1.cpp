/* Trey Wells
   CSCE 463
   Fall 2022 */

#include "pch.h"

int main(int argc, char** argv) {

    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        printf("WSAStartup error %d\n", WSAGetLastError());
        exit(-1);
    }

    // Check number of arguments
    if (argc != 2) {
        printf("Incorrect amount of commandline arguments\n");
        exit(-1);
    }

    url myUrl;
    myUrl.baseUrl = argv[1];

    myUrl.run();

    if (WSACleanup() == SOCKET_ERROR) {
        printf("WSACleanup() failed with %d\n", WSAGetLastError());
        exit(-1);
    }

}

