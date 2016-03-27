#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>

#define BUFSIZE 1024
int count = 0;

sem_t sem_thread;

void error_handler(const char *msg)
{
	std::cout<<msg<<std::endl;
	exit(1);
}

void *client_handler(void *arg)
{
	int sock = *(int*)arg;
	int sem_count;
	char thread_message[BUFSIZE];

	sem_wait(&sem_thread);
	sem_count = count;
	sem_count += 1;
	count = sem_count;

	for(int i = 0; i < BUFSIZE; i++) thread_message[i] = 0;

	read(sock, thread_message, BUFSIZE);
	std::cout<<"Send to Server : "<<thread_message<<std::endl<<std::endl;
	sem_post(&sem_thread);
	sleep(10);
	pthread_exit(NULL);
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

	while(1){
		std::cout<<"Type message ('quit' to exit) : ";
		std::cin>>message;

		if(strcmp(message, "quit") == 0)
			break;

		write(sock, message, strlen(message));

		if(sem_init(&sem_thread, 0, 1) == -1)
			error_handler("Semaphore error");

		if(pthread_create(&client_threads, NULL, client_handler, (void*)&sock) < 0)
			error_handler("Thread error");

		pthread_detach(client_threads);
	}

	close(sock);
}
