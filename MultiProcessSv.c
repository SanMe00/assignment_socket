// fork()를 하면 이 전체를 완전히 다 복사해서 별개의 프로세스를 생성한다. 전역변수 또한 data영역에 가져다가 이용하며 별개의 전역변수이다.
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>	// exit warning을 방지
#include <signal.h> // signal 사용
#include <sys/wait.h>

#define PORT 10000
#define BUFSIZE 10000

char snBf[BUFSIZE];
char rcvBf[BUFSIZE];
// sizeof(buffer) => 100 배열의 크기를 의미
// strlen(buffer) => 만약 hi라면 2. 내용의 길이를 의미

int numClient=0;	//현재 접속중인 클라이언트의 수 관리

void do_service(int c_socket);	//인자로 클라이언트 소켓을 받는다 ~이렇게하면 파이프통신 쓰지않는 이상 kill server 불가능!
void sig_handler(int signo);

int main(){
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int n;
	
	signal(SIGCHLD, sig_handler);
	// 첫 번째 인자: 시그널 번호
	// 두 번째 인자: 첫번째 인자의 시그널이 발생했을 때 실행되는 함수명

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
	while(1){
		len = sizeof(c_addr);
		printf("클라이언트 접속을 기다리는 중....\n");
		c_socket = accept(s_socket, (struct sockaddr *)&c_addr, &len);
		printf("/client is connected\n");
		numClient++;	// 접속 해제 시의 경우도 생각해줘야한다.
		printf("현재 접속 중인 클라이언트 수:  %d\n",numClient);
		int pid=fork();
		//클라이언트의 요청이 오면 허용(accept)해 주고, 해당 클라이언트와 통신할 수 있도록 클라이언트 소켓(c_socket)을 반환함.
		if(pid>0){	//부모프로세스
			close(c_socket);	//c_socket 같은 파일 디스크립터도 전부 복사해서 별개의 것으로 이용한다. 중복이 아님!
			continue;	//continue만 해줘도 된다.
		}
		else if(pid==0){	//자식프로세스
			close(s_socket);	//자식 프로세스 쪽에서는 s_socket을 사용할 일이 없기 때문에 닫아준다. 
								// 어차피 안닫으도 실행은 되지만 메모리만 먹는다.
								// 통신은 c_socket을 이용
			do_service(c_socket);
			// 만약 이 위치에 numClient--;를 해줘도 증가만 된다. 그 이유는 부모와 자식 프로세스는 완전히 별개이다.
			// 부모의 numClient와 자식의 numClient가 다르다!
			exit(0);
		}
		else{	//fork()함수 실패
			printf("fork() failed\n");
			exit(0);
		}
	}
}

void do_service(int c_socket){
	int n;
	int chlen; //strlen -- 처리용
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
			else if(!strncasecmp(rcvBf,"몇 살이야?\n",strlen("몇 살이야?"))||!strncasecmp(rcvBf,"몇살이야?\n",strlen("몇살이야?"))){
				strcpy(snBf,"저는 1살이에요!\n");
			}
			else if(!strncasecmp(rcvBf,"strlen ",strlen("strlen "))){
				chlen=strlen(rcvBf)-7;
				sprintf(snBf,"문자열의 길이는 %d입니다.\n",chlen);	
				//sprintf(chlen_str,"문자열의 길이는 %d입니다.\n",strlen(rcvBf)-7); 교수님 방식, 개행제외하려면 -1 추가라고하신다.
				//strcpy(snBf,chlen_str); //불필요, sprintf에서 한 번에 처리 가능. 반면교사로 남겨놓음
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
				char *ptr=strtok(rcvBf," ");
				char *str[10];
				int cnt=0;
				while(ptr!=NULL){
					str[cnt]=ptr;
					cnt++;
					ptr=strtok(NULL," ");
				}
				if(cnt < 2){
					strcpy(snBf,"파일명을 입력해주세요");
				}
				else{
					FILE *fp=fopen(str[1],"r");
					if(fp){
						char bf[BUFSIZE];	//파일내용을 저장할 변수
						memset(snBf,0,BUFSIZE);	//snBf초기화
						while(fgets(bf,BUFSIZE,(FILE *)fp)){
							strcat(snBf,bf);	//여러줄의 내용을 하나의 buffer에 저장하기 위해 strcat()이용
							//write(c_socket,bf,strlen(bf));
						}
						fclose(fp); //core dumped 오류는 if(fp) 즉, fp가 존재할 때 기준인데, 만약 fp=null이면 열린적이없는데 닫으려하기때문에 뜬 것.
									   // 때문에 fclose 위치를 if 안쪽으로 옮겨주었다. 
					}
					else{
						strcpy(snBf,"파일이 존재하지 않습니다.\n");
						//write(c_socket,snBf,strlen(snBf));			
						//fclose(fp); 해주면 코어덤프 뜨는데 아마도 열린적이없는데 닫으려 해서 그런 듯
						}
					//continue;	// 마지막 줄에 snBf를 write 하는 구문이 있기 때문에 내부에서 이미 write 했기에 중복되는 상황을 피하기 위해서
					}
			}
			else if(!strncasecmp(rcvBf,"exec ",strlen("exec "))){	// 명령어 실행 여부
				char *ptr=strtok(rcvBf," ");
				char *str=strtok(NULL,"\0");
				memset(snBf,0,BUFSIZE);
				printf("Command Call: %s\n",str);
				if(!system(str)){	//한 번 실행을 이미 하는 것이고, 그 값을 조건판별한다. 그러므로 시스템함수 한 번 이용 됨.
					printf("Command is Executed\n");
					sprintf(snBf,"[%s] command is executed!",str);
				}
				else{
					printf("Command is Failed\n");
					sprintf(snBf,"[%s] command is failed!",str);
				}
			}
			else
				strcpy(snBf,"무슨말인지 모르겠네요..\n");	// 어떤 경우에도 해당하지 않는 경우
			write(c_socket,snBf,strlen(snBf));	// write는 이렇게 한번에 가능
		}
		close(c_socket);
}

void sig_handler(int signo){
	int pid;
	int status;
	pid = wait(&status);	//자식 프로세스가 종료할 때까지 기다리는 함수.
	// 자식 프로세스가 종료되면 종료된 자식 프로세스의 pid반환, status에는 자식 프로세스의 종료 상태를 저장함.
	// status=0 이면 종료된 것이다.
	printf("pid[%d] is terminated.status = %d\n",pid,status);
	numClient--;
	printf("현재 접속 중인 클라이언트 수: %d\n",numClient);
}

