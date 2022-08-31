/* Trey Wells
   CSCE 463
   Fall 2022 */

#include "pch.h"

bool url::ParseUrl() {

	printf("\tParsing URL... ");

	if (strlen(baseUrl) > MAX_URL_LEN) {
		printf("Url is too long!\n");
		return false;
	}

	memcpy(backupUrl, baseUrl, strlen(baseUrl));

	// Proper protocol
	char protocol[] = "http://";

	if (strncmp(protocol, baseUrl, strlen(protocol)) != 0) {
		printf("failed with invalid scheme\n");
		return false;
	}

	baseUrl = baseUrl + strlen(protocol);

	// Check for fragment
	char* fragment = strchr(baseUrl, '#');

	if (fragment != NULL) {
		baseUrl[fragment - baseUrl] = '\0';
	}

	// Check for query
	char* query = strchr(baseUrl, '?');

	char tempQuery[MAX_REQUEST_LEN];
	sprintf(tempQuery, "\0");
	if (query) {
		sprintf(tempQuery, "%s\0", query);
		baseUrl[query - baseUrl] = '\0';
	}

	// Check for path
	char* path = strchr(baseUrl, '/');

	if (path == NULL) {
		if (strlen(tempQuery) > MAX_REQUEST_LEN - 1) {
			printf("failed with invalid request\n");
			return false;
		}
		sprintf(request, "/%s", tempQuery);
	}
	else {
		if (strlen(tempQuery) + strlen(path) > MAX_REQUEST_LEN) {
			printf("failed with invalid request\n");
			return false;
		}
		sprintf(request, "%s%s", path, tempQuery);
		*path = '\0';
	}

	requestLength = strlen(request);

	// Check for port
	char* tempPort = strchr(baseUrl, ':');

	if (tempPort) {
		if (strlen(tempPort + 1) == 0) {
			printf("failed with invalid port\n");
			return false;
		}

		if (strlen(tempPort + 1) > MAX_PORT_LEN) {
			printf("failed with invalid port\n");
			return false;
		}

		int portNumber = atoi(tempPort + 1);

		// Check if port is invalid
		if (portNumber < 1) {
			printf("failed with invalid port\n");
			return false;
		}

		sprintf(port, "%s", tempPort + 1);
		baseUrl[tempPort - baseUrl] = '\0';
	}
	else {
		sprintf(port, "80");
	}

	if (strlen(host) > MAX_HOST_LEN) {
		printf("failed with invalid host\n");
		return false;
	}

	sprintf(host, "%s", baseUrl);

	printf("host %s, port %s, request %s\n",host,port,request);

	return true;
}

bool url::getIP() {

	clock_t startTime = clock();

	// first assume that the string is an IP address
	DWORD IP = inet_addr(host);

	if (IP == INADDR_NONE)
	{
		// if not a valid IP, then do a DNS lookup
		if ((dnsLookup = gethostbyname(host)) == NULL)
		{
			printf("failed with %d\n", WSAGetLastError());
			return false;
		}
		else // take the first IP address and copy into sin_addr
			memcpy((char*)&(server.sin_addr), dnsLookup->h_addr, dnsLookup->h_length);
	}
	else
	{
		// if a valid IP, directly drop its binary version into sin_addr
		server.sin_addr.S_un.S_addr = IP;
	}

	printf("done in %.0f ms, found %s\n", (double)(clock() - startTime)/CLOCKS_PER_SEC*1000, inet_ntoa(server.sin_addr));

	return true;
}

bool url::connectSocket() {

	clock_t startTime = clock();

	server.sin_family = AF_INET;
	server.sin_port = htons((u_short)atoi(port));

	if (connect(sock->sock, (struct sockaddr*)&server,
		sizeof(struct sockaddr_in)) == SOCKET_ERROR) {

		printf("failed with %d\n", WSAGetLastError());
		return false;
	}

	printf("done in %.0f ms\n", (double)(clock() - startTime) / CLOCKS_PER_SEC * 1000);

	return true;
}

bool url::sendRequest() {

	char formatString[] = "GET %s HTTP/1.0\r\nUser-agent: theCrawler/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n";

	char* sendBuf = new char[strlen(formatString) - 4 + 1 + requestLength + strlen(host)]; // +1 for NULL
	
	sprintf(sendBuf, formatString, request, host);

	if (send(sock->sock, sendBuf, strlen(formatString) - 4 + 1 + requestLength + strlen(host), 0) == SOCKET_ERROR) {
		printf("Send error: %d\n", WSAGetLastError());
		return false;
	}

	delete[] sendBuf;

	return true;
}

bool url::loadPage() {
	clock_t startTime = clock();

	if (!sock->Read()) {
		return false;
	}

	// Check for HTTP Header	
	char httpHeader[] = "HTTP/";

	if (strncmp(sock->buf, httpHeader, strlen(httpHeader)) != 0) {
		printf("failed with non-HTTP header\n");
		return false;
	}

	startOfHeader = sock->buf;

	// Find the start of the object part of the file
	char* carriageReturn = strstr(startOfHeader, "\r\n\r\n");
	*carriageReturn = '\0';

	startOfObject = carriageReturn + 4;

	printf("done in %.0f ms with %lu bytes\n", (double)(clock() - startTime) / CLOCKS_PER_SEC * 1000, sock->curPos);

	printf("\tVerifying header... ");

	statusCode = atoi(sock->buf + strlen(httpHeader) + 4);

	printf("status code %d\n", statusCode);

	return true;
}

bool url::parsePage() {
	printf("      + Parsing page... ");

	clock_t startTime = clock();

	parser.Parse(startOfObject, sock->curPos - strlen(startOfHeader) - 4, backupUrl, strlen(backupUrl), &nlinks);

	if (nlinks < 0)
		nlinks = 0;

	printf("done in %.0f ms with %lu links\n", (double)(clock() - startTime) / CLOCKS_PER_SEC * 1000, nlinks);

	return true;

}

void url::run()  {

	Socket s1;
	sock = &s1;

	printf("URL: %s\n", baseUrl);

	if (!ParseUrl()) {
		return;
	}

	printf("\tDoing DNS... ");

	if (!getIP()) {
		return;
	}

	printf("      * Connecting on page... ");
	if (!connectSocket()) {
		return;
	}

	if (!sendRequest()) {
		return;
	}

	printf("\tLoading... ");
	if (!loadPage()) {
		return;
	}

	if (statusCode / 100 == 2) {
		parsePage();
	}

	printf("\n----------------------------------------\n");

	printf("%s\n", startOfHeader);

}

