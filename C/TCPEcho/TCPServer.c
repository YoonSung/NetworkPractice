#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#include<time.h>

#define TIME_PORT 8080

int main(int argc, char *argv[]) {
	int sock, sock2;
	struct sockaddr_in server, client;
	int len;
	char buf[256];
	time_t today;
	printf("1\n");
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket error");
		//exit(1);
		return 1;
	}
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(TIME_PORT);
	printf("2\n");	
	if (bind(sock, (struct sockaddr *)&server, sizeof(server))) 
	{
		printf("bind error");
		//exit(1);
		return 1;
	}
	printf("3\n");
	if (listen(sock, 5)) 
	{
		printf("listen error");
		//exit(1);
		return 1;
	}
	
	while(1) 
	{
		printf("4\n");
		if ((sock2 = accept(sock, (struct sockaddr *)&client, &len)) == -1) {
			printf("accept error");
			//exit(1);
			return 1;
		}
		printf("accept!!!!\n");
		time(&today);
		strcpy(buf, ctime(&today));
		send(sock2, buf, strlen(buf) + 1, 0);
		//close(sock2);
	}
}