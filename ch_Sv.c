#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 10000

char snBf[100];
char rcvBf[100];
// sizeof(buffer) => 100 배열의 크기를 의미
// strlen(buffer) => 만약 hi라면 2. 내용의 길이를 의미

int main(){
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int n;
	int chlen; //strlen -- 처리용
	char chlen_str[100]; //chlen을 write 하는 도중 sprintf을 위한 변수 
	int chcmp; //	strcmp -- 처리용
	int result;
	char cmp_str[100];	//cmp결과 담을 변수

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
			//읽어올 때는 정확한 크기를 모르기 때문에 sizeof로 버퍼 전체의 크기만큼 읽어들이지만, read 할 때는 명확한 길이를 알기 때문에 strlen을 이용한다.
			n=read(c_socket, rcvBf, sizeof(rcvBf));
			rcvBf[n-1]='\0'; //개행문자포함를 삭제! strcmp에서 오류나는 것 해결
			printf("Received Data: %s\n",rcvBf);  // 체크용도용
			if(strncasecmp(rcvBf,"quit",4)==0 || strncasecmp(rcvBf,"kill server",11)==0)
				break;
			//아래처럼 ! 이용하여 표현가능
			//원래는 안녕하세요\n을 strncasecmp의 두 번째 인자로 넣었으나 위에서 코드를 제거하니 안해도 되는것같다.
			else if(!strncasecmp(rcvBf,"안녕하세요",strlen("안녕하세요"))|| !strncasecmp(rcvBf,"안녕하세요.",strlen("안녕하세요."))){
				strcpy(snBf,"안녕하세요. 만나서 반가워요!\n");
			}
			else if(!strncasecmp(rcvBf,"이름이 뭐야?",strlen("이름이 뭐야?"))|| !strncasecmp(rcvBf,"이름이뭐야?",strlen("이름이뭐야?"))){
				strcpy(snBf,"제 이름은 챗봇이에요.\n");			
			}
			else if(strncasecmp(rcvBf,"몇 살이야?\n",n)==0||strncasecmp(rcvBf,"몇살이야?\n",n)==0){
				strcpy(snBf,"저는 1살이에요!\n");
			}
			else if(!strncasecmp(rcvBf,"strlen ",strlen("strlen "))){
				chlen=strlen(rcvBf)-7;
				sprintf(chlen_str,"문자열의 길이는 %d입니다.\n",chlen);	//strlen(chlen_str)-7 교수님 방식, 개행제외하려면 -1 추가라고하신다
				strcpy(snBf,chlen_str);
			}
			else if(!strncasecmp(rcvBf,"strcmp ",strlen("strcmp "))){
				char *ptr=strtok(rcvBf," ");
				char *str[3];
				int index=0;
				while(ptr != NULL){
					str[index]=ptr;
					printf("str[%d] = %s\n",index,str[index]);
					index++;
					ptr=strtok(NULL," ");
				}
				if(index < 3)
					strcpy(snBf,"문자열 비교를 위해서는 2개의 문자열이 필요합니다.");
				//str2[strlen(str2)-1]='\0'; 원래 방식에서 개행문자 처리 
				else if(!strcmp(str[1],str[2]))
					sprintf(snBf,"%s와 %s는 같은 문자열입니다.",str[1],str[2]);
				else
					sprintf(snBf,"%s와 %s는 다른 문자열입니다.",str[1],str[2]);	//sprintf는 버퍼에 저장하는 것일 뿐 실제로 출력하려면 printf 필요하다.
			}
			else if(!strncasecmp(rcvBf,"readfile ",strlen("readfile "))){	// 파일 출력
				FILE *fp;
				char bf[255];
				char *ptr=strtok(rcvBf," ");
				char *str;
				str=strtok(NULL," ");

				fp=fopen(str,"r");
				if(fp){
					while(fgets(bf,255,(FILE *)fp)){
						write(c_socket,bf,strlen(bf));
					}
					fclose(fp); //core dumped 오류는 if(fp) 즉, fp가 존재할 때 기준인데, 만약 fp=null이면 열린적이없는데 닫으려하기때문에 뜬 것.
								   // 때문에 fclose 위치를 if 안쪽으로 옮겨주었다. 
				}
				else{
					strcpy(snBf,"파일이 존재하지 않습니다.\n");
					write(c_socket,snBf,strlen(snBf));			
					//fclose(fp); 해주면 코어덤프 뜨는데 아마도 열린적이없는데 닫으려 해서 그런 듯
				}
				continue;	// 마지막 줄에 snBf를 write 하는 구문이 있기 때문에 내부에서 이미 write 했기에 중복되는 상황을 피하기 위해서
			}
			else if(!strncasecmp(rcvBf,"exec ",strlen("exec "))){	// 명령어 실행 여부
				char *ptr=strtok(rcvBf," ");
				char *str=strtok(NULL," ");
				
			}
			else
				strcpy(snBf,"무슨말인지 모르겠네요..\n");	// 어떤 경우에도 해당하지 않는 경우
			write(c_socket,snBf,strlen(snBf));	// write는 이렇게 한번에 가능
		}
		close(c_socket);
		if(!strncasecmp(rcvBf,"kill server",strlen("kill server")))
			break;
	}
	close(s_socket);
	return 0;	
}


