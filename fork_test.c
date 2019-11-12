#include<stdio.h>

int main(){
	int a=10;
	int b=100;
	
	a+=10;
	sleep(10);
	int pid=fork();
	// fork() 함수는 현재 구동 중인 프로소세의 복제본을 생성
	// 현재 프로세스 = 부모 프로세스
	// fork()에 의해 생성된 프로세스 = 자식 프로세스
	// 리턴 값(pid) = 0 then 자식프로세스
	// 리턴 값(oid) = 자식 프로세스의 pid값 then 부모 프로세스
	// 만약 fork() 함수 실패 시, -1값을 리턴 -> 대체로 실패x 메모리 부족하면 실패
	
	if(pid>0){	//부모 프로세스, 결과: a=30, b=100
		printf("부모 프로세스\n");
		a+=10;
		printf("[Parent] a=%d, b=%d\n",a,b);
		sleep(20);	// 20초 동안 정지하지 않고 대기
	}
	else if(pid==0){	//자식 프로세스, 결과: a=20, b=1000 
		printf("자식 프로세스\n");
		b*=10;
		printf("[Child] a=%d, b=%d\n",a,b);
		sleep(20);
	}
	// 부모 프로세스와 자식 프로세스는 완전히 독립된 공간을 지니고 있기에 서로에게 영향x
	else{	// fork()함수 실패
		printf("fork() 함수가 실패하였습니다.\n");
	}
	return 0;
}
