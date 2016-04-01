#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <iostream>

#define BUFSIZE 1024

typedef struct thread_value
{
	int echo, sock, turn;
}thread_value;

void error_handler(const char *msg)
{
	std::cout<<msg<<std::endl;
	exit(1);
}

void *server_handler(void *arg)
{
	thread_value* ret = (thread_value*)arg;
	int message_len;
	int check_echo = ret -> echo;
	int client_sock = ret -> sock;
	int order = ret -> turn;
	char message[BUFSIZE];
	const char *rank;

	switch(order){
		case 1: {rank = "st"; break;}
		case 2:	{rank = "nd"; break;}
		case 3: {rank = "rd"; break;}
		default: {rank = "th"; break;}
	}

	std::cout<<order<<rank<<" client Connected"<<std::endl;

	while(1){
		for(int i=0; i<BUFSIZE; i++) message[i] = 0;

		message_len = read(client_sock, message, BUFSIZE);

		if(message_len == 0){
			std::cout<<order<<rank<<" client Disconnected\n"<<std::endl;
			break;
		}

		if(check_echo == 3) write(client_sock, message, message_len+1);

		std::cout<<order<<rank<<" client Message : "<<message<<std::endl;
	}

	close(client_sock);

	return (void*)NULL;
}

int main(int argc, char **argv)
{
	int server_sock, client_sock, client_addr_len;
	thread_value thread_data;
	thread_data.echo = argc;
	thread_data.turn = 0;
	struct sockaddr_in server_addr, client_addr;
	pthread_t server_threads;

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

	if(listen(server_sock, 10) == -1)
		error_handler("Listen error");

	while(1){
		client_addr_len = sizeof(client_addr_len);
		client_sock = accept(server_sock, (struct sockaddr *)&client_addr, (socklen_t*)&client_addr_len);

		if(client_sock == -1)
			error_handler("Accept error");

		thread_data.turn++;
		thread_data.sock = client_sock;

		if(pthread_create(&server_threads, NULL, server_handler, (void*)&thread_data) < 0)
			error_handler("Thread error");

		pthread_detach(server_threads);
	}

	close(server_sock);

	return 0;
}

