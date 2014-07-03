#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <errno.h>

#define SERVER_PORT 			8080
#define HEAD_BUFFER_SIZE 	8
#define BODY_BUFFER_SIZE 	1024

void error(char* msg) {
	fprintf(stderr, "ERROR: %s. (errno = %d)\n", msg, errno);
	exit(1);
}

int main(int argc, char *argv[]) 
{
	const int BUFFER_SIZE = HEAD_BUFFER_SIZE + BODY_BUFFER_SIZE;
	
	/* 변수 선언 */
	int sock;	//생성된 소켓번호(socket descriptor)를 저장할 변수
	struct sockaddr_in server, client;  //서버 주소정보를 담을 변수들
	int clientaddr_len;
	
	char head_buf[HEAD_BUFFER_SIZE]; 
	char body_buf[BODY_BUFFER_SIZE];	char buf[BUFFER_SIZE];	//송. 수신데이터를 담을 버퍼변수 (head_buf + body_buf)
	char ack_buf[HEAD_BUFFER_SIZE]; //acknoledgement를 받기위한 buffer
	
	/* 데이터 전송에 사용되  는 소켓을 생성하고, 생성된 소켓번호 descriptor(각 소켓간의 구분자)를 가져온다.*/
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1 ) 
	{
		error("socket() Fail");
	}
	
	/* 소켓 주소정보를 담고있는 sockaddr_in sturcture에 서버 호스트에 대한 정보를 저장한다. */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(SERVER_PORT);
	
	//상대 프로세스와 통신하려면 위에서 생성한 소켓에 주소를 부여해야 한다.
	//이때 socket descriptor를 이용한다.
	if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0 ) 
	{
		error("bind() Fail");
	}
	
	while(1) 
	{		
		/* 서버에서 처리해야하는 일을 수행하는 Business Logic부분 */
		char* file_name = "result.pdf";
		
		FILE* file = fopen(file_name, "a");

		if(file == NULL)
		{
			error("File cannot be opened on Server");
		} 
		else 
		{
			//버퍼변수 초기화
			bzero(buf, BUFFER_SIZE); 
			bzero(head_buf, HEAD_BUFFER_SIZE); 
			bzero(body_buf, BODY_BUFFER_SIZE); 
			bzero(ack_buf, HEAD_BUFFER_SIZE); 
			
			int fr_block_sz = 0;	//클라이언트에서 전달받는 buffer data크기
			clientaddr_len = sizeof(client);//client의 sockaddr의 크기를 저장 
			int recv_ack = 0, current_ack = 0;//acknoledgement를 저장할 변수.
			int write_sz; //전달받은 데이터를 파일에 쓸때, 작성된 데이터의 크기를 저장할 변수
			
			//모든 데이터를 클라이언트로부터 받는다.
			while((fr_block_sz = recvfrom(sock, buf, BUFFER_SIZE, 0, (struct sockaddr *)&client, &clientaddr_len)) > 0) 
			{
				//printf("in while\n"); //test
				
				//헤더버퍼를 추출
				for (int i = 0 ; i < HEAD_BUFFER_SIZE ; ++i) 
				{
					head_buf[i] = buf[i];
				}
				
				//바디버퍼를 추출
				for (int i = 0 ; i < fr_block_sz-HEAD_BUFFER_SIZE ; ++i) 
				{
					body_buf[i] = buf[HEAD_BUFFER_SIZE+i];
				}
				
				//헤더버퍼에서 문자열로 이루어진 숫자를 integer로 변환
				recv_ack = atoi(head_buf);
								
				//보내준 데이터가 현재 받아야하는 데이터순서와 일치할경우
				if (current_ack == recv_ack)
				{
WRITE_DATA_ON_FILE:
					//데이터를 파일에 쓴다.
					write_sz = fwrite(body_buf, sizeof(char), fr_block_sz-HEAD_BUFFER_SIZE, file);
					
					//써야할 buffer사이즈만큼 작성에 성공했을경우
					if (write_sz == fr_block_sz-HEAD_BUFFER_SIZE)
					{
						//다음번 offset을 증가시킨다.
						++current_ack;
						
					//작성에 실패할 경우
					} else {
						goto WRITE_DATA_ON_FILE;
					}				
				}
				//현재 전달받아야하는 offset을 버퍼에 저장한다.
				sprintf(ack_buf, "%d", current_ack);
				
				//offset에 대한 acknoledgement를 클라이언트에게 회신해준다.
				sendto(sock, ack_buf, HEAD_BUFFER_SIZE, 0, (struct sockaddr *)&client, sizeof(client));
				
				//다음 while문을 위한 버퍼초기화
				bzero(buf, BUFFER_SIZE);
				bzero(body_buf, BODY_BUFFER_SIZE);
				bzero(head_buf, HEAD_BUFFER_SIZE);
				
				//전달받은 데이터가 전혀없거나,
				//마지막 데이터를 수신했을경우
				//질문, break를 하지 않을경우 데이터가 없어지는 현상
				if (fr_block_sz == 0 || fr_block_sz != BUFFER_SIZE) 
				{
					//printf("recv_ack : %d\n", recv_ack);
					//printf("current_ack : %d\n", current_ack);
					break;
				}
				
				//데이터 수신에 실패할 경우
				if(fr_block_sz < 0)
				{
					if (errno == EAGAIN)
					{
						error("recv() timed out");
					}
					else
					{
						error("recv() failed due to errno = %d");
					}
				}	
			}
			
			printf("Ok received from client!\n");
			fclose(file);
		}//else
	}
	close(sock);
}