#define _CRT_SECURE_NO_WARNINGS 1
#include<stdio.h>
#include "game.h"
#include<windows.h>

void menu()
{
	printf("***********************************\n");
	printf("****   1.play       0.exit     ****\n");
	printf("***********************************\n");
}

void game()
{
	char ret = 0;
	char arr[ROW][COL] = {0};
	InitBoard(arr, ROW, COL);
	DisplayBoard(arr, ROW, COL);
	do
	{	
		PlayerMove(arr, ROW, COL);
		DisplayBoard(arr, ROW, COL);
		ret = Is_Win(arr, ROW, COL);
		if (ret != ' ')
			break;
		Sleep(100);
		ComputerMove(arr, ROW, COL);
		DisplayBoard(arr, ROW, COL);
		ret = Is_Win(arr, ROW, COL);
	} while (ret == ' ');
	if (ret == 'X')
	{
		printf("恭喜你，你赢了!\n");
	}
	else if (ret == '&')
	{
		printf("很遗憾，你输了!\n");
	}
	else if (ret == 'q')
	{
		printf("平局\n");
	}

}
void test()
{
	int input = 0;
	srand((unsigned int)time(NULL));
	do
	{
		menu();
		printf("请选择：>");
		scanf("%d", &input);
		switch (input)
		{
		case 1:
			game();
			break;
		case 0:
			break;
		default:
			printf("选择错误，请重新输入");
		}
	} while (input);
} 

int main()
{
	test();
	return 0;
}