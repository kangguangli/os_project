#include "types.h"
#include "stat.h"
#include "user.h"
#include "gui.h"
#include "windows.h"
#include "message.h"
#include "bitmap.h"

void msgproc(struct color24*, int, int*);
void paint(struct color24* hwnd);

int main(void)
{
  int x = 0;
  int y = 0;


  int toolbar_x = x;
  int toolbar_y = y;
  int toolbar_width = 0;
  int toolbar_height =0;

  //printf(1, "phase1\n");
  //printf(1, "phase2\n");
  //struct bitmap_file_struct img;
  //readBitmap24("desktop.bmp", &img);

  int width = 1024;
  int height = 768;
  struct color24* hwnd = malloc(1024*768*3);
  // int width = 10*;
  // int height = 10;
  // struct color24* hwnd = malloc(width * height * 3);

  struct color24 blue = {0x21, 0x21, 0x21};
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      hwnd[i * width + j] = blue;
    }
  }

  //printf(1, "phase3\n");
  printf(1, "user %x\n", hwnd);
  int id = createWindow(x, y, width, height, toolbar_x, toolbar_y, toolbar_width, toolbar_height,
    hwnd);
  //printf(1, "phase4\n");
  updateWholeWindow(id);

  int timer_id = 0;
  int intervals = 1000;//必须是10的整数倍
  setTimer(id, timer_id, intervals);

  //printf(1, "phase5\n");
  struct message p;
  while (1)
  {
    if (getMessage(id, &p) != -1)
    {
      switch (p.type)
      {
        case WM_TIMER:
          break;
        case WM_KEYDOWN:
          break;
        case WM_LBTNDOWN:
        case WM_RBTNDOWN:
        case WM_MBTNDOWN:
        case WM_LDOUBLE:
          break;
        case WM_PAINT:
          //绘制窗口，结束后调用updateWholeWindow
          updateWholeWindow(id);
      }
    }
  }
}

void paint(struct color24* hwnd)
{

}

void msgproc(struct color24* hwnd, int type, int* params)
{
  printf(1, "hh\n");
  return;
}
