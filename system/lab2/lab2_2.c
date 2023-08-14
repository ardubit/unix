// scp -P 6022 lab2_2.c dm21mx26@ee.kpi.ua:/home/students/dm21mx26/lab2
// kill -9 13065, where kill -9 13065 is PID

// ps -e
// ps -C lab2_2 -o "pid,ppid,pgid,sid,tty,command"
// ps -C mydaemon -o "pid,ppid,pgid,sid,tty,command"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define LOG_FILE "/home/students/dm21mx26/lab2/log.txt"
// #define LOG_FILE "/Volumes/DATA-SSD/fel-labs/system/lab2/log.txt"
#define DELAY 2
char str[256];

// void write_log(const char *msg) {
//   size_t msg_len = strlen(msg);
//   int fd_open = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);

//   if (fd_open > 0) {
//     int bytes = write(fd_open, msg, msg_len);
//     if (bytes == -1) {
//       perror("Помилка запису даних у файл");
//       exit(EXIT_FAILURE);
//     } else {
//       close(fd_open);
//     }

//   } else {
//     perror("Помилка відкриття файлу");
//     exit(EXIT_FAILURE);
//   }
// }

void write_log(const char *msg) {
  FILE *file = fopen(LOG_FILE, "a");
  if (file != NULL) {
    fprintf(file, "%s", msg);
    fclose(file);
  }
}

int main(int argc, char *argv[]) {
  write_log("*** Cтарт програми ***\n");

  // Після форку утворюються два процеси: нащадок отримує значення pid = 0,
  // батько pid > 0
  pid_t pid = fork();

  if (pid == -1) {
    perror("Помилка виклику fork");
    exit(EXIT_FAILURE);
  }

  if (pid > 0) {
    // Код для батька (процесу)
    printf(" -> Процес батька відповідає: Створено процес-нащадок\n");
    sprintf(str, " -> Створено процес-нащадок\n");
    write_log(str);

    // Завершення батька
    printf(" -> Завершення батьківського процесу\n");
    exit(0);
  }

  if (pid == 0) {
    // Код для нащадка (процесу)
    printf(" -> Процес нащадок відповідає: pid %d \t ppid %d \t sid %d\n",
           getpid(), getppid(), getsid(0));

    // Демонізація
    pid_t sid = setsid();
    printf(" -> Виконання setsid()\n");
    printf(" -> Процес нащадок відповідає: pid %d \t ppid %d \t sid %d\n",
           getpid(), getppid(), getsid(0));

    // Нащадок стає лідером процесу
    if (sid < 0) {
      perror("Помилка setsid()");
      exit(EXIT_FAILURE);
    } else {
      printf(" -> Нащадок виконав setsid(), sid сесії стає: %d\n", getsid(0));
      write_log("Нащадок виконав setsid()\n");
    }

    // Зміна поточного каталогу на "/"
    if (chdir("/") < 0) {
      perror("Помилка chdir");
      exit(EXIT_FAILURE);
    } else {
      write_log("Нащадок змінив директорію на '/'\n");
      printf(" -> Нащадок змінив директорію на '/'\n");
    }

    // Закриття нащадком усіх відкритих батьком дескрипторів
    printf(" -> Закриття відкритих батьком дескрипторів\n");
    printf("*** Наступний вивід не буде транслюватись у термінал! *** \n");

    // int maxfd = sysconf(_SC_OPEN_MAX);
    // for (int fd = 0; fd < maxfd; fd++) {
    //   close(fd);
    // }

    // Закриття стандартних потоків вводу-виводу
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // umask(0);

    // Все. Наступний вивід вже не буде транслюватись у термінал
    printf(" -> Демон, демон, демон\n");
    write_log("Нащадок закрив відкриті батьком дескриптори\n");

    // Перенаправлення. Відкриваємо "/dev/null" на запис для трьох стандартних
    // потоків
    int devnull = open("/dev/null", O_RDWR);
    if (devnull < 0) {
      perror("Помилка open /dev/null");
      exit(EXIT_FAILURE);
    } else {
      write_log("Нащадок відкрив /dev/null на запис трьох потоків: "
                "stdin, stdout, stderr\n");
    }

    dup2(devnull, STDIN_FILENO);
    dup2(devnull, STDOUT_FILENO);
    dup2(devnull, STDERR_FILENO);
    // close(devnull);

    while (1) {
      // Код процесу-демона
      // Відкриття логу та запис про параметри демонізованого процесу
      write_log("-> Процес-демон\n");
      sprintf(str, "Process ID: %d\n", getpid());
      write_log(str);
      sprintf(str, "Deamons Parent PID: %d\n", getppid());
      write_log(str);
      sprintf(str, "Session ID: %d\n", getsid(0));
      write_log(str);
      sleep(DELAY);

      // Завершення процесу-демона
      write_log("*** Завершення процесу-демона *** \n");
      exit(EXIT_SUCCESS);
    }
  }

  return 0;
}
