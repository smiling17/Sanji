#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
//void error_handling(char *message);

int main() {
	int send_sock = -1;
	struct sockaddr_in broad_adr = {0};
	char buf[BUF_SIZE];
	int so_brd=1;

	/* Input params */
	const char *broad_ip = "255.255.255.255";
	unsigned int port = 10025;
	const char *message = "my name is sangjun lee\n";

	/* socket create */
	send_sock=socket(PF_INET, SOCK_DGRAM, 0);
	memset(&broad_adr, 0, sizeof(broad_adr));

	/* Before bind, set broad_adr (struct sockaddr_in ) */
	broad_adr.sin_family = AF_INET;
	broad_adr.sin_addr.s_addr=inet_addr(broad_ip);
	broad_adr.sin_port = htons(port);

	setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, (void*)&so_brd, sizeof(so_brd));

	/* UDP send thru socket */
	fprintf(stdout, "SOCK SEND START\n");
	sendto(send_sock, message, strlen(buf),
			0, (struct sockaddr*)&broad_adr, sizeof(broad_adr));
	fprintf(stdout, "SOCK SEND END\n");
	close(send_sock);
	return 0;
}
