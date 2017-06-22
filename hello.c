#include "types.h"
#include "stat.h"
#include "user.h"

#include "gui.h"
#include "windows.h"
#include "message.h"
#include "bitmap.h"


int main(void)
{
  int x = 100;
  int y = 100;
  int width = 100;
  int height = 100;

  int toolbar_x = x;
  int toolbar_y = y;
  int toolbar_width = width;
  int toolbar_height = 20;

  struct color24* hwnd = (struct color24*)malloc(width*height*3);

  int id = createWindow(x, y, width, height, toolbar_x, toolbar_y, toolbar_width, toolbar_height,
    hwnd);
  struct color24 color = {0xFF, 0xFF, 0xFF};
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      hwnd[i * width + j] = color;
    }
  }
  printf(1, "papapa: %d\n", id);

  struct bitmap_file_struct img;
  readBitmap24("close.bmp", &img);
  //printf(1, "phase4%d %d\n", img.bitmap_info_header.height, img.bitmap_info_header.width);
  for (int i = 0; i < 16; i++)
  {
    for (int j = 0; j < 16; j++)
    {
      hwnd[i * width + j] = ((struct color24*)img.buffer)[i * 16 + j];
    }
  }

  updateWholeWindow(id);
  struct message p;
  while (1)
  {
    int cursor_x, cursor_y;
    if (getMessage(id, &p) != -1)
    {
      switch (p.type)
      {
        case WM_TIMER:
          break;
        case WM_KEYDOWN:
          break;
        case WM_LBTNDOWN:
          cursor_x = p.params[0] - x;
          cursor_y = p.params[1] - y;
          if (cursor_x < 18 && cursor_y< 18
            && cursor_x >= 0 && cursor_y >= 0)
          {
            printf(1, "phase2: %d\n", id);
            destroyWindow(id);
            exit();
          }
          break;
        case WM_RBTNDOWN:
        case WM_MBTNDOWN:
        case WM_LDOUBLE:
          //printf(1, "\nnnn");
          break;
        case WM_PAINT:
          //绘制窗口，结束后调用updateWholeWindow
          updateWholeWindow(id);
      }
    }
  }
  return 0;
}
