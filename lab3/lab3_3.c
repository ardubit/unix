// gcc -o lab3_3 lab3_3.c -lrt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define SHARED_MEMORY_NAME "/my_shared_memory"
#define DATUM_SIZE sizeof(struct Datum)

struct Datum {
    int pid;
    int timestamp;
    char message[100];
};

void signal_handler(int signum) {
    // Обробка сигналу SIGINT
    printf("Received SIGINT signal\n");
    // Видалення розподіленої пам'яті
    shm_unlink(SHARED_MEMORY_NAME);
    exit(0);
}

int main() {
    int shm_fd;
    struct Datum *shared_memory;

    // Створення та відкриття розподіленої пам'яті
    shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // Приведення розміру розподіленої пам'яті до кратного розміру структури датуму
    if (ftruncate(shm_fd, DATUM_SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // Відображення розподіленої пам'яті у пам'ять процесу
    shared_memory = (struct Datum *)mmap(NULL, DATUM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Реєстрація обробника сигналу SIGINT
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    while (1) {
        char input[100];

        // Запит рядка з клавіатури
        printf("Enter a message: ");
        fgets(input, sizeof(input), stdin);

        // Вичитування та виведення вмісту структури датуму
        printf("Datum content: Process ID: %d, Timestamp: %d, Message: %s", shared_memory->pid, shared_memory->timestamp, shared_memory->message);

        // Запис у структуру датуму
        shared_memory->pid = getpid();
        shared_memory->timestamp = time(NULL);
        strncpy(shared_memory->message, input, sizeof(shared_memory->message));

        // Синхронізація з розподіленою пам'яттю
        if (msync(shared_memory, DATUM_SIZE, MS_SYNC) == -1) {
            perror("msync");
            exit(1);
        }
    }

    return 0;
}
