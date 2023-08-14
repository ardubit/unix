/*
Частина 2: Розподілена пам`ять
Скласти програму, яка:
- Описує структуру датуму, яка містить ціле значення для ідентифікатора процесу,
ціле значення для мітки часу та строку фіксованої довжини.
- Реєструє об'єкт розподіленої пам`яті через виклик shm_open
- Приводить його до розміру кратного розміру структури датуму
- Відображає отриманий об`єкт у пам`ять через показчик на структуру датуму та
виклик mmap.
- Переходить до нескінченного циклу у якому:
- Запитує рядок з клавіатури.
- Вичитує та презентує вміст структури датуму.
- Записує у структуру натомість свій ідентифікатор процесу, поточний час та
отриманий рядок.
- Протестувати отриману програму, запустивши два її примірники у різних сесіях.
- Пояснити отримані результати.

- Для приведення розміру розподіленого сегмента використовуйте ftruncate
- Перед читанням структури датуму з розподіленого сегмента, використовуйте msync
- Для стеження за останніми записами у логу використовуйте tail -f
- Виклики: sigaction, sigqueue, mmap, shm_open, shm_unlink.
*/

// gcc -o lab3_2 lab3_2.c
// gcc -o lab3_2 lab3_2.c -lrt
// -lrt використовується при компіляції для підключення бібліотеки librt (Realtime Library)
// Журнал: tail -f /dev/shm/lab3_memory

// scp -P 6022 lab3_2.c dm21mx26@ee.kpi.ua:/home/students/dm21mx26/lab3

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define SHARED_MEMORY_NAME "/lab3_memory"
#define DATUM_SIZE sizeof(struct Datum)

struct Datum {
  int pid;
  time_t timestamp;
  char msg[256];
};

void signal_handler(int signum) {
  // Обробка сигналу SIGINT
  printf("Отримано сигнал SIGINT\n");
  // Видалення розподіленої пам'яті
  shm_unlink(SHARED_MEMORY_NAME);
  exit(0);
}

int main() {
  int fd_shm;
  struct Datum *shared_memory;

  // Створення та відкриття розподіленої пам'яті
  fd_shm = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
  if (fd_shm == -1) {
    perror("Помилка: shm_open");
    exit(1);
  }

  // Приведення розміру розподіленої пам'яті до кратного розміру структури
  // датуму
  if (ftruncate(fd_shm, DATUM_SIZE) == -1) {
    perror("Помилка: ftruncate");
    exit(1);
  }

  // Відображення розподіленої пам'яті у пам'ять процесу
  shared_memory = mmap(NULL, DATUM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
  if (shared_memory == MAP_FAILED) {
    perror("Помилка: mmap");
    exit(1);
  }

  // Реєстрація обробника сигналу SIGINT
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = signal_handler;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("Помилка: sigaction");
    exit(1);
  }

  while (1) {
    char input[256];

    printf("Введіть рядок з клавіатури: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    // Отримання локального часу
    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);

    // Форматування часу у рядок
    char time_str[256];
    strftime(time_str, sizeof(time_str), "%c", local_time);

    // Виведення вмісту структури датуму
    printf("Попередній датум:\t Процес ID: %d,\t Час: %s,\t Рядок: %s\n",
           shared_memory->pid, time_str, shared_memory->msg);

    // Запис даних у структуру датуму
    shared_memory->pid = getpid();
    shared_memory->timestamp = current_time;
    strncpy(shared_memory->msg, input, sizeof(shared_memory->msg));

    // Виведення нового вмісту структури датуму
    printf("Оновлений датум:\t Процес ID: %d,\t Час: %s,\t Рядок: %s\n",
        shared_memory->pid, time_str, shared_memory->msg);

    // Синхронізація з розподіленою пам'яттю
    if (msync(shared_memory, DATUM_SIZE, MS_SYNC) == -1) {
      perror("Помилка: msync");
      exit(1);
    }
  }

  return 0;
}
