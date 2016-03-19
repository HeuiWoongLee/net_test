#include <sys/types.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 1024

void error_handler(const char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

int main(int argc, char **argv)
{
	int sock, message_len, recv_len, recv_num;
	char message[BUFSIZE];
	struct sockaddr_in server_addr;

	if(argc != 3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);

	if(sock == -1) error_handler("Socket error");

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		error_handler("Connect error");

	while(1){
		fputs("Type message ('quit' to exit) : ", stdout);
		fgets(message, sizeof(message), stdin);

		if(strcmp(message, "quit\n") == 0) break;

		write(sock, message, strlen(message));
 		message_len = read(sock, message, BUFSIZE);
		printf("Send to server : %s\n", message);
	}

	close(sock);

	return 0;
}
