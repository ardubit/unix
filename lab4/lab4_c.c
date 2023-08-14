/*
Частина 2: Побудова мережевого клієнта
Скласти програму, яка:
1 Описує структуру struct sockaddr_in з параметрами: формат сокетів -
PF_INET, адреса - обчислена за викликом htonl(INADDR_LOOPBACK), порт - 3200 +
номер варіанта №4.
2 Формує сокет типу SOCK_STREAM формату PF_INET.
3 Налаштовує сокет на підключення до сервера за допомогою connect.
4 В нескінченному циклі запитує рядки від оператора, передає їх на сервер та
друкує отримані відповіді.
5 Робота закінчується після вводу оператором рядка "close" та отримання на нього
відповіді від сервера.
6 Пояснити отримані результати.
*/

// Варіант 4 (10)
// scp -P 6022 lab4_c.c dm21mx26@ee.kpi.ua:/home/students/dm21mx26/lab4
// ps -C lab4_с.c -o "pid,ppid,pgid,sid,tty,command"
// gcc -o lab4_c lab4_c.c

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 3204

int main() {
  int client_socket;
  struct sockaddr_in server_address;
  char buffer[1024];

  printf("*** Клієнт ***\n");

  // Створення сокета
  if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Помилка створення сокета");
    exit(1);
  }

  // Налаштування адреси сервера
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT);
  server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  // Підключення до сервера
  if (connect(client_socket, (struct sockaddr *)&server_address,
              sizeof(server_address)) < 0) {
    perror("Помилка підключення до сервера");
    exit(1);
  }

  printf("-> Підключено до сервера\n");
  printf("Введіть повідомлення:\n");

  while (1) {
    memset(buffer, 0, sizeof(buffer));
    fgets(buffer, sizeof(buffer), stdin);

    // Відправка повідомлення на сервер
    if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
      perror("Помилка відправки даних на сервер");
      exit(1);
    }

    // Отримання відповіді від сервера
    memset(buffer, 0, sizeof(buffer));
    if (recv(client_socket, buffer, sizeof(buffer), 0) < 0) {
      perror("Помилка отримання даних від сервера");
      exit(1);
    }

    printf("Відповідь сервера: %s", buffer);

    if (strcmp(buffer, "Клієнт закрив з'єднання\n") == 0) {
      break;
    }
  }

  close(client_socket);

  return 0;
}
