#include "gui_first_try.h"
#include "mouse.h"

#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"

extern unsigned char fontdata_8x16[];

static unsigned char color_table_rgb[16 * 3] =
{
  0x00, 0x00, 0x00, //Black       0
  0xff, 0x00, 0x00, //Pure Red    1
  0x00, 0xff, 0x00, //Pure Green  2
  0xff, 0xff, 0x00, //Pure Yellow 3
  0x00, 0x00, 0xff, //Pure Blue   4
  0xff, 0x00, 0xff, //Pure Purple 5
  0x00, 0xff, 0xff, //Light Blue  6
  0xff, 0xff, 0xff, //White       7
  0xc6, 0xc6, 0xc5, //Grey        8
  0x84, 0x00, 0x00, //Deep Red    9
  0x00, 0x84, 0x00, //Deep Green  10
  0x84, 0x84, 0x00, //Deep Yellow 11
  0x00, 0x00, 0x84, //Deep Blue   12
  0x84, 0x00, 0x84, //Deep Purple 13
  0x00, 0x84, 0x84, //Deep Blue   14
  0x84, 0x84, 0x84  //Deep Grey   15
};

void drawDesktop()
{
  uchar* p = (uchar*)P2V(Video_Cache_Start);

  drawRect(p, 0, 0, Screen_Width - 1, Screen_Height - 29, 14);
  drawRect(p, 0, Screen_Height - 28, Screen_Width - 1, 0, 8);
  drawRect(p, 0, Screen_Height - 27, Screen_Width - 1, 0, 7);
  drawRect(p, 0, Screen_Height - 26, Screen_Width - 1, 25, 8);

  drawRect(p, 3, Screen_Height - 24, 56, 0, 7);
  drawRect(p, 2, Screen_Height - 24, 0, 20, 7);
  drawRect(p, 3, Screen_Height - 4, 56, 0, 15);
  drawRect(p, 59, Screen_Height - 23, 0, 18, 15);
  drawRect(p, 2, Screen_Height - 3, 57, 0, 1);
  drawRect(p, 60, Screen_Height - 24, 0, 21, 1);

  drawRect(p, Screen_Width - 47, Screen_Height - 24, 43, 0, 15);
  drawRect(p, Screen_Width - 47, Screen_Height - 23, 0, 19, 15);
  drawRect(p, Screen_Width - 47, Screen_Height - 3, 43, 0, 7);
  drawRect(p, Screen_Width - 3, Screen_Height - 24, 0, 21, 7);

  drawString(p, 30, 20, "Hello world", 0);
}

void drawRect(uchar* p, int x, int y,
  int width, int height, int color)
{
  //int _x;
  //int _y;
  for (int _y = y; _y <= y + height; _y++)
  {
    for (int _x = x; _x <= x + width; _x++)
    {
      p[_y * Screen_Width + _x] = color;
    }
  }
}

void setPaletee()
{
  int start = 0;
  int end = 15;
  int eflags = readeflags();
  cli();

  unsigned char* table = color_table_rgb;

  outb(0x03c8, start);
  int i = 0;
  for (i = start; i <= end; i++)
  {
    outb(0x03c9, table[i] >> 2);
    outb(0x03c9, table[i+1] >> 2);
    outb(0x03c9, table[i+2] >> 2);
    table += 3;
  }
  modifyeflags(eflags);
}

void drawPixel(uchar* p, int x, int y, int color)
{
  p[y * Screen_Width + x] = color;
}

void drawChacter(uchar* p, int _x, int _y, char ch, int color)
{
  int mask[8] = {128,64,32,16,8,4,2,1};
  uchar* gylph = fontdata_8x16 + (int)(ch)*16;

  for (int y = 0; y < Char_Height; y++)
  {
    uchar* _p = p + (_y + y) * Screen_Width + _x;
    for (int x = 0; x < Char_Width; x++)
    {
      if (gylph[y] & mask[x])
        _p[x] = color;
    }
  }
}

void drawString
(uchar* p, int _x, int _y, char* str, int color)
{
  while (*str != 0)
  {
    drawChacter(p, _x, _y, *str, color);
    _x += 8;
    str++;
  }
}


void drawCursor(int x, int y)
{
  uchar* p = (uchar*)P2V(Video_Cache_Start);

  for (int _y = 0; _y < Mouse_Shape_Height; _y++)
  {
    for (int _x = 0; _x < Mouse_Shape_Width; _x++)
    {
      if (mouse_shape[_y][_x])
        p[(y + _y) * Screen_Width + x + _x] = mouse_shape[_y][_x];
    }
  }
}

//Extremly Stupid, Estimate Need
void printInfo(char* str)
{
  uchar* p = (uchar*)P2V(Video_Cache_Start);
  drawString(p, 100, 100, str, 0);
}
