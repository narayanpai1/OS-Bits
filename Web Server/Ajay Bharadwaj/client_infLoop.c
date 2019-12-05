#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#define MESSAGE_SIZE 200
#define FILE_NAME "test.txt"

int SERV_PORT;
char* connLoc;

void* makeConnection(void* arg)
{
	//CREATING A SOCKET
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	//CONNECTING TO THE SERVER
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, connLoc, &servaddr.sin_addr);
	if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) == -1)
	{
		printf("Error during connecting\n");
		close(sockfd);
		pthread_exit(NULL);
	}
	printf("Connected\n");

	//SENDING THE FILE NAME TO THE SERVER
	char fileName[MESSAGE_SIZE] = FILE_NAME;
	write(sockfd, fileName, sizeof(fileName));

	char success;
	read(sockfd, &success, 1);

	//IF THE FILE DOES NOT EXISTS ON THE SERVER
	if (!success)
	{
		printf("File does not exist\nClosing the connection\n");
		close(sockfd);
		pthread_exit(NULL);
	}

	//OBTAINING THE FILE FROM THE SERVER ALONG WITH STATISTICS ABOUT THE REQUEST
	char input[MESSAGE_SIZE];
	while (read(sockfd, input, MESSAGE_SIZE))
	{
		printf("%s", input);
	}

	printf("\n\nClosing the connection\n\n");
	close(sockfd);
	pthread_exit(NULL);
}

int main(int argc, char** argv)
{
	//SETTING THE PORT NUMBER
	printf("Enter the port number: ");
	scanf("%d", &SERV_PORT);

	connLoc = argv[1];

	pthread_t threadId;
	int a = 0;
	while (a < 20)
	{
		pthread_create(&threadId, NULL, makeConnection, NULL);
		a++;
	}

	scanf("%d", &a);
}