/*
Частина 1: Побудова мережевого сервера
Скласти програму, яка:
1 Виконує демонізацію, подальший функціонал стосується демона.
2 Описує глобальний дескриптор файла логу.
3 Описує структуру struct sockaddr_in з параметрами: формат
сокетів - PF_INET, адреса - будь-яка, порт - 3200 + номер варіанта №4.
4 Формує сокет типу SOCK_STREAM формату PF_INET.
5 Налаштовує сокет на очікування запитів за допомогою bind.
6 Запускає нескінченний цикл обробки запитів від клієнтів.
7 На кожний запит виконує fork для породження процесу обробки.
8 Батьківський процес закриває сокет.
9 Процес обробки в нескінченному циклі отримує від клієнта строки
за допомогою recv, додає до них свій префікс у вигляді поточного часу та
власного pid, та повертає клієнту за допомогою send. 10	Процес обробки завершує
обробку даних від клієнта отримавши строку "close". 11	Кожна дія сервера
супроводжується відміткою у логу.
*/

// Варіант 4 (10)
// scp -P 6022 lab4_s.c dm21mx26@ee.kpi.ua:/home/students/dm21mx26/lab4
// ps -C lab4_s.c -o "pid,ppid,pgid,sid,tty,command"
// gcc -o lab4_s lab4_s.c

// ls -l
// killall lab4_s

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define PORT 3204
// #define LOG_FILE "/home/students/dm21mx26/lab4/log.txt"
#define LOG_FILE "log.txt"

// Дескриптор файла логу
int fd_log;

void process_client(int client_socket) {
  char buffer[1024];

  while (1) {
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
      perror("Помилка отримання даних від клієнта");
      break;
    } else if (bytes_received == 0) {
      printf("Клієнт закрив з'єднання\n");
      break;
    }

    if (strcmp(buffer, "close\n") == 0) {
      printf("Клієнт відправив команду закриття з'єднання\n");
      break;
    }

    time_t now;
    time(&now);
    char *current_time = ctime(&now);

    char response[1024];
    snprintf(response, sizeof(response),
             "%s Потік PID: %d обробив повідомлення: - %s", current_time,
             getpid(), buffer);

    if (send(client_socket, response, strlen(response), 0) < 0) {
      perror("Помилка відправки даних клієнту");
      break;
    }

    printf("--> Буфер (тест): %s", buffer);

    char str[1024];
    // Запис у файл логу
    sprintf(str, "Час: %s Потік обробки PID: %d повернув повідомлення: - %s \n",
            current_time, getpid(), buffer);
    write(fd_log, str, strlen(str));
    // write(fd_log, str, sizeof(str) - 1);
  }

  close(client_socket);
}

int main() {
  int server_socket, client_socket;
  struct sockaddr_in server_address, client_address;
  socklen_t client_address_len;

  printf("*** Сервер ***\n");

  // Відкриття файла логу
  // Відкриття файлу логу
  fd_log = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (fd_log == -1) {
    perror("Помилка відкриття файлу");
    exit(1);
  }

  // Створення сокета
  if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Помилка створення сокета");
    exit(1);
  }

  // Налаштування адреси сервера
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT);
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);

  // Прив'язка сокета до адреси сервера
  if (bind(server_socket, (struct sockaddr *)&server_address,
           sizeof(server_address)) < 0) {
    perror("Помилка прив'язки сокета до адреси сервера");
    exit(1);
  }

  // Очікування запитів на з'єднання
  if (listen(server_socket, 5) < 0) {
    perror("Помилка очікування запитів на з'єднання");
    exit(1);
  }

  printf("Сервер запущений та очікує з'єднання...\n");

  while (1) {
    // Прийняття з'єднання
    client_address_len = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr *)&client_address,
                           &client_address_len);
    if (client_socket < 0) {
      perror("Помилка прийняття з'єднання");
      exit(1);
    }

    printf("-> Отримано нове з'єднання\n");
    printf("Сервер PID: %d\n", getpid());

    // Створення нового процесу для обробки клієнта
    pid_t pid = fork();
    if (pid < 0) {
      perror("Помилка створення процесу для обробки клієнта");
      exit(1);
    } else if (pid == 0) {
      // Дочірній процес - обробка клієнта
      close(server_socket);
      process_client(client_socket);

      // Закриття файла логу
      close(fd_log);

      exit(0);
    } else {
      // Батьківський процес
      close(client_socket);
    }
  }

  close(server_socket);

  return 0;
}
