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
	int sock;
	char message[BUFSIZE];
	int message_len, recv_len, recv_num;
	struct sockaddr_in server_addr;

	if(argc != 3){
		printf("usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);

	if(sock == -1) error_handler("socket() error");

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		error_handler("connect() error");

	while (1){
		fputs("type message here that you want ('q' to quit): ", stdout);
		fgets(message, sizeof(message), stdin);
 		write(sock, message, strlen(message));
 		message_len = read(sock, message, BUFSIZE+21);
		message[message_len] = '\0';

		if(strcmp(message, "q\n") == 0) break;

		printf("message from server: %s\n", message);
	}

	close(sock);

	return 0;
}
