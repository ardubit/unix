#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main() {
    // Створення дочірнього процесу
    pid_t pid = fork();

    // Перевірка на помилку під час виклику fork()
    if (pid == -1) {
        perror("Помилка виклику fork");
        exit(EXIT_FAILURE);
    }

    // Завершення батьківського процесу
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // Створення нової сесії
    if (setsid() == -1) {
        perror("Помилка виклику setsid");
        exit(EXIT_FAILURE);
    }

    // Зміна поточного каталогу
    if (chdir("/") == -1) {
        perror("Помилка виклику chdir");
        exit(EXIT_FAILURE);
    }

    // Закриття стандартних потоків вводу-виводу
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Відкриття файлу для запису
    int fd = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        perror("Помилка відкриття файлу");
        exit(EXIT_FAILURE);
    }

    // Запис рядка у файл
    const char *message = "Привіт, я демон\n";
    if (write(fd, message, strlen(message)) == -1) {
        perror("Помилка запису до файлу");
        exit(EXIT_FAILURE);
    }

    // Закриття файлу
    close(fd);

    // Безкінечний цикл для збереження демона активним
    while (1) {
        // Виконання демонічних операцій
        // ...
        sleep(1);
    }

    return 0;
}
