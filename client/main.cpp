#define WIN32
#ifdef WIN32
#include <windows.h>
#else
#include <stdlib.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"
#include <stdint.h>
#include <locale.h>
#include <WinSock2.h>

#define MAX_STR 255

typedef struct modbus{      ///��������� ���������
    //������������ ���������� � ���������, ������ ���������
    short idTrans, idProt, dataSize;
    char addr;           //������ ����������
    char func;           //�������������� ���
    char comand[MAX_STR];    //�������
};

int main(int argc, char* argv[])
{
        // �������������� DLL
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
        // ������� �����
    SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        // ���������� ������ � �������
    sockaddr_in sockAddr;
    memset (& sockAddr, 0, sizeof (sockAddr)); // ������ ���� ����������� 0
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(8888);

	int menu, i;

	setlocale(LC_ALL, "rus");
        //������������� ���������� � �������
	connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

        //��������� ��������� ����� ���������
    modbus mb;
    mb.idTrans = 0x02;      //�������������  �����������
    mb.idProt = 0x00;       //�������������  ���������
    mb.dataSize = 0xff;     //������ ��������� �����
    mb.addr = 0x01;         //��� ����������
    mb.comand[0] = '\0';    //������� ��� �����

        //�������� ������ � �������
    recv(sock, (char*)&mb, sizeof(modbus), NULL);
    if(!mb.func){   //� ������ ������ ����������� � �����
        printf("��������� � �������: %s\n", mb.comand);
        // ��������� �����
        closesocket(sock);
        // ���������� ������������� DLL
        WSACleanup();
        return 1;
    }
    do{
        do{
            system("cls");
            printf("��������� � �������: %s\n", mb.comand);
            printf("������� ��� �����: \n");
            printf("1. �������\n");
            printf("2. �����\n");
            printf("3. �������\n");
            printf("4. ���������\n");
            printf("5. ��������� ������ ������\n\n");
            printf("0. �����\n\n");
            printf("�������� ��������:");
            scanf("%d", &menu);
        }while(menu <0 && menu>5);

        switch(menu){
            case 1:{    //�������� ������� ��� �����
                mb.func = 0x10;           //������� �� ��������� - ������
                mb.comand[0] = 0x02;     //������� �����
                mb.comand[1] = 0xff;     //�������
                mb.comand[2] = 0xff;
                mb.comand[3] = 0x00;     //�����
                mb.comand[4] = 0x00;
                mb.comand[5] = 0x00;     //�������
                mb.comand[6] = 0x00;
            }
            break;
            case 2:{
                mb.func = 0x10;           //������� �� ��������� - ������
                mb.comand[0] = 0x02;     //������� �����
                mb.comand[1] = 0x00;     //�������
                mb.comand[2] = 0x00;
                mb.comand[3] = 0xff;     //�����
                mb.comand[4] = 0xff;
                mb.comand[5] = 0x00;     //�������
                mb.comand[6] = 0x00;
            }
            break;
            case 3:{
                mb.func = 0x10;           //������� �� ��������� - ������
                mb.comand[0] = 0x02;     //������� �����
                mb.comand[1] = 0x00;     //�������
                mb.comand[2] = 0x00;
                mb.comand[3] = 0x00;     //�����
                mb.comand[4] = 0x00;
                mb.comand[5] = 0xff;     //�������
                mb.comand[6] = 0xff;
            }
            break;
            case 4:{
                mb.func = 0x10;           //������� �� ��������� - ������
                mb.comand[0] = 0x02;     //������� �����
                mb.comand[1] = 0x00;     //�������
                mb.comand[2] = 0x00;
                mb.comand[3] = 0x00;     //�����
                mb.comand[4] = 0x00;
                mb.comand[5] = 0x00;     //�������
                mb.comand[6] = 0x00;
            }
            break;
            case 5:{
                mb.func = 0x03;          //������� �� ��������� - ������
                mb.comand[0] = 0x1;     //������� �����
            }
            break;
            case 0:{
                mb.func = 0x00;          //������� ��� ���������� �������
            }
            break;
        }
            //���������� ������ �� ������
        send(sock, (char*)&mb, sizeof(modbus), NULL);
            //��������� ������, ������������ ��������
        recv(sock, (char*)&mb, sizeof(modbus), NULL);
    }while(menu);
         // ��������� �����
    closesocket(sock);
         // ���������� ������������� DLL
    WSACleanup();

    printf("������ ���������� ���������. %s\n", mb.comand);
    return 0;
}
