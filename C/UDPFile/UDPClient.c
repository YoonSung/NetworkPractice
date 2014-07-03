#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <errno.h>

#define SERVER_IP 				"127.0.0.1"
#define SERVER_PORT 			8080
#define HEAD_BUFFER_SIZE 	8
#define BODY_BUFFER_SIZE 	1024

void error(char* msg) {
	fprintf(stderr, "ERROR: %s. (errno = %d)\n", msg, errno);
	exit(1);
}

int main(int argc, char *argv[]) 
{
	//네트워크로 전송할 총 데이터의 길이
	const int BUFFER_SIZE = HEAD_BUFFER_SIZE + BODY_BUFFER_SIZE;
	printf("BUFFER SIZE : %d\n", BUFFER_SIZE);
	//int BUFFER_SIZE = HEAD_BUFFER_SIZE + BODY_BUFFER_SIZE;
	
	/* 변수 선언 */
	int sock;	//생성된 소켓번호(socket descriptor)를 저장할 변수
	struct sockaddr_in server, client; //서버 주소정보를 담을 변수들
	int serveraddr_len;
	char head_buf[HEAD_BUFFER_SIZE];
	char body_buf[BODY_BUFFER_SIZE];
	char ack_buf[HEAD_BUFFER_SIZE];//acknoledgement를 받기위한 buffer
	char buf[BUFFER_SIZE]; //송. 수신데이터를 담을 버퍼변수 (head_buf + body_buf)
	
	/* 소켓 주소정보를 담고있는 sockaddr_in sturcture에 목표 호스트 서버에 대한 정보를 저장한다. */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(SERVER_IP);
	server.sin_port = htons(SERVER_PORT);
	
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
	
	
	/* 클라이언트에서 처리해야하는 일을 수행하는 Business Logic부분 */
	char* file_name = "reading01.pdf";
	//char* file_name = "/Users/next/Desktop/test.txt";
	FILE* file = fopen(file_name, "rb");

	if (file == NULL) {
		error("File not found");
		
	} else {
		//init
		bzero(buf, BUFFER_SIZE); //byte stream을 0으로 채운다.
		bzero(head_buf, HEAD_BUFFER_SIZE); //byte stream을 0으로 채운다.
		bzero(body_buf, BODY_BUFFER_SIZE); //byte stream을 0으로 채운다.
		bzero(ack_buf, HEAD_BUFFER_SIZE);
				
		int fs_block_sz;
		int send_data_sz;
		int current_ack = 0, recv_ack = 0;
		serveraddr_len = sizeof(server);
		

		while((fs_block_sz = fread(body_buf, sizeof(char), BODY_BUFFER_SIZE, file)) > 0)
		{
			//head buffer에 현재 데이터의 번호를 저장 (ack저장)
			sprintf(head_buf, "%d", current_ack);
			
			//entire buffer에 헤더버퍼, body버퍼를 더한다.
			memcpy(buf, head_buf, (HEAD_BUFFER_SIZE) * sizeof(char));
			memcpy(buf+HEAD_BUFFER_SIZE, body_buf, (BODY_BUFFER_SIZE) * sizeof(char));

SEND_DATA:
			//전체데이터 전송
			send_data_sz = sendto(sock, buf, fs_block_sz+HEAD_BUFFER_SIZE, 0, (struct sockaddr *)&server, sizeof(server));
			//printf("send_data_sz : %d\n", send_data_sz); //test
			
			//데이터 전송에 실패할 경우
			if (send_data_sz < 0)
			{
				error("Fail To Send File");
			}
			
			//연결형은 recv, 비연결형은 recvfrom을 사용
			recvfrom(sock, ack_buf, HEAD_BUFFER_SIZE, 0, (struct sockaddr *)&server, &serveraddr_len);
			
			//헤더버퍼에서 문자열로 이루어진 숫자를 integer로 변환
			recv_ack = atoi(ack_buf);
			//printf("current_ack : %d\n", current_ack);//test
			//printf("recv_ack : %d\n", recv_ack);//test
			
			//실제로 수신자에게 보낸데이터 크기와, 파일로부터 읽은데이터 크기가 다를경우, 재전송한다.
			if (send_data_sz-HEAD_BUFFER_SIZE < fs_block_sz) 
			{
				goto SEND_DATA;	
			}
			
			//수신자에게 전달한 데이터의 수신이 성공적인것을 acknoledgement를 통해 확인한 경우
			if (current_ack +1 == recv_ack)
			{
				//보내야할 데이터 index를 증가시킨다.
				++current_ack;
			} else {
				//재전송한다
				goto SEND_DATA;
			}
			
			//다음 while문을 위한 버퍼초기화
			bzero(buf, BUFFER_SIZE);
			bzero(head_buf, HEAD_BUFFER_SIZE); 
			bzero(body_buf, BODY_BUFFER_SIZE);
			bzero(ack_buf, HEAD_BUFFER_SIZE);
		}
		printf("Ok File %s from Client was Sent!\n", file_name);
	}
	
	close(sock);
}