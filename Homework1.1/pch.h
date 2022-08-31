/* Trey Wells
   CSCE 463
   Fall 2022 */

#ifndef PCH_H
#define PCH_H

#define MAX_HOST_LEN		256
#define MAX_URL_LEN			2048
#define MAX_REQUEST_LEN		2048
#define MAX_PORT_LEN        6
#define INITIAL_BUF_SIZE    4 * 1024   // 4 KB
#define THRESHOLD           100        

// add headers that you want to pre-compile here
#include <Windows.h>
#include <winsock.h>
#include <iostream>

#include "HTMLParserBase.h"
#include "Socket.h"
#include "URL.h"

#endif //PCH_H
