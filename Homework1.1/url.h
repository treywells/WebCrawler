/* Trey Wells
   CSCE 463
   Fall 2022 */

#pragma once

class url {
public:
	char* baseUrl;
	char backupUrl[MAX_URL_LEN];
	char host[MAX_HOST_LEN];
	char port[MAX_PORT_LEN];
	char request[MAX_REQUEST_LEN];
	DWORD requestLength;
	int statusCode;
	struct hostent* dnsLookup;
	struct sockaddr_in server;
	Socket* sock;
	char* startOfHeader;
	char* startOfObject;
	HTMLParserBase parser;
	int nlinks;

	bool ParseUrl();
	bool getIP();
	bool connectSocket();
	bool sendRequest();
	bool loadPage();
	bool parsePage();
	void run();


};
