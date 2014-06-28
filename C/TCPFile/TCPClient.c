#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<errno.h>
#include	<netinet/in.h>

#define SERVER_IP 		"127.0.0.1"
#define SERVER_PORT 		8080
#define BUFFER_SIZE 		256

void error(char* msg) {
	fprintf(stderr, "ERROR: %s. (errno = %d)\n", msg, errno);
	exit(1);
}

int main(int argc, char *argv[]) 
{
	/* 변수 선언 */
	int sock; //생성된 소켓번호(socket descriptor)를 저장할 변수
	struct sockaddr_in server; //서버 주소정보를 담을 변수
	char buf[BUFFER_SIZE]; //수신데이터를 담을 버퍼
	
	/* 데이터 전송에 사용되는 소켓을 생성하고, 생성된 소켓번호 descriptor(각 소켓간의 구분자)를 가져온다.*/
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		error("socket create fail");
	
	/* 소켓 주소정보를 담고있는 sockaddr_in sturcture에 목표 호스트에 대한 정보를 저장한다. */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(SERVER_IP);
	server.sin_port = htons(SERVER_PORT);
	
	/* 클라이언트 프로세스가 서버 프로세스에 연결요청을 한다. */
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
		error("connect Server fail");
	
	/* 클라이언트에서 처리해야하는 일을 수행 */
	char* file_name = "reading01.pdf";
	FILE* file = fopen(file_name, "rb");
	
	if (file == NULL) {
		error("File not found");
	} else {
		bzero(buf, BUFFER_SIZE); //byte stream을 0으로 채운다.
		int fs_block_sz; 
		while((fs_block_sz = fread(buf, sizeof(char), BUFFER_SIZE, file)) > 0)
		{
			if(send(sock, buf, fs_block_sz, 0) < 0)
			{
				error("Faile To Send File");
			}
			bzero(buf, BUFFER_SIZE);
		}
		printf("Ok File %s from Client was Sent!\n", file_name);
	}

	close(sock);
}