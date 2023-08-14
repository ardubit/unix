#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 3210

FILE* log_file; // Глобальний дескриптор файла логу

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
    char* current_time = ctime(&now);

    char response[1024];
    snprintf(response, sizeof(response), "[%s] PID: %d - %s", current_time, getpid(), buffer);

    if (send(client_socket, response, strlen(response), 0) < 0) {
      perror("Помилка відправки даних клієнту");
      break;
    }

    // Запис у файл логу
    fprintf(log_file, "[%s] PID: %d - %s", current_time, getpid(), buffer);
    fflush(log_file);
  }

  close(client_socket);
}

int main() {
  int server_socket, client_socket;
  struct sockaddr_in server_address, client_address;
  socklen_t client_address_len;

  log_file = fopen("log.txt", "a"); // Відкриття файла логу для додавання даних

  if (log_file == NULL) {
    perror("Помилка відкриття файла логу");
    return 1;
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
  if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
    perror("Помилка прив'язки сокета до адреси сервера");
    exit(1);
  }

  // Очікування запитів на з'єднання
  if (listen(server_socket, 5) < 0) {
    perror("Помилка очікування запитів на з'єднання");
    exit(1);
  }

  printf("Сервер запущений і очікує з'єднання...\n");

  while (1) {
    // Прийняття з'єднання
    client_address_len = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
    if (client_socket < 0) {
      perror("Помилка прийняття з'єднання");
      exit(1);
    }

    printf(" -> Отримано нове з'єднання\n PID процесу обробки: %d\n", getpid());

    // Створення нового процесу для обробки клієнта
    pid_t pid = fork();
    if (pid < 0) {
      perror("Помилка створення процесу для обробки клієнта");
      exit(1);
    } else if (pid == 0) {
      // Дочірній процес - обробка клієнта
      close(server_socket);
      process_client(client_socket);
      fclose(log_file); // Закриття файла логу
      exit(0);
    } else {
      // Батьківський процес
      close(client_socket);
    }
  }

  close(server_socket);

  return 0;
}
