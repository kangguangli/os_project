#include "types.h"
#include "stat.h"
#include "user.h"
#include "gui.h"
#include "windows.h"
#include "message.h"
#include "font_8x16.h"
#include "bitmap.h"

#define WINDOW_W 400
#define WINDOW_H 400
#define RECTANGLE_NUMBER 10
#define BG_SPEED 3
#define FALL_SPEED 10
#define TOOLBAR 20
void msgproc(struct color24*, int, int*);
void paint(struct color24* hwnd);
void cacheTohwnd(struct color24* hwnd,struct color24* cache);
struct Rec
{
  int x;
  int y;
  int w;
  int h;
};
void drawRec(struct color24*, struct Rec,struct color24 x);
int Interset(struct Rec r1,struct Rec r2);
void bginit(struct Rec*);
int bgfunc(struct color24*,struct Rec*,struct color24 x);
void drawChacter(struct color24*p, int _x, int _y, char ch, struct color24 color);
void drawString(struct color24* p, int _x, int _y, char *str, struct color24 color);
void drawCache(struct Rec*,struct color24 *cache,struct color24 c);

int main(void)
{
  int x = 200;
  int y = 300;
  int width = 400;
  int height = 400 + TOOLBAR;

  int toolbar_x = x;
  int toolbar_y = y;
  int toolbar_width = width;
  int toolbar_height = TOOLBAR;

  int gameover = 0;
  //printf(1, "phase1\n");
  int size = width * height * 3;
  struct Rec* background =
	malloc(sizeof(struct Rec)*(RECTANGLE_NUMBER+1));
  struct Rec* hero = &background[RECTANGLE_NUMBER];
  hero->x = 25;hero->y =150;hero->w = 20;hero->h = 20;
  bginit(background);

  struct color24* hwnd = malloc(size);
  struct color24* cache = malloc(size);
  struct color24 blue = {0xFF, 0, 0};
  struct color24 green = {0, 0xFF, 0};
  struct color24 black = {0,0,0};
  //printf(1, "phase2\n");
  for (int i = 0; i < TOOLBAR * width; i++)
  {
        hwnd[i] = black;
        cache[i] = black;
  }
  for (int i = TOOLBAR * width; i < height * width; i++)
  {
        hwnd[i] = blue;
        cache[i] = blue;
  }
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

  int id = createWindow(x, y, width, height, toolbar_x, toolbar_y, toolbar_width, toolbar_height,
    hwnd);
  updateWholeWindow(id);

  int timer_id = 0;
  int intervals = 150;//必须是10的整数倍
  setTimer(id, timer_id, intervals);
  //printf(1, "phase5\n");
  struct message p;
  int keystatus = 0;
  while (1)
  {
    int cursor_x, cursor_y;
    if (getMessage(id, &p) != -1)
    {
      switch (p.type)
      {
        case WM_TIMER:
	if(!gameover || 1)
		{
		cacheTohwnd(hwnd,cache);

		if(keystatus == 1)hero->y -= 30;
		keystatus = 0;
		if(bgfunc(cache,background,green) == -1){
		gameover = 1;
		drawString(hwnd,x, y, "YOU LOSE", black);}
		updateWholeWindow(id);

		}
          break;
        case WM_KEYDOWN:
	  keystatus = 1;
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
          break;
        case WM_PAINT:
          //绘制窗口，结束后调用updateWholeWindow
          updateWholeWindow(id);
      }
    }
  }
	while(1);
}
void drawCache(struct Rec* bg,struct color24 *cache,struct color24 color)
{
int i = 0;
struct color24 red = {0,0,0xFF};
for( ;i < RECTANGLE_NUMBER;i++)drawRec(cache,bg[i],color);
drawRec(cache,bg[i],red);
}

int bgfunc(struct color24* cache,struct Rec* bg,struct color24 color)
{
int i;

for(i = 0 ;i < RECTANGLE_NUMBER;i++){
bg[i].x -= BG_SPEED;if(bg[i].x < 0)bg[i].x += WINDOW_W;}

if(bg[i].y +bg[i].w + FALL_SPEED<= WINDOW_H && bg[i].y >= TOOLBAR)bg[i].y += FALL_SPEED;
else return -1;

for(int j = 0;j < RECTANGLE_NUMBER;j++)
if(!Interset(bg[i],bg[j]))return -1;

drawCache(bg,cache,color);

return 1;
}
void bginit(struct Rec* bg)
{
        int bd = 80;
	int bwidth = 25;
	for(int i = 0 ;i < RECTANGLE_NUMBER;i+= 2)
	{
		bg[i].x = i * bd;
		bg[i].y = TOOLBAR;
		bg[i].w = bwidth;
		bg[i].h = TOOLBAR + 100 +(i + 20)*(i + 20)%100;

		bg[i + 1].x= i * bd;
		bg[i + 1].y = bg[i].h + (i + 10)*(i + 10)%100 + 70;
		bg[i + 1].w = bwidth;
		bg[i + 1].h = WINDOW_H - bg[i + 1].y;
	}
}
void cacheTohwnd(struct color24* hwnd,struct color24* cache)
{
  //struct color24 black = {0,0,0};
  struct color24 blue = {0xFF, 0xFF, 0};
  for (int i =TOOLBAR * WINDOW_W; i < WINDOW_H * WINDOW_W; i++)
  {
        hwnd[i] = cache[i];
  }
  //  for (int i = 0; i < TOOLBAR * WINDOW_W; i++)
  // {
  //       cache[i] = black;
  // }
  for (int i = TOOLBAR * WINDOW_W; i < WINDOW_H * WINDOW_W; i++)
  {
        cache[i] = blue;
  }
}
void paint(struct color24* hwnd)
{

}
int Interset(struct Rec r1,struct Rec r2)
{
if(r1.x + r1.w > r2.x && r1.x < r2.x)
	{
		if(r1.y + r1.h > r2.y && r1.y < r2.y)return 0;
		else if(r2.y + r2.h >r1.y && r2.y < r1.y)return 0;
	}
else if(r2.x + r2.w > r1.x && r2.x < r1.x)
	{
		if(r1.y + r1.h > r2.y && r1.y < r2.y)return 0;
		else if(r2.y + r2.h >r1.y && r2.y < r1.y)return 0;
	}
return 1;
}
void drawRec(struct color24* cache, struct Rec r,struct color24 color)
{
  for (int i = r.y; i < r.y + r.h; i++)
  {
    for (int j = r.x; j < r.x + r.w; j++)
    {
        cache[i * (WINDOW_W) + j] = color;
    }
  }
}
void msgproc(struct color24* hwnd, int type, int* params)
{
  printf(1, "hh\n");
  return;
}

void drawChacter(struct color24*p, int _x, int _y, char ch, struct color24 color)
{
    int mask[8] = {128, 64, 32, 16, 8, 4, 2, 1};
    uchar *gylph = fontdata_8x16 + (int) (ch) * 16;

    for (int y = 0; y < Char_Height; y++)
    {
        struct color24* _p = p + (_y + y) * WINDOW_W + _x;
        for (int x = 0; x < Char_Width; x++)
        {
            if (gylph[y] & mask[x])
                _p[x] = color;
        }
    }
}

void drawString
        (struct color24* p, int _x, int _y, char *str, struct color24 color)
{
    while (*str != 0)
    {
        drawChacter(p, _x, _y, *str, color);
        _x += 8;
        str++;
    }
}
