#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<time.h>

#define TIME_PORT 8080

int main(int argc, char *argv[]) 
{
	int sock;
	struct sockaddr_in server, client;
	int server_len;
	int client_len = sizeof(client_len);
	char buf[256];
	int buf_len;
	time_t today;
	
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1 ) 
	{
		printf("socket error\n");
		//exit(1);
		return 1;
	}
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(TIME_PORT);
	
	if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0 ) 
	{
		printf("bind error\n");
		//exit(1);
		return 1;
	}
	
	while(1) 
	{
		buf_len = recvfrom(sock, buf, 256, 0, (struct sockaddr *)&client, &client_len);
		
		if ( buf_len < 0 ) {
			printf("recvfrom error\n");
			//exit(1);
			return 1;
		}
		
		printf("Server : Got %s\n", buf);
		
		time(&today);
		strcpy(buf, ctime(&today));
		sendto(sock, buf, strlen(buf) + 1, 0, (struct sockaddr *)&client, client_len);
	}
}