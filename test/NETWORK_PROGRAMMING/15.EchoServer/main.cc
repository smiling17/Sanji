#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <iostream>

#include <arpa/inet.h>
#include <sys/socket.h>


#define BUF_SIZE 1024

static const char* SERV_ADDR = "192.168.8.46";
static unsigned int SERV_PORT = 12346;

int main() {
	int serv_sock, clnt_sock;
	char message[BUF_SIZE];

	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;

	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1) {
		printf("failed to create socket\n");
	}
	memset(&serv_adr, 0x0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(SERV_ADDR);
	serv_adr.sin_port = htons(SERV_PORT);

	if (bind(serv_sock, (struct sockaddr*)(&serv_adr), sizeof(serv_adr)) == -1) {
		printf("failed to bind socket\n");
	}

	if (listen(serv_sock, 3) == -1) {
		assert("failed to bind socket");
	}

	clnt_adr_sz = sizeof(clnt_adr);

	while(1) {
		sleep(1);
	}

	clnt_sock = accept(serv_sock, (struct sockaddr*)(&clnt_adr), &clnt_adr_sz);
	if (clnt_sock == -1) {
		std::cout << "Client is invalid" << std::endl;
	}
	else {
		std::cout << "Client is accepted" << std::endl;
	}
	close(clnt_sock);

	close(serv_sock);
	return 0;
}

