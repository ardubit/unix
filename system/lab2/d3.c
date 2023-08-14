#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  // Створення нового процесу
  pid_t pid = fork();

  // Перевірка на помилку fork()
  if (pid < 0) {
    perror("Помилка виклику fork");
    exit(EXIT_FAILURE);
  }

  // Завершення батьківського процесу
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  // Зміна файлової маски
  umask(0);

  // Створення нової сесії
  pid_t sid = setsid();
  if (sid < 0) {
    perror("Помилка setsid");
    exit(EXIT_FAILURE);
  }

  // Зміна робочого каталогу на кореневий
  if (chdir("/") < 0) {
    perror("Помилка chdir");
    exit(EXIT_FAILURE);
  }

  // Закриття стандартних файлових дескрипторів
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  // Відкриття файлу для запису
  int logFile = open("/Volumes/DATA-SSD/fel-labs/system/lab2/log.txt",
                     O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (logFile < 0) {
    perror("Помилка відкриття файлу");
    exit(EXIT_FAILURE);
  }

  // Написання повідомлення у файл
  // dprintf() є функцією, яка працює подібно до printf(),
  // але виводить результати у вказаний файловий дескриптор замість стандартного
  // виводу (stdout)

  dprintf(logFile, "Привіт, я демон\n");

  // Безкінечний цикл для демонічних операцій
  while (1) {
    // Операції демона

    // Затримка для демонічних операцій
    sleep(1);
    dprintf(logFile, "Привіт, я демон\n");
    dprintf(logFile, "Process ID: %d\n", getpid());
    dprintf(logFile, "Deamons Parent PID: %d\n", getppid());
    dprintf(logFile, "Session ID: %d\n", getsid(0));

    sleep(1);
  }

  // Закриття файлу
  close(logFile);

  return 0;
}
