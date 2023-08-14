/*
Частина 1: Обробка сигналів
Скласти програму, яка:
- Описує глобальний дескриптор файла логу.
- Описує функцію-обробник сигналів, прототипу void signal_handler(int signo,
siginfo_t *si, void * ucontext );
- У функції-обробнику виконати запис у файлі логу з докладним розкриттям
структури siginfo_t, яка подана на вхід.
- Відкриває файл логу на запис.
- Відмічає в ньому факт власного запуску та свій pid.
- Описує структуру sigaction, у якій вказує на функцію обробник.
- Реєструє обробник для сигналу SIGHUP із збереженням попереднього обробника.
- Переходить до нескінченного циклу із засинанням на кілька секунд та відмітками
у файлі логу.
- Протестувати отриману програму, посилаючи до неї сигнали утілітою kill, та
спостерігаючи результат у файлі логу.
- Пояснити отримані результати
*/

// scp -P 6022 lab3.c dm21mx26@ee.kpi.ua:/home/students/dm21mx26/lab3
// ps -C lab3 -o "pid,ppid,pgid,sid,tty,command"

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DELAY 2

// #define LOG_FILE "/home/students/dm21mx26/lab3/log.txt"
#define LOG_FILE "log.txt"

// Дескриптор файла логу
int fd_log;

// Обробник сигналів
void signal_handler(int signo, siginfo_t *si, void *ucontext) {
  char buf[100];
  snprintf(buf, sizeof(buf), "Отримано сигнал: %d\n", signo);
  write(fd_log, buf, strlen(buf));

  // Розкриття структури siginfo_t
  snprintf(buf, sizeof(buf), "Відправник: \n");
  write(fd_log, buf, strlen(buf));

  snprintf(buf, sizeof(buf), " -> si_signo: %d\n", si->si_signo);
  write(fd_log, buf, strlen(buf));

  snprintf(buf, sizeof(buf), " -> si_code:  %d\n", si->si_code);
  write(fd_log, buf, strlen(buf));

  snprintf(buf, sizeof(buf), " -> si_pid:   %d\n", si->si_pid);
  write(fd_log, buf, strlen(buf));

  snprintf(buf, sizeof(buf), "\n");
  write(fd_log, buf, strlen(buf));
}

int main() {
  struct sigaction sa;

  // Відкриття файлу логу
  fd_log = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (fd_log == -1) {
    perror("Помилка відкриття файлу");
    exit(1);
  }

  // Запис власного запуску та pid у файл логу
  char buf[100];
  snprintf(buf, sizeof(buf), "*** Старт програми ***\n-> PID: %d\n", getpid());
  write(fd_log, buf, strlen(buf));

  // Опис структури sigaction
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = signal_handler;
  //   sa.sa_flags = SA_SIGINFO;
  sa.sa_flags = SA_SIGINFO | SA_RESTART;

  // Реєстрація обробника для сигналу SIGHUP збереженням попереднього обробника
  if (sigaction(SIGHUP, &sa, NULL) == -1) {
    perror("Помилка реєстрації обробника для сигналу SIGHUP");
    exit(1);
  }

  // Нескінченний цикл з засинанням та відмітками у файлі логу
  while (1) {
    sleep(DELAY);
    snprintf(buf, sizeof(buf), "Нескінченний цикл\n");
    write(fd_log, buf, strlen(buf));
  }

  // Закриття файлу логу
  close(fd_log);

  return 0;
}
