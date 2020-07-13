#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define SERV_PORT 12345

static const char* SERV_ADDR = "192.168.8.46";

int main () {
	int sock;
	char message[BUF_SIZE] = {};
	int str_len;
	struct sockaddr_in serv_adr;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		std::cerr << "Failed to create socket" << std::endl;
		return -1;
	}
	memset(&serv_adr, 0x0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(SERV_ADDR);
	serv_adr.sin_port=htons(SERV_PORT);

	/* set non-blocking */

	/* set no-route */
	int sock_opt = 0;
	int one = 1;
	sock_opt |= SO_DONTROUTE;
	setsockopt(sock, SOL_SOCKET, sock_opt, reinterpret_cast<char*>(&one), sizeof(one));

	if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) {
		std::cerr << "Failed to connect : " << strerror(errno) << std::endl;
		return -1;
	}

	std::cerr << "Success to connect" << std::endl;
	return 0;
}
