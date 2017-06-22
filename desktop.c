#include "types.h"
#include "stat.h"
#include "user.h"

#include "gui.h"
#include "windows.h"
#include "message.h"
#include "bitmap.h"


int main(void)
{
  int x = 0;
  int y = 00;
  int width = 1024;
  int height = 768;

  int toolbar_x = 0;
  int toolbar_y = 0;
  int toolbar_width = 0;
  int toolbar_height = 0;

  struct color24* hwnd = (struct color24*)malloc(width*height*3);


  struct color24 color = {0xFF, 0xFF, 0xFF};
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      hwnd[i * width + j] = color;
    }
  }

  int id = createWindow(x, y, width, height, toolbar_x, toolbar_y, toolbar_width, toolbar_height,
    hwnd);
  int game_x = 20;
  int game_y = 20;
  int game_width = 80;
  int game_height = 80;
  struct bitmap_file_struct img;
  readBitmap24("bird.bmp", &img);
  //printf(1, "phase4%d %d\n", img.bitmap_info_header.height, img.bitmap_info_header.width);
  for (int i = 0; i < game_height; i++)
  {
    for (int j = 0; j < game_width; j++)
    {
      hwnd[(game_height - 1 - i + game_y) * width + j + game_x] = ((struct color24*)img.buffer)[i * game_width + j];
    }
  }

  int notepad_x = 20;
  int notepad_y = 120;
  int notepad_width = 80;
  int notepad_height = 80;
  struct bitmap_file_struct img1;
  readBitmap24("notepad.bmp", &img1);
  //printf(1, "phase4%d %d\n", img.bitmap_info_header.height, img.bitmap_info_header.width);
  for (int i = 0; i < notepad_height; i++)
  {
    for (int j = 0; j < notepad_width; j++)
    {
      hwnd[(notepad_height - 1 - i + notepad_y) * width + j + notepad_x] = ((struct color24*)img1.buffer)[i * notepad_width + j];
    }
  }

  int rm_x = 20;
  int rm_y = 220;
  int rm_width = 80;
  int rm_height = 80;
  struct bitmap_file_struct img2;
  readBitmap24("file.bmp", &img2);
  //printf(1, "phase4%d %d\n", img.bitmap_info_header.height, img.bitmap_info_header.width);
  for (int i = 0; i < rm_height; i++)
  {
    for (int j = 0; j < rm_width; j++)
    {
      hwnd[(rm_height - 1 - i + rm_y) * width + j + rm_x] = ((struct color24*)img2.buffer)[i * rm_width + j];
    }
  }

  updateWholeWindow(id);
  struct message p;
  while (1)
  {
    int cursor_x, cursor_y;
    char *argv[] = { 0 };
    if (getMessage(id, &p) != -1)
    {
      switch (p.type)
      {
        case WM_TIMER:
          break;
        case WM_KEYDOWN:
          break;
        case WM_LBTNDOWN:

          break;
        case WM_RBTNDOWN:
        case WM_MBTNDOWN:
        case WM_LDOUBLE:
          cursor_x = p.params[0] - x;
          cursor_y = p.params[1] - y;
          if (cursor_x < game_x + game_width && cursor_y < game_y + game_height
            && cursor_x >= game_x && cursor_y >= game_y)
          {
            if (fork() == 0)
              exec("game", argv);
          }
          else if (cursor_x < notepad_x + notepad_width && cursor_y < notepad_y + notepad_height
            && cursor_x >= notepad_x && cursor_y >= notepad_y)
          {
            if (fork() == 0)
              exec("notepad", argv);
          }
          else if (cursor_x < rm_x + rm_width && cursor_y < rm_y + rm_height
            && cursor_x >= rm_x && cursor_y >= rm_y)
          {
            if (fork() == 0)
              exec("resourcemanager", argv);
          }
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
