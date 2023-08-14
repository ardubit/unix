#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {
  int fd;
  if ((fd = open("nonesuch", O_RDONLY)) == -1)
    fprintf(stderr, "Помилка %d\n", errno);

  return 0;
}
