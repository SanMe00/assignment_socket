#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

void *do_chat(void *); //채팅 메세지를 보내는 함수
int pushClient(char *,int); //새로운 클라이언트가 접속했을 때 클라이언트 정보 추가
int popClient(int); //클라이언트가 종료했을 때 클라이언트 정보 삭제

pthread_t thread;
pthread_mutex_t mutex;

#define MAX_CLIENT 10
#define CHATDATA 1024
#define INVALID_SOCK -1
#define PORT 9000
#define NICK 20

typedef struct list_c{
	int socket;
	char nickname[NICK];
};

struct list_c list_c[MAX_CLIENT];
char    escape[ ] = "exit";
char    greeting[ ] = "Welcome to chatting room\n";
char    CODE200[ ] = "Sorry No More Connection\n";

int main(int argc, char *argv[ ])
{
    int c_socket, s_socket;
    struct sockaddr_in s_addr, c_addr;
    int    len;
    int    i, j, n;
    int    res;
	char nickname[NICK]; //임시로 닉네임 담아두는 변수
    if(pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Can not create mutex\n");
        return -1;
    }
    s_socket = socket(PF_INET, SOCK_STREAM, 0);
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(PORT);
    if(bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
        printf("Can not Bind\n");
        return -1;
    }
    if(listen(s_socket, MAX_CLIENT) == -1) {
        printf("listen Fail\n");
        return -1;
    }
    for(i = 0; i < MAX_CLIENT; i++)
        list_c[i].socket = INVALID_SOCK;
    while(1) {
        len = sizeof(c_addr);
        c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);
		 read(c_socket,nickname,sizeof(nickname)); //nickname을 받아와서 설정 또한 해줘야 함
        res = pushClient(nickname,c_socket);
        if(res < 0) { //MAX_CLIENT만큼 이미 클라이언트가 접속해 있다면,
            write(c_socket, CODE200, strlen(CODE200));
            close(c_socket);
        } else {
            write(c_socket, greeting, strlen(greeting));
            //pthread_create with do_chat function. ------개인 과제 1
            pthread_create(&thread,NULL,do_chat,(void *)&c_socket);
        }
    }
}
void *do_chat(void *arg)
{
    int c_socket = *((int *)arg);
    char chatData[CHATDATA];
	char wisper[CHATDATA];
	char wis_result[CHATDATA];
    int i, n;
    while(1) {
        memset(chatData, 0, sizeof(chatData));
        if((n = read(c_socket, chatData, sizeof(chatData))) > 0) {
        		// 귓속말인 경우 ('/r')
			write(1,"receive",strlen("receive"));
			char *sender;
			char *wis;
			strcpy(wisper,chatData);
			sender=strtok(wisper," "); //[nickname]부분 제거
			
			wis=strtok(NULL," "); // '/r'
        	if(!strncasecmp(wis,"/r",strlen("/r"))){
				write(1,"wisper",strlen("wisper"));
				char *nick;
				char *chat;
        		nick=strtok(NULL," "); // 대상 유저
        		chat=strtok(NULL,"\0"); // 귓속말 채팅부분
        		for(i=0;i<=MAX_CLIENT;i++){
        			if(!strncasecmp(list_c[i].nickname,nick,strlen(nick))){//지정한 대상에게만 글을 보이게
        				write(1,"receive",strlen("test"));
						sprintf(wis_result,"[Wisper From.%s]",sender);
						strcat(wis_result,chat);
						write(list_c[i].socket,wis_result,strlen(wis_result));
        				}
        			}
        		}
        	
        	else{
			//write chatData to all clients
        	for(i=0;i<=MAX_CLIENT;i++){
        		if(list_c[i].socket == INVALID_SOCK)
        			continue;
        		else if(list_c[i].socket != c_socket){ //이렇게 해야 자기 제외하고 보낸다
        			write(list_c[i].socket,chatData,strlen(chatData));
        				// list_c[i].list라고 해야 모든사용자에게 보내는 것이다.
        			} //for문 종료
        			}
			}
        	

            if(strstr(chatData, escape) != NULL) {
                popClient(c_socket);
                break;
            }
        }
    }
}
int pushClient(char * nickname, int c_socket) {	//------개인과제 2
	int i;
	for(i=0;i<=MAX_CLIENT;i++){
		if(list_c[i].socket==INVALID_SOCK){
			list_c[i].socket=c_socket;
			strcpy(list_c[i].nickname,nickname);
			return i;
		}
		else if(list_c[i].socket != INVALID_SOCK){
			continue;
		}
	}
	return INVALID_SOCK;
    //ADD c_socket to list_c array.
    //
    ///////////////////////////////
    //return -1, if list_c is full.
    //return the index of list_c which c_socket is added.
}
int popClient(int c_socket)
{
	int i;
	for(i=0;i<=MAX_CLIENT;i++){
		if(list_c[i].socket==c_socket){
			list_c[i].socket=INVALID_SOCK;
			close(c_socket);
		}
		else if(list_c[i].socket != c_socket){
			continue;
		}
	}
   	return i;
    //REMOVE c_socket from list_c array.
    //
    ///////////////////////////////////
}
