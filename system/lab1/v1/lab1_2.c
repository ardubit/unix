/*
Частина 2: Функції мультиплексованого вводу-виводу
Скласти програму, яка:
- Отримує аргументом командної строки довільний строковий ідентифікатор.
- Налаштовує системний виклик select для очікування читання у вхідному потоці (файловий дескріптор STDIN_FILENO) з таймаутом 5 секунд.
- При отриманні можливості читання, прочитати з потоку буфер довжиною не більше 1024 байта та вивести його у вихідний потік з поміткою у вигляді ідентифікатора п.1.
- При спливанні таймауту, вивести у потік помилок повідомлення про це з поміткою у вигляді ідентифікатора п.1 та знову налаштувати системний виклик select (п.2).
- Протестувати роботу програми отримуючи через вхідний потік результати вводу з клавіатури.

Notes
----------------------
байт = символ ascii
Printing to stdout -> printf
Printing to stderr -> fprintf
printf("%s", "Hello world\n");              // "Hello world" on stdout (using printf)
fprintf(stdout, "%s", "Hello world\n");     // "Hello world" on stdout (using fprintf)
fprintf(stderr, "%s", "Stack overflow!\n"); // Error message on stderr (using fprintf)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

float timeval_tofloat(struct timeval tv)
{
    float seconds = tv.tv_sec + (float)tv.tv_usec / 1000000.0;
    return seconds;
}

int main(int argc, char const *argv[])
{
    // - Отримує аргументами командної строки два імені файлів
    if (argc == 1)
    {
        // printf("Помилка! Вкажіть довільний строковий ідентифікатор. \n");
        fprintf(stderr, "Відсутній довільний строковий ідентифікатор %s id\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Виконання: %s \n", argv[0]);
    }

    /*
    Цей набір служить для стеження появи даних, доступних для читання з будь-якого
    файлового дескриптора. Після повернення з select() у readfds залишаються лише ті
    дескриптори файлів, у тому числі можливе негайне читання.
    */

    fd_set readfds;
    struct timeval tv;
    int r_select;

    const char *identifier = argv[1];
    char buffer[BUFFER_SIZE];

    while (1)
    {

        // Налаштування файлових дескрипторів
        FD_ZERO(&readfds);              // Очистка
        FD_SET(STDIN_FILENO, &readfds); // Додавання

        tv.tv_sec = 5;
        tv.tv_usec = 0;
        float timeout = timeval_tofloat(tv);

        // Виклик select для очікування читання з потоку STDIN
        r_select = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
        if (r_select == -1)
        {
            perror("Помилка виклику select");
            exit(EXIT_FAILURE);
        }
        else if (r_select)
        {
            // Читання з потоку STDIN
            ssize_t r_read_bytes = read(STDIN_FILENO, buffer, sizeof(buffer));

            if (r_read_bytes == -1)
            {
                perror("Помилка виклику select");
                exit(EXIT_FAILURE);
            }
            else if (r_read_bytes == 0)
            {
                printf("Кінець вхідного потоку.\n");
                exit(EXIT_SUCCESS);
            }
            else
            {
                printf("Читання з потоку: [%s]: %.*s\n", identifier, (int)r_read_bytes, buffer);
                fwrite(buffer, r_read_bytes, 1, stdout);
            }
        }
        else
        {
            // Вичерпано таймаут (select_result == 0)
            fprintf(stderr, "[%s] Таймаут %.1f c. Відсутні дані для читання!\n", argv[1], timeout);
        }
    }

    return 0;
}
