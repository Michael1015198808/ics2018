#include <assert.h>
#include <stdlib.h>
#include <ndl.h>
#include <stdio.h>

int main() {
  //puts("Hello0");
  NDL_Bitmap *bmp = (NDL_Bitmap *) malloc(sizeof(NDL_Bitmap));
  //puts("Hello1");
  NDL_LoadBitmap(bmp, "/share/pictures/projectn.bmp");
  //puts("Hello2");
  assert(bmp->pixels);
  puts("Hello3");
  printf("%d,%d\n",bmp->w,bmp->h);
  NDL_OpenDisplay(bmp->w, bmp->h);
  puts("Hello4");
  NDL_DrawRect(bmp->pixels, 0, 0, bmp->w, bmp->h);
  puts("Hello5");
  NDL_Render();
  puts("Hello6");
  NDL_CloseDisplay();
  puts("Hello7");
  while (1);
  return 0;
}
