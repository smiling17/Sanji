#include <stdio.h> /* fprintf */
#include <stdlib.h> /* memset */
#include <string.h> /* strlen */
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30

int main () {
	int recv_sock;
	struct sockaddr_in recv_adr = {0}; 
	int str_len;
	char buf[BUF_SIZE];

	/* for network */
	unsigned int port = 10025;

	/* socket create */
	recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
	memset(&recv_adr, 0, sizeof(recv_adr));

	/* bind setting */
	recv_adr.sin_family = AF_INET;
	recv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	recv_adr.sin_port = htons(port);

	/* bind */
	if (bind(recv_sock, (struct sockaddr*)&recv_adr, sizeof(recv_adr)) == -1) {
		fprintf(stdout, "bind() error\n");
		return -1;
	}
	while (1) {
		str_len = recvfrom(recv_sock, buf, BUF_SIZE-1, 0, NULL, 0);
		if (str_len < 0 ) {
			break;
		}
		buf[str_len]=0;
		fputs(buf, stdout);
	}
	close(recv_sock);
	return 0;
}


