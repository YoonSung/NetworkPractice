#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <errno.h>

#define TIME_SERVER "127.0.0.1"
#define TIME_PORT 8080
#define HEAD_BUFFER_SIZE 8
#define BODY_BUFFER_SIZE 1024

void error(char* msg) {
	fprintf(stderr, "ERROR: %s. (errno = %d)\n", msg, errno);
	exit(1);
}

int main(int argc, char *argv[]) 
{
	//네트워크로 전송할 총 데이터의 길이
	//const int BUFFER_SIZE = HEAD_BUFFER_SIZE + BODY_BUFFER_SIZE;
	int BUFFER_SIZE = BODY_BUFFER_SIZE;
	
	/* 변수 선언 */
	int sock;	//생성된 소켓번호(socket descriptor)를 저장할 변수
	struct sockaddr_in server, client; //서버 주소정보를 담을 변수들
	char head_buf[HEAD_BUFFER_SIZE];
	char body_buf[BODY_BUFFER_SIZE];
	char recv_buf[HEAD_BUFFER_SIZE];
	char buf[BUFFER_SIZE]; //송. 수신데이터를 담을 버퍼변수
	int buf_len; //전송, 수신데이터 크기를 담을 변수
	
	/* 소켓 주소정보를 담고있는 sockaddr_in sturcture에 목표 호스트 서버에 대한 정보를 저장한다. */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(TIME_SERVER);
	server.sin_port = htons(TIME_PORT);
	
	/* 데이터 전송에 사용되는 소켓을 생성하고, 생성된 소켓번호 descriptor(각 소켓간의 구분자)를 가져온다.*/
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1 ) 
	{
		printf("socket error\n");
		//exit(1);
		return 1;
	}
	
	/* 소켓 주소정보를 담고있는 sockaddr_in sturcture에 클라이언트 호스트에 대한 정보를 저장한다. */
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = htonl(INADDR_ANY);
	client.sin_port = htons(0);
	
	//상대 프로세스와 통신하려면 위에서 생성한 소켓에 주소를 부여해야 한다.
	//이때 socket descriptor를 이용한다.
	//본래 bind는 연결형서비스에서 서버에 해당하는 코드였으나, 비연결형 서비스는
	//각 packet별로 목표점 설정이 필요하므로 클라이언트에도 bind와 같은 코드가 필요하다.
	if (bind(sock, (struct sockaddr *)&client, sizeof(client)) < 0) 
	{
		printf("bind error\n");
		//exit(1);
		return 1;
	}
	
	
	/* 클라이언트에서 처리해야하는 일을 수행 */
	char* file_name = "reading01.pdf";
	FILE* file = fopen(file_name, "rb");

	if (file == NULL) {
		error("File not found");
		
	} else {
		//init
		bzero(buf, BUFFER_SIZE); //byte stream을 0으로 채운다.
		bzero(head_buf, HEAD_BUFFER_SIZE); //byte stream을 0으로 채운다.
		bzero(body_buf, BODY_BUFFER_SIZE); //byte stream을 0으로 채운다.
		
		int fs_block_sz;
		int send_data_sz;
		int ack = 2;
		
		//while((fs_block_sz = fread(body_buf, sizeof(char), BODY_BUFFER_SIZE, file)) > 0)
		while((fs_block_sz = fread(buf, sizeof(char), BUFFER_SIZE, file)) > 0)
		{
			//printf("fs_block_sz : %d\n", fs_block_sz);
			/*
			sprintf(head_buf, "%d", ack);
			memcpy(buf, head_buf, (HEAD_BUFFER_SIZE) * sizeof(char));
			memcpy(buf+HEAD_BUFFER_SIZE, body_buf, (BODY_BUFFER_SIZE) * sizeof(char));
			
			for (int i = 0 ; i < BUFFER_SIZE ; ++i)
			{
					printf("%d : %c\n", i, buf[i]);
			}
SEND_DATA:
			*/
			
			send_data_sz = sendto(sock, buf, fs_block_sz, 0, (struct sockaddr *)&server, sizeof(server));
			printf("send_data_sz : %d\n", send_data_sz);
			//buf_len = recvfrom(sock, buf, 256, 0, (struct sockaddr *)0, (int *)0); //연결형은 recv, 비연결형은 recvfrom을 사용
			
			if (send_data_sz < 0)
			{
				error("Faile To Send File");
			}
			
			if (send_data_sz < fs_block_sz) 
			{
				//goto SEND_DATA;	
			}
			bzero(buf, BUFFER_SIZE);
		}
		printf("Ok File %s from Client was Sent!\n", file_name);
	}
	
	/*
	buf[0] = '?'; buf[1] = '\0';
	//비연결형 서비스에서 데이터전송에 사용하는 함수. 연결형 서비스에서는 send 함수를 사용한다.
	//첫번쨰 인자는 클라이언트와 서버 사이의 연결설정 과정을 거치지 않은 socket descriptor이다.
	buf_len = sendto(sock, buf, strlen(buf) + 1, 0, (struct sockaddr *)&server, sizeof(server));
	
	if (buf_len < 0) 
	{
		printf("sendto error\n");
		//exit(1);
		return 1;		
	}
	*/
	/* 소켓을 통해 읽은 데이터는 buf가 가리키는 공간에 저장되며, 데이터 크기가 반환된다. */
	/*
	buf_len = recvfrom(sock, buf, 256, 0, (struct sockaddr *)0, (int *)0); //연결형은 recv, 비연결형은 recvfrom을 사용
	
	if ( buf_len < 0 )
	{
		printf("recvfrom error\n");
		//exit(1);
		return 1;				
	}
	
	printf("Time information from server is %s", buf);
	*/
	close(sock);
}