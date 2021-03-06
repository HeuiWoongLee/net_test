#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <iostream>

#define BUFSIZE 1024

void error_handler(const char *msg)
{
	std::cout<<msg<<std::endl;
	exit(1);
}
 
void *client_handler(void *arg)
{
	int check;
	int sock = *(int*)arg;
	char thread_message[BUFSIZE];

	while(1){
		check = read(sock, thread_message, BUFSIZE);

		if(check == 0) break;

		std::cout<<"Send to Server : "<<thread_message<<std::endl<<std::endl;
	}
}
 
int main(int argc, char **argv)
{
	int sock, thread_id;
	char message[BUFSIZE];
	struct sockaddr_in server_addr;
	pthread_t client_threads;

	if(argc != 3){
		std::cout<<"Usage : "<<argv[0]<<" <IP> <port>"<<std::endl;
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);

	if(sock == -1)
		error_handler("Socket error");

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		error_handler("Connect error");

	if(pthread_create(&client_threads, NULL, client_handler, (void*)&sock) < 0)
		error_handler("Thread error");

	while(1){
		usleep(1000);

		for(int i=0; i<BUFSIZE; i++) message[i] = 0;

		std::cout<<"Type message ('quit' to exit) : ";
		std::cin>>message;

		write(sock, message, strlen(message)+1);

		if(strcmp(message, "quit") == 0) break;
	}

	close(sock);

	pthread_detach(client_threads);
}

