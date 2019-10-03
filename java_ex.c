package exer;

import java.util.Scanner;

public class GamblingGame{
	private Player [] p = new Player[2]; // 2대신 3으로 고치면 3명이 하는 게임이 된다.
	private Scanner scanner = new Scanner(System.in);
	
	public GamblingGame() {
		for(int i=0; i<p.length; i++) {
			System.out.print((i+1)+"번째 선수 이름>>");
			p[i] = new Player(scanner.nextLine());
		}
	}
	
	public void run() {
		int i=0;
		while (true) {
			if (p[i].turn()) { // 선수 i의 세 수가 모두 같은 경우
				System.out.println(p[i].getName()+"님이 이겼습니다!");
				break;
			}
			else {
				System.out.println("아쉽군요!");
				i++; // 다음 사람
				i = i%p.length; // 두 사람이 번갈아 게임함
			}
		}
	}
		
	public static void main(String [] args) {
		GamblingGame game = new GamblingGame();
		game.run();
	}
}


class Player {
	private String name;
	private Scanner scanner = new Scanner(System.in);
	public Player(String name) {
		this.name = name;
	}
	public String getName() {return name;}
	public void getEnterKey() {
		scanner.nextLine(); // <Enter> 키를 기다린다.
	} 
	public boolean turn() {
		System.out.print("[" + name + "]:<Enter를 입력하세요>");			
		getEnterKey(); 
		// 참가자가 <Enter>키 입력할 때까지 기다림. next는 내가 알기로 개행포함 안하고, nextLine()은 개행문자 기준이야.
		
		int num[] = new int [3]; // 3개의 난수를 저장하기 위한 배열
		 // 3개의 난수 생성 
		for (int i=0; i<num.length; i++) {
			num[i] = (int)(Math.random()*3 + 1); // 1~3까지의 임의의 수 발생
		}
		
		 // 3개의 난수 출력
		System.out.print("\t\t");
		for (int i=0; i<num.length; i++) {
			System.out.print(num[i]+"\t");
		}
	
		 // 3개의 난수가 같은지 비교
		boolean result = true;
		for (int i=0; i<num.length; i++) {
			if (num[i] != num[0]) { // 하나라도 다르면 false
				result = false; // 같지 않음
				break;
			}
		}
		
		return result; // result가 true 이면 승리
	}
}
