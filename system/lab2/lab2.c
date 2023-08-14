/*
Частина 1: Запуск, завершення та параметри процесу
Скласти програму, яка:
- Отримує та друкує інформацію про параметри свого процесу за допомогою
системних викликів getpid(), getgid(), getsid() тощо.
- Виконує розгалудження процесу за допомогою системного виклику fork().
- Для процесу-батька та процесу-нащадка окремо роздруковує їхні ідентифікатори у
циклі.
- Виконати очікування процесом-батьком завершення нащадка.
- Повідомляє про завершення процесів – батька та нащадка.
- Пояснити отримані результати.

У Unix системах під процесом розуміється об'єкт операційної системи, що виконує
код програми і має власний контекст: стек, набір сторінок пам'яті, таблицю
відкритих файлів та унікальний номер (PID - скорочення від process ID),
привласнений йому системою. Процес – це екземпляр програми, що виконується.

A call to wait() blocks the calling process until one
of its child processes exits or a signal is received.
After child process terminates, parent continues its
execution after wait system call instruction.

We know if more than one child processes are terminated,
then wait() reaps any arbitrarily child process but if we
want to reap any specific child process, we use waitpid() function.
*/

// scp -P 6022 lab2.c dm21mx26@ee.kpi.ua:/home/students/dm21mx26/lab2

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define TIMES 5
#define DELAY 2

int main(int argc, char const *argv[]) {
  // Інформація про процес
  pid_t pid = getpid();
  pid_t ppid = getppid();
  pid_t gid = getgid();
  pid_t sid = getsid(0);

  printf("Інформація про процес:\n");
  printf("ID поточного процесу PID: \t%d\n", pid);
  printf("ID батьківського процесу PPID: \t%d\n", ppid);
  printf("ID групи GID: \t%d\n", gid);
  printf("ID сесії SID: \t%d\n", sid);
  printf("***\n");

  // Розгалуження процесу. Створює новий процес, що виконує копію програми.
  // Батько та нащадок виконуються одночасно після fork()
  pid = fork();

  if (pid == -1) {
    perror("Помилка виклику fork");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {

    // Код для процесу-нащадка
    printf(" -> Створено процес-нащадок\n");
    for (size_t i = 0; i < TIMES; i++) {
      printf("Процес-нащадок, ID: getpid() %d \t getppid() %d \t pid %d\n",
             getpid(), getppid(), pid);
      sleep(DELAY);
    }
    printf(" -> Нащадок вихід з циклу\n");
  } else {

    // Код для процесу-батька
    for (size_t i = 0; i < TIMES; i++) {
      printf("Процес-батько,  ID: getpid() %d \t getppid() %d \t pid %d\n",
             getpid(), getppid(), pid);
      sleep(DELAY);
    }
    printf(" -> Батько вихід з циклу\n");

    // Очікуємо завершення нащадка. Функція wait припиняє виконання поточного
    // процесу доти, доки дочірній процес не завершиться

    // wait(NULL);
    // printf("Процес-нащадок завершився\n");
    // printf("Процес-батько завершився\n");

    // Очікування процесом-батьком завершення процесу нащадка
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
      printf("Процес-нащадок успішно завершився з кодом виходу: %d\n",
             WEXITSTATUS(status));
    } else {
      printf("Процес-нащадок не завершився успішно!\n");
    }

    if (WIFSIGNALED(status)) {
      printf("Процес-нащадок завершився з сигналом: %d\n", WTERMSIG(status));
    }
    exit(EXIT_SUCCESS);
  }

  return 0;
}
