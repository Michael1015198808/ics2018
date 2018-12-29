#include <unistd.h>
#include <stdio.h>

int main() {
  write(1, "Hello World!\n", 13);
  printf("1234567\n");
  printf("abcde\n");
  //printf("Hello World\n");
  return 0;
  int i = 2;
  volatile int j = 0;
  while (1) {
    j ++;
    if (j == 10000) {
      printf("Hello World from Navy-apps for the %dth time!\n", i ++);
      return 0;
      j = 0;
    }
  }
  return 0;
}
