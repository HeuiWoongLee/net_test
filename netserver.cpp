#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <pthread.h>

#define BUFSIZE 1024

void *server_handler(void *arg);

void error_handler(const char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void *server_handler(void *arg)
{
	std::cout<<"Connected"<<std::endl;
	pthread_detach(pthread_self());

	int message_len;
	int client_sock = *(int*)arg;
	char message[BUFSIZE];

	while(1){
		for(int i=0; i<BUFSIZE; i++) message[i] = 0;

		message_len = read(client_sock, message, BUFSIZE);

		if(message_len == 0){
			std::cout<<"Disconnected\n"<<std::endl;
			break;
		}

		write(client_sock, message, message_len);
		std::cout<<"Receive Message : "<<message<<std::endl;
		}

	close(client_sock);

	return (void*)NULL;
}

int main(int argc, char **argv)
{
	int server_sock, client_sock, /*message_len, */client_addr_len, status;//, pid;
	//char message[BUFSIZE];
	struct sockaddr_in server_addr, client_addr;
	pthread_t threads;

	if(argc != 2 && (argc != 3 || strcmp(argv[2], "-echo"))){
		std::cout<<"Usage : "<<argv[0]<<" <port> [-echo]"<<std::endl;
		exit(1);
	}

	server_sock = socket(PF_INET, SOCK_STREAM, 0);

	if(server_sock == -1)
		error_handler("Socket error");

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));

	if(bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		error_handler("Bind error");

	if(listen(server_sock, 5) == -1)
		error_handler("Listen error");

	//signal(SIGCHLD, SIG_IGN);

	while(1){
		client_addr_len = sizeof(client_addr_len);
		client_sock = accept(server_sock, (struct sockaddr *)&client_addr, (socklen_t*)&client_addr_len);

		if(client_sock == -1)
			error_handler("Accept error");

		pthread_create(&threads, NULL, server_handler, (void*)&client_sock);
	}

	close(server_sock);

	return 0;
}
