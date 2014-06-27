#include<stdio.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <errno.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 256

void error(char* msg) {
	fprintf(stderr, "ERROR: %s. (errno = %d)\n", msg, errno);
	exit(1);
}

int main(int argc, char *argv[]) 
{
	/*************** 변수 선언 ***************/
	int sock;	//생성된 소켓번호(socket descriptor)를 저장할 변수
	
	//서버 주소정보를 담을 변수들
	struct sockaddr_in server, client;
	
	//클라이언트 소켓주소정보의 크기를 담은 변수
	int client_len = sizeof(client);
	
	//송. 수신데이터를 담을 버퍼변수
	char send_buf[BUFFER_SIZE], recv_buf[BUFFER_SIZE];	
	//전송, 수신데이터 크기를 담을 변수
	int send_buf_len, recv_buf_len;
	
	//파일 송,수신의 acknoledgement를 확인하기 위한 seqeunce number
	int ack;

	/***** 데이터 전송에 사용되는 소켓을 생성하고, 생성된 소켓번호 descriptor(각 소켓간의 구분자)를 가져온다.*****/
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1 ) 
	{
		printf("socket error\n");
		//exit(1);
		return 1;
	}
	
	/* 소켓 주소정보를 담고있는 sockaddr_in sturcture에 서버 호스트에 대한 정보를 저장한다. */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(SERVER_PORT);
	
	//상대 프로세스와 통신하려면 위에서 생성한 소켓에 주소를 부여해야 한다.
	//이때 socket descriptor를 이용한다.
	if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0 ) 
	{
		printf("bind error\n");
		//exit(1);
		return 1;
	}
	
	while(1) 
	{
		/* 클라이언트로부터 파일을 전송받는다. */
		char* file_name = "result.pdf";
		FILE* file = fopen(file_name, "a");

		if(file == NULL)
		{
			error("File cannot be opened on Server");
		} 
		else 
		{
			bzero(send_buf, BUFFER_SIZE);
			bzero(recv_buf, BUFFER_SIZE);
			int fr_block_sz = 0;
			ack = 0;
			
			/* 소켓을 통해 읽은 데이터는 buf가 가리키는 공간에 저장되며, 데이터 크기가 반환된다. */
			//연결형은 recv, 비연결형은 recvfrom을 사용
			while((fr_block_sz = recvfrom(sock, recv_buf, BUFFER_SIZE, 0, (struct sockaddr *)&client, &client_len)) > 0) 
			{
				int write_sz = fwrite(recv_buf, sizeof(char), fr_block_sz, file);
				
				//파일 데이터작성에 실패할 경우
				if (write_sz < fr_block_sz)
				{
					error("Write File Fail");
				}
				else 
				{
					ack++;
					//strcpy(send_buf, ack);
					sprintf(send_buf, "%d", ack);

					//비연결형 서비스에서 데이터전송에 사용하는 함수. 연결형 서비스에서는 send 함수를 사용한다.
					//첫번쨰 인자는 클라이언트와 서버 사이의 연결설정 과정을 거치지 않은 socket descriptor이다.
					sendto(sock, send_buf, strlen(send_buf) + 1, 0, (struct sockaddr *)&client, client_len);
				}
				
				bzero(send_buf, BUFFER_SIZE);
				bzero(recv_buf, BUFFER_SIZE);
			}
					
			if (fr_block_sz < 0)
			{
				if (errno == EAGAIN)
				{
					printf("recv() timed out.\n");
				}
				else
				{
					fprintf(stderr, "recv() failed due to errno = %d\n", errno);
					exit(1);
				}
			}
			printf("Ok received from client!\n");
			fclose(file); 
		}//else
	}
}