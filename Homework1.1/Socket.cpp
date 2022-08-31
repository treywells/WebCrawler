/* Trey Wells
   CSCE 463
   Fall 2022 */

#include "pch.h"

Socket::Socket() {
	allocatedSize = INITIAL_BUF_SIZE;
	buf = new char[allocatedSize];
	curPos = 0;

	// Create the socket
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		printf("socket() error %d\n", WSAGetLastError());
		exit(-1);
	}
}

Socket::~Socket() {
	
	delete[] buf;

	if (closesocket(sock) == SOCKET_ERROR) {
		printf("Failed to close socket with error: %d\n", WSAGetLastError());
	}

}

bool Socket::Read(void) {

	// set timeout to 10 seconds
	timeval timeoutParam;
	timeoutParam.tv_sec = 10;
	timeoutParam.tv_usec = 0;
	int socketsReady;

	while (true) {

		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(sock, &fd);

		// wait to see if socket has any data (see MSDN)
		if ((socketsReady = select(0, &fd, NULL, NULL, &timeoutParam)) > 0) {
			// new data available; now read the next segment
			int bytes = recv(sock, buf + curPos, allocatedSize - curPos, NULL);

			if (bytes == SOCKET_ERROR) {
				printf("failed with %d on recv\n", WSAGetLastError());
				break;
			}
			if (bytes == 0) {
				// NULL-terminate buffer
				buf[curPos] = '/0';
				return true; // normal completion
			}
			curPos += bytes; // adjust where the next recv goes
			if (allocatedSize - curPos < THRESHOLD) {
				buf = (char*) realloc(buf, allocatedSize * 2);
				if (buf == NULL) {
					printf("realloc failed, quitting program\n");
					return false;
				}
				allocatedSize *= 2;
			}
		}
		else if (socketsReady == 0) {
			printf("failed with timeout\n");
			break;
		}
		else {
			printf("failed with %d on select()\n", WSAGetLastError());
			break;
		}
	}
	return false;
}