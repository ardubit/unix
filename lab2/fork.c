#include <unistd.h>
#include <stdio.h>

int main()
{
    pid_t pid; // process-id

    printf("Виклик fork\n");
    pid = fork(); // Створення нового процесу
    if (pid == 0)
        printf("Процес-нащадок, pid %d\n", pid);
    else if (pid > 0)
        printf("Процес-батько, pid %d\n", pid);
    else
        printf("Помилка fork, нащадок не створено\n");
}
