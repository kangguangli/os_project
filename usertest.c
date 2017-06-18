#include "types.h"
#include "stat.h"
#include "user.h"
#include "gui.h"

void msgproc(struct color24*, int, int*);

int main(void)
{
  int x = 200;
  int y = 300;
  int width = 100;
  int height = 100;

  int toolbar_x = x;
  int toolbar_y = y;
  int toolbar_width = width;
  int toolbar_height = 30;

  //printf(1, "phase1\n");
  int size = width * height * 3;
  struct color24* hwnd = malloc(width * height * 3);
  struct color24 blue = {0xFF, 0, 0};
  struct color24 green = {0, 0xFF, 0};
  //printf(1, "phase2\n");

  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      if (i < 30)
        hwnd[i * width + j] = blue;
      else
        hwnd[i * width + j] = green;
      if (i * width + j >= size)
        printf(1, "wrong\n");
    }
  }

  //printf(1, "phase3\n");
  int id = createWindow(x, y, width, height, toolbar_x, toolbar_y, toolbar_width, toolbar_height,
    hwnd, msgproc);
  //printf(1, "phase4\n");
  updateWholeWindow(id);
  //printf(1, "phase5\n");
  while (1)
  ;
}

void msgproc(struct color24* hwnd, int type, int* params)
{
  return;
}
