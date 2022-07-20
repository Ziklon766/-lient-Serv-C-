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

typedef struct modbus{
    //������������ ���������� � ���������, ������ ���������
    short idTrans, idProt, dataSize;
    char addr;           //������ ����������
    char func;           //�������������� ���(0x03 ��� 0x10)
    char comand[MAX_STR];    //�������
};

///������
int main(int argc, char* argv[])
{
        // �������������� DLL
    WSADATA wsaData;
    WSAStartup( MAKEWORD(2, 2), &wsaData);
        // ������� �����
    SOCKET clntSock, servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        // ����������� �����
    sockaddr_in sockAddr;

    int res=1, i, size;
	hid_device *handle;

	modbus mb;
    mb.idTrans = 0x02;
    mb.idProt = 0x00;
    mb.dataSize = 0xff;
    mb.addr = 0x01;
    mb.comand[0] = '\0';

    memset (& sockAddr, 0, sizeof (sockAddr)); // ������ ���� ����������� 0
    sockAddr.sin_family = PF_INET; // ������������ IPv4-�����
    sockAddr.sin_addr.s_addr = inet_addr ("127.0.0.1"); // ������������ IP-�����
    sockAddr.sin_port = htons (8888); // ����
    bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    SOCKADDR clntAddr;

	setlocale(LC_ALL, "rus");

	if (hid_init())
		return -1;

        // ������ � ��������� �����������
    listen(servSock, 5);
        // ��������� ����������� �������
    size = sizeof(SOCKADDR);
    clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &size);
    printf("������ �����������\n");
        //������� ����������� � �����
    handle = hid_open(0x1234, 0x0001, NULL);
	if (handle) {   //� ������ ��������� ���������� - �������� �� ���� �������
		sprintf(mb.comand, "�������� ����������� � USB-hid");
		mb.func=0xff;
		send(clntSock, (char*)&mb, sizeof(modbus), NULL);
	}else{//� ������ ���������� ���������� - �������� �� ���� �������, ��������� ����� � ��������� ������
        sprintf(mb.comand, "��������� ������������ � �����. ������ ��������");
        mb.func=0x00;
        send(clntSock, (char*)&mb, sizeof(modbus), NULL);
        // ��������� �����
        closesocket(clntSock);
        closesocket(servSock);
        // ���������� ������������� DLL
        WSACleanup();
 		return 1;
	}


	do{
	    printf("\n�������� �������...\n");
        //���� ������ �� �������
        recv(clntSock, (char*)&mb, 264, NULL);
        switch(mb.func){
            case 0x10:{     //���� ��� ������
                if(mb.comand[0]==0x02){
                        //���������� ������� �����
                        hid_send_feature_report(handle,(unsigned char*)&mb.comand[0], 7);
                        //����������, ��� ������ �������
                        if(mb.comand[1]==-1) sprintf(mb.comand, "��������� ������ �������");
                        else if(mb.comand[3]==-1) sprintf(mb.comand, "��������� ������ �����");
                             else if(mb.comand[5]==-1) sprintf(mb.comand, "��������� ������ �������");
                                  else sprintf(mb.comand, "��������� �������");
                    }
                }
            break;
            case 0x03:{     //���� ��� ������
                //� �������� ������� ������ ������ ������ �� �����
                printf("������� � ����������� ������ �� �����...\n");
                system("pause");
                //���������� ������� �����
                hid_get_feature_report(handle, (unsigned char*)&mb.comand[0], 2);
                //�������, ����� ������ ������
                switch(mb.comand[1]){
                    case 1:
                        sprintf(mb.comand, "������ ������ 1");
                    break;
                    case 2:
                        sprintf(mb.comand, "������ ������ 2");
                    break;
                    case 4:
                        sprintf(mb.comand, "������ ������ 3");
                    break;
                    case 8:
                        sprintf(mb.comand, "������ ������ 4");
                    break;

                    default: sprintf(mb.comand, "������ �� ������...");
                }
            }
            break;
            //� ������ ������� ����������
            case 0x00:{
                sprintf(mb.comand, "������ ��������.");
            }
            break;
        }
        //����� � �������, ��� ���� �������
        printf("%s\n", mb.comand);
        //� ���������� ����� ������������
        send(clntSock, (char*)&mb, sizeof(modbus), NULL);
	}while(mb.func);//���� �� ������ ������� �� ���������
    // ��������� �����
    closesocket(clntSock);
    closesocket(servSock);
    // ���������� ������������� DLL
    WSACleanup();
    return 0;
}
