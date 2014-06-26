#include<sys/types.h>
#include<sys/socket.h>

#include<netinet/in.h>

#define TIME_SERVER "127.0.0.1"
#define TIME_PORT 8080

int main(int argc, char *argv[]) 
{
	int sock;
	struct sockaddr_in server;
	char buf[256];
	
	/* Get the Socket file descriptor */
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket error\n");
		//exit(1);
		return 1;
	}
	
	/* Fill the socket address struct */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);
	
	/* Try to connect the remote */
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		printf("connect error\n");
		//exit(1);
		return 1;
	}
	
	if (recv(sock, buf, sizeof(buf), 0) == -1)
	{
		printf("receive error\n");
		//exit(1);
		return 1;
	}
	
	printf("receive complete\n");
	printf("Time information from server is %s\n", buf);
	close(sock);
}