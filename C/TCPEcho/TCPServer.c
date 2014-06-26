#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#include<time.h>

#define TIME_PORT 8080

int main(int argc, char *argv[]) {
	
	/* 변수 선언 */
	int sock, sock2; //생성된 소켓번호(socket descriptor)를 저장할 변수들. 클라이언트와 서버 하나씩이다.
	struct sockaddr_in server, client; //서버, 클라이언트 주소정보를 담을 변수
	int len;	//질문,
	char buf[256];	//수신, 송신데이터를 담을 버퍼
	time_t today;

	/* 데이터 전송에 사용되는 소켓을 생성하고, 생성된 소켓번호 descriptor(각 소켓간의 구분자)를 가져온다.*/
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket error");
		//exit(1);
		return 1;
	}
	
	/* 소켓 주소정보를 담고있는 sockaddr_in sturcture에 목표 호스트에 대한 정보를 저장한다. */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(TIME_PORT);

	//상대 프로세스와 통신하려면 위에서 생성한 소켓에 주소를 부여해야 한다.
	//이때 socket descriptor를 이용한다.
	if (bind(sock, (struct sockaddr *)&server, sizeof(server))) 
	{
		printf("bind error");
		//exit(1);
		return 1;
	}

	//연결형 서비스에서 사용하는 함수. (질문)
	if (listen(sock, 5)) 
	{
		printf("listen error");
		//exit(1);
		return 1;
	}
	
	while(1) 
	{
		//연결형 서비스에서 클라이언트의 연결요청을 받기 위해 사용하는 함수.
		//요청이 들어올때까지 기다리고, 클라이언트 프로세스의 connect()요청이 발생하면 연결이 설정된다.
		//연결이 발생하면 두번째 인자에 클라이언트의 주소값을 저장한다.
		//또한 성공적인 연결을 통해 socket()함수로 생성한 원래 소켓 이외의 별도소켓이 새로 만들어지며
		//accept함수의 리턴데이터로 생성된 socket descriptor가 반환된다.
		//이 descriptor는 요청 클라이언트와 통신할 때 사용된다.
		if ((sock2 = accept(sock, (struct sockaddr *)&client, &len)) == -1) {
			printf("accept error");
			//exit(1);
			return 1;
		}
		
		time(&today);
		strcpy(buf, ctime(&today));
		
		//연결형 서비스에서 데이터전송에 사용하는 함수. 비연결형 서비스에서는 sendto 함수를 사용한다.
		//첫번쨰 인자는 클라이언트와 서버 사이의 연결설정이 완료된 상태의 소켓이다. 
		//즉, 서버accept함수와 클라이언트 connecti()함수가 성공적으로 실행된 후의 소켓 descriptor이다.
		send(sock2, buf, strlen(buf) + 1, 0);
		close(sock2);
	}
	//질문
	close(sock);
}