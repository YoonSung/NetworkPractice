#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <errno.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BODY_BUFFER_SIZE 256
#define HEAD_BUFFER_SIZE 8

void error(char* msg) {
	fprintf(stderr, "ERROR: %s. (errno = %d)\n", msg, errno);
	exit(1);
}

int main(int argc, char *argv[]) 
{
	const BUFFER_SIZE = HEAD_BUFFER_SIZE + BODY_BUFFER_SIZE;
	
	/* 변수 선언 */
	int sock;	//생성된 소켓번호(socket descriptor)를 저장할 변수
	struct sockaddr_in server, client; //서버 주소정보를 담을 변수들
	char send_buf[BUFFER_SIZE], recv_buf[BUFFER_SIZE]; //송. 수신데이터를 담을 버퍼변수
	int send_buf_len, recv_buf_len; //전송, 수신데이터 크기를 담을 변수
	int send_ack = 0, recv_ack = 0; //파일 송,수신의 acknoledgement를 확인하기 위한 seqeunce number
	
	/* 소켓 주소정보를 담고있는 sockaddr_in sturcture에 목표 호스트 서버에 대한 정보를 저장한다. */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(SERVER_IP);
	server.sin_port = htons(SERVER_PORT);
	
	/* 데이터 전송에 사용되는 소켓을 생성하고, 생성된 소켓번호 descriptor(각 소켓간의 구분자)를 가져온다.*/
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1 ) 
		error("socket create fail");
	
	/* 소켓 주소정보를 담고있는 sockaddr_in sturcture에 클라이언트 호스트에 대한 정보를 저장한다. */
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = htonl(INADDR_ANY);
	client.sin_port = htons(0);
	
	//상대 프로세스와 통신하려면 위에서 생성한 소켓에 주소를 부여해야 한다.
	//이때 socket descriptor를 이용한다.
	//본래 bind는 연결형서비스에서 서버에 해당하는 코드였으나, 비연결형 서비스는
	//각 packet별로 목표점 설정이 필`요하므로 클라이언트에도 bind와 같은 코드가 필요하다.
	if (bind(sock, (struct sockaddr *)&client, sizeof(client)) < 0) 
		error("Bind Address fail");
	
	/* 파일전송 */
	char* file_name = "reading01.pdf";
	FILE* file = fopen(file_name, "rb");
		
	if (file == NULL) {
		error("File not found");
			
	} else {
		bzero(send_buf, BUFFER_SIZE); //byte stream을 0으로 채운다.
		int fs_block_sz; 
		while((fs_block_sz = fread(send_buf, sizeof(char), BUFFER_SIZE, file)) > 0)
		{
SEND_DATA:			
			//서버로 데이터를 전송한다.
			send_buf_len = sendto(sock, send_buf, fs_block_sz + 1, 0, (struct sockaddr *)&server, sizeof(server));
			
			//블록만큼의 데이터전송이 실패할 경우
			if(send_buf_len < 0)
			{
				error("Faile To Send File");
			} 
			//블록만큼의 데이터전송이 성공할 경우
			else 
			{
				recv_buf_len = recvfrom(sock, recv_buf, BUFFER_SIZE, 0, (struct sockaddr *)0, (int *)0);
				printf("recv_buf : %s\n", recv_buf);
				recv_ack = atoi(recv_buf);
				
				if ( send_ack + 1 != recv_ack ) {
					goto SEND_DATA;
				} else {
					++send_ack;
				}
			}
			
			bzero(send_buf, BUFFER_SIZE);
			bzero(recv_buf, BUFFER_SIZE);
		}
		printf("Ok File %s from Client was Sent!\n", file_name);
	}
	fclose(file);
	close(sock);
}