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

typedef struct modbus{      ///структура протокола
    //индификаторы транкзации и протокола, длинна сообщения
    short idTrans, idProt, dataSize;
    char addr;           //адресс устройства
    char func;           //функциональный код
    char comand[MAX_STR];    //команда
};

int main(int argc, char* argv[])
{
        // Инициализируем DLL
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
        // Создаем сокет
    SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        // Инициируем запрос к серверу
    sockaddr_in sockAddr;
    memset (& sockAddr, 0, sizeof (sockAddr)); // Каждый байт заполняется 0
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(8888);

	int menu, i;

	setlocale(LC_ALL, "rus");
        //Устанавливаем соединение с сокетом
	connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

        //заполняем статичную часть протокола
    modbus mb;
    mb.idTrans = 0x02;      //идентификатор  транкзакции
    mb.idProt = 0x00;       //идентификатор  протокола
    mb.dataSize = 0xff;     //размер оставшеся части
    mb.addr = 0x01;         //код устройства
    mb.comand[0] = '\0';    //команда для платы

        //Получаем данные с сервера
    recv(sock, (char*)&mb, sizeof(modbus), NULL);
    if(!mb.func){   //в случае ошибки подключения к плате
        printf("Сообщение с сервера: %s\n", mb.comand);
        // Закрываем сокет
        closesocket(sock);
        // Прекращаем использование DLL
        WSACleanup();
        return 1;
    }
    do{
        do{
            system("cls");
            printf("Сообщение с сервера: %s\n", mb.comand);
            printf("Команды для платы: \n");
            printf("1. Красный\n");
            printf("2. Синий\n");
            printf("3. Зеленый\n");
            printf("4. Выключить\n");
            printf("5. Попросить нажать кнопку\n\n");
            printf("0. Выход\n\n");
            printf("Выберите действие:");
            scanf("%d", &menu);
        }while(menu <0 && menu>5);

        switch(menu){
            case 1:{    //собираем команду для платы
                mb.func = 0x10;           //команда по протоколу - запись
                mb.comand[0] = 0x02;     //команда платы
                mb.comand[1] = 0xff;     //красный
                mb.comand[2] = 0xff;
                mb.comand[3] = 0x00;     //синий
                mb.comand[4] = 0x00;
                mb.comand[5] = 0x00;     //зеленый
                mb.comand[6] = 0x00;
            }
            break;
            case 2:{
                mb.func = 0x10;           //команда по протоколу - запись
                mb.comand[0] = 0x02;     //команда платы
                mb.comand[1] = 0x00;     //красный
                mb.comand[2] = 0x00;
                mb.comand[3] = 0xff;     //синий
                mb.comand[4] = 0xff;
                mb.comand[5] = 0x00;     //зеленый
                mb.comand[6] = 0x00;
            }
            break;
            case 3:{
                mb.func = 0x10;           //команда по протоколу - запись
                mb.comand[0] = 0x02;     //команда платы
                mb.comand[1] = 0x00;     //красный
                mb.comand[2] = 0x00;
                mb.comand[3] = 0x00;     //синий
                mb.comand[4] = 0x00;
                mb.comand[5] = 0xff;     //зеленый
                mb.comand[6] = 0xff;
            }
            break;
            case 4:{
                mb.func = 0x10;           //команда по протоколу - запись
                mb.comand[0] = 0x02;     //команда платы
                mb.comand[1] = 0x00;     //красный
                mb.comand[2] = 0x00;
                mb.comand[3] = 0x00;     //синий
                mb.comand[4] = 0x00;
                mb.comand[5] = 0x00;     //зеленый
                mb.comand[6] = 0x00;
            }
            break;
            case 5:{
                mb.func = 0x03;          //команда по протоколу - чтение
                mb.comand[0] = 0x1;     //команда платы
            }
            break;
            case 0:{
                mb.func = 0x00;          //команда для выключения сервера
            }
            break;
        }
            //Отправляем запрос на сервер
        send(sock, (char*)&mb, sizeof(modbus), NULL);
            //Получение данных, возвращаемых сервером
        recv(sock, (char*)&mb, sizeof(modbus), NULL);
    }while(menu);
         // Закрываем сокет
    closesocket(sock);
         // Прекращаем использование DLL
    WSACleanup();

    printf("Работа приложения завершена. %s\n", mb.comand);
    return 0;
}
