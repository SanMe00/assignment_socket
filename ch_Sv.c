#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 10000

char snBf[100];
char rcvBf[100];

int main(){
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int n;
	int chlen; //strlen -- 처리용
	char chlen_str[100]; //chlen을 write 하는 도중 sprintf을 위한 변수 
	int chcmp; //	strcmp -- 처리용
	char cmp_str[100];	//cmp결과 담을 변수

	int compareCmp(char * objectBuff); //strcmp 실행을 위한 함수

	// 1. 서버 소켓 생성
	//서버 소켓 = 클라이언트의 접속 요청을 처리(허용)해 주기 위한 소켓
	s_socket = socket(PF_INET, SOCK_STREAM, 0); //TCP/IP 통신을 위한 서버 소켓 생성
	
	//2. 서버 소켓 주소 설정
	memset(&s_addr, 0, sizeof(s_addr)); //s_addr의 값을 모두 0으로  초기화
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP 주소 설정
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);

	//3. 서버 소켓바인딩
	if(bind(s_socket,(struct sockaddr *) &s_addr, sizeof(s_addr)) == -1){ 
		//바인딩 작업 실패 시, Cannot Bind 메시지 출력 후 프로그램 종료
		printf("Cannot Bind\n");
		return -1;
	}
	
	//4.listen() 함수 실행
	if(listen(s_socket, 5) == -1){
		printf("listen Fail\n");
		return -1;
	}

	//5. 클라이언트 요청 처리
	// 요청을 허용한 후, Hello World 메세지를 전송함
	while(1){ //무한 루프
		len = sizeof(c_addr);
		printf("클라이언트 접속을 기다리는 중....\n");
		c_socket = accept(s_socket, (struct sockaddr *)&c_addr, &len); 
		//클라이언트의 요청이 오면 허용(accept)해 주고, 해당 클라이언트와 통신할 수 있도록 클라이언트 소켓(c_socket)을 반환함.
		printf("/client is connected\n");
		printf("클라이언트 접속 허용\n");

		while(1){	

			n=read(c_socket, rcvBf, sizeof(rcvBf));
			rcvBf[n]='\0'; //개행문자포함시켜서 오류막기
			printf("Received Data: %s\n",rcvBf);  // 체크용도

			if(strncasecmp(rcvBf,"quit",4)==0 || strncasecmp(rcvBf,"kill server",11)==0)
				break;

			if(strncasecmp(rcvBf,"안녕하세요\n",n)==0||strncasecmp(rcvBf,"안녕하세요.\n",n)==0){
				strcpy(snBf,"안녕하세요. 만나서 반가워요!\n");
				write(c_socket,snBf,strlen(snBf));
			}
			else if(strncasecmp(rcvBf,"이름이 뭐야?\n",n)==0||strncasecmp(rcvBf,"이름이뭐야?\n",n)==0){
				strcpy(snBf,"내 이름은 챗봇이야.\n");
				write(c_socket,snBf,strlen(snBf));			
			}
			else if(strncasecmp(rcvBf,"몇 살이야?\n",n)==0||strncasecmp(rcvBf,"몇살이야?\n",n)==0){
				strcpy(snBf,"나는 1살이야!.\n");
				write(c_socket,snBf,strlen(snBf));
			}
			else if(strncasecmp(rcvBf,"strlen",6)==0){
				chlen=strlen(rcvBf)-8;
				sprintf(chlen_str,"문자열의 길이= %d\n",chlen);
				strcpy(snBf,chlen_str);
				write(c_socket,snBf,strlen(snBf));
			}
			else if(strncasecmp(rcvBf,"strcmp",6)==0){
				chcmp= compareCmp(rcvBf);
				if(chcmp==0){
					sprintf(cmp_str,"비교결과= %d(일치)\n",chcmp);
					strcpy(snBf,cmp_str);
					write(c_socket,snBf,strlen(snBf));
				}
				else if(chcmp!=0){
					sprintf(cmp_str,"비교결과= %d(불일치)\n",chcmp);
					strcpy(snBf,cmp_str);
					write(c_socket,snBf,strlen(snBf));
				}
			}
			else
				write(c_socket, rcvBf, n); //모든 조건을 만족하지 않는경우 => 에코
		}
		close(c_socket);
		if(strncasecmp(rcvBf,"kill server",11)==0)
			break;

	}
	close(s_socket);
	return 0;	
}

// strcmp를 위해 작성한 함수. 입력받은 문장을 공백기준으로 둘로 나눠 비교한다.
int compareCmp(char * objectBuff){
	int lcv=7; //loop control value, strcmp와 " " 제외한 7번부터 시작.
	int length=strlen(rcvBf);	// 마지막  " " 포함한 전체길이
	int stop1=0; // 분기점 기록
	int stop2=0; // 두번째 분기점 기록
	int fin;
	char obj1[100]={0,};
	char obj2[100]={0,};

	while(lcv<length){
		if(objectBuff[lcv]==' '){
			stop1=lcv;
		}
		else if(&objectBuff[lcv]=="\0"){
			stop2=lcv;
		}
		lcv++;
	}

	lcv=0;	//재활용을 위한 초기화 

	while(lcv<stop1){
		//strcat(obj1,objectBuff[lcv]); 문자열과 문자 관계라 작동안함
		int i=strlen(obj1);
		obj1[i]=objectBuff[lcv];
		lcv++;
	}

	lcv++;	//stop1지점을 건너 다음 순번부터 시작

	while(lcv<stop2){
		//strcat(obj2,objectBuff[lcv]);
		int i=strlen(obj2);
		obj2[i]=objectBuff[lcv];
		lcv++;
	}
	
	strcat(obj1,"\0");
	strcat(obj2,"\0");

	if(strcmp(obj1,obj2)==0){
		fin=0;	
	}
	else if (strcmp(obj1,obj2)!=0){
		fin=strcmp(obj1,obj2);
		printf("%d %d\n",strlen(obj1),strlen(obj2));
	}
	
	return fin;
}

