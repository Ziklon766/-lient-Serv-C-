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
    //индификаторы транкзации и протокола, длинна сообщения
    short idTrans, idProt, dataSize;
    char addr;           //адресс устройства
    char func;           //функциональный код(0x03 или 0x10)
    char comand[MAX_STR];    //команда
};

///СЕРВЕР
int main(int argc, char* argv[])
{
        // Инициализируем DLL
    WSADATA wsaData;
    WSAStartup( MAKEWORD(2, 2), &wsaData);
        // Создаем сокет
    SOCKET clntSock, servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        // Привязываем сокет
    sockaddr_in sockAddr;

    int res=1, i, size;
	hid_device *handle;

	modbus mb;
    mb.idTrans = 0x02;
    mb.idProt = 0x00;
    mb.dataSize = 0xff;
    mb.addr = 0x01;
    mb.comand[0] = '\0';

    memset (& sockAddr, 0, sizeof (sockAddr)); // Каждый байт заполняется 0
    sockAddr.sin_family = PF_INET; // Использовать IPv4-адрес
    sockAddr.sin_addr.s_addr = inet_addr ("127.0.0.1"); // Определенный IP-адрес
    sockAddr.sin_port = htons (8888); // Порт
    bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    SOCKADDR clntAddr;

	setlocale(LC_ALL, "rus");

	if (hid_init())
		return -1;

        // Входим в состояние мониторинга
    listen(servSock, 5);
        // Получение клиентского запроса
    size = sizeof(SOCKADDR);
    clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &size);
    printf("Клиент подключился\n");
        //Попытка подключения к плате
    handle = hid_open(0x1234, 0x0001, NULL);
	if (handle) {   //в случае успешного подклчения - сообщаем об этом клиенту
		sprintf(mb.comand, "Успешное подключение к USB-hid");
		mb.func=0xff;
		send(clntSock, (char*)&mb, sizeof(modbus), NULL);
	}else{//в случае неудачного подклчения - сообщаем об этом клиенту, закрываем сокет и выключаем сервер
        sprintf(mb.comand, "Неудалось подключиться к плате. Сервен выключен");
        mb.func=0x00;
        send(clntSock, (char*)&mb, sizeof(modbus), NULL);
        // Закрываем сокет
        closesocket(clntSock);
        closesocket(servSock);
        // Прекращаем использование DLL
        WSACleanup();
 		return 1;
	}


	do{
	    printf("\nОжидание запроса...\n");
        //ждем запрос от клиента
        recv(clntSock, (char*)&mb, 264, NULL);
        switch(mb.func){
            case 0x10:{     //если это запись
                if(mb.comand[0]==0x02){
                        //отправляем уоманду плате
                        hid_send_feature_report(handle,(unsigned char*)&mb.comand[0], 7);
                        //определяем, что делала команда
                        if(mb.comand[1]==-1) sprintf(mb.comand, "Светодиод теперь красный");
                        else if(mb.comand[3]==-1) sprintf(mb.comand, "Светодиод теперь синий");
                             else if(mb.comand[5]==-1) sprintf(mb.comand, "Светодиод теперь зеленый");
                                  else sprintf(mb.comand, "Светодиод погашен");
                    }
                }
            break;
            case 0x03:{     //если это чтение
                //в конасоли сервера просим зажать клавшу на плате
                printf("Нажмите и удерживайте кнопку на плате...\n");
                system("pause");
                //отправляем команлу плате
                hid_get_feature_report(handle, (unsigned char*)&mb.comand[0], 2);
                //смотрим, какая кнопка нажата
                switch(mb.comand[1]){
                    case 1:
                        sprintf(mb.comand, "Нажата кнопка 1");
                    break;
                    case 2:
                        sprintf(mb.comand, "Нажата кнопка 2");
                    break;
                    case 4:
                        sprintf(mb.comand, "Нажата кнопка 3");
                    break;
                    case 8:
                        sprintf(mb.comand, "Нажата кнопка 4");
                    break;

                    default: sprintf(mb.comand, "Кнопка не нажата...");
                }
            }
            break;
            //в случае команды выключения
            case 0x00:{
                sprintf(mb.comand, "Сервер выключен.");
            }
            break;
        }
        //пишем в консоль, что было сделано
        printf("%s\n", mb.comand);
        //и отправляем ответ пользователю
        send(clntSock, (char*)&mb, sizeof(modbus), NULL);
	}while(mb.func);//пока не пришла команда на выклчение
    // Закрываем сокет
    closesocket(clntSock);
    closesocket(servSock);
    // Прекращаем использование DLL
    WSACleanup();
    return 0;
}
