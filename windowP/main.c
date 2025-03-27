#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include "display.h"



int game() {
	system("cls");
	puts("게임 시작!");
	puts("");


	int pos = 5;
	char move;

	while (1) {
		for (int i = 0; i < 11; i++)
		{
			if (i == pos) {
				printf("0");
			}
			else
			{
				printf("_");
			}
		}

		printf("\n");

		move = _getch();

		if (move == 'a' && pos > 0)
		{
			pos--;
		}
		else if (move == 'd' && pos < 10) {
			pos++;
		}

		system("cls");
	}
}


int main() {
	menu();

	char c = getch();

	if (c == '1')
	{
		game();
	}
	else if(c == '2')
	{
		puts("게임이 종료됩니다.");
	}
	else
	{
		puts("잘못된 입력입니다.");
	}
	

	//git test

	return 0;
}