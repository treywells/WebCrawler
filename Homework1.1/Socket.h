/* Trey Wells
   CSCE 463
   Fall 2022 */

#pragma once

class Socket {
public:
	SOCKET sock; // socket handle
	char* buf; // current buffer
	int allocatedSize; // bytes allocated for buf
	int curPos; // current position in buffer 

	bool Read(void);

	Socket();
	~Socket();

};