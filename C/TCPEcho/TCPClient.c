#include<sys/types.h>
#include<sys/socket.h>

#include<netinet/in.h>

#define TIME_SERVER "127.0.0.1"
#define TIME_PORT 8080

int main(int argc, char *argv[]) 
{
	/* 변수 선언 */
	int sock; //생성된 소켓번호(socket descriptor)를 저장할 변수
	struct sockaddr_in server; //서버 주소정보를 담을 변수
	char buf[256]; //수신데이터를 담을 버퍼
	
	/* 데이터 전송에 사용되는 소켓을 생성하고, 생성된 소켓번호 descriptor(각 소켓간의 구분자)를 가져온다.*/
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket error\n");
		//exit(1);
		return 1;
	}
	
	/* 소켓 주소정보를 담고있는 sockaddr_in sturcture에 목표 호스트에 대한 정보를 저장한다. */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(TIME_SERVER);
	server.sin_port = htons(TIME_PORT);
	
	/* 클라이언트 프로세스가 서버 프로세스에 연결요청을 한다. */
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		printf("connect error\n");
		//exit(1);
		return 1;
	}
	
	/* 소켓을 통해 읽은 데이터는 buf가 가리키는 공간에 저장되며, 데이터 크기가 반환된다. */
	if (recv(sock, buf, sizeof(buf), 0) == -1) //연결형은 recv, 비연결형은 recvfrom을 사용
	{
		printf("receive error\n");
		//exit(1);
		return 1;
	}
	
	printf("receive complete\n");
	printf("Time information from server is %s\n", buf);
	close(sock);
}