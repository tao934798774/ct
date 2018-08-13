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
		printf("��ϲ�㣬��Ӯ��!\n");
	}
	else if (ret == '&')
	{
		printf("���ź���������!\n");
	}
	else if (ret == 'q')
	{
		printf("ƽ��\n");
	}

}
void test()
{
	int input = 0;
	srand((unsigned int)time(NULL));
	do
	{
		menu();
		printf("��ѡ��>");
		scanf("%d", &input);
		switch (input)
		{
		case 1:
			game();
			break;
		case 0:
			break;
		default:
			printf("ѡ���������������");
		}
	} while (input);
} 

int main()
{
	test();
	return 0;
}