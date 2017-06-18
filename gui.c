#include "gui.h"
#include "mouse.h"

#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"


extern unsigned char fontdata_8x16[];

struct video_info_struct video_info;

void drawDesktop()
{
    struct color24* p = video_info.video_memory;

    struct color24 color;
    color.R = 0xFF;
    color.G = 0xFF;
    color.B = 0xFF;
    drawRect(p, 0, 0, video_info.screen_width - 1, video_info.screen_height - 29, color);
}

void initVideoInfo()
{
    uint _memory = *(uint*)P2V(Video_Info_Store  + 0x28);
    video_info.video_memory = (struct color24*)(_memory);
    cprintf("\n Memory: %x \n", video_info.video_memory);
    video_info.screen_height = *(ushort*)P2V(Video_Info_Store + 0x14);
    cprintf("\n height: %d \n", video_info.screen_height);
    video_info.screen_width =  *(ushort*)P2V(Video_Info_Store + 0x12);
    cprintf("\n width: %d \n", video_info.screen_width);
    video_info.color_num =  *(uchar*)P2V(Video_Info_Store + 0x19);
    video_info.color_num = (int)(video_info.color_num);
    cprintf("\n color_num: %d \n", video_info.color_num);
    int screen_size = (video_info.screen_width * video_info.screen_height) * 3;
    video_info.cache1 = video_info.video_memory + screen_size;
    video_info.cache2 = video_info.cache1 + screen_size;
}

void drawRect(struct color24* p, int x, int y,
              int width, int height, struct color24 color)
{
    //int _x;
    //int _y;
    for (int _y = y; _y <= y + height; _y++)
    {
        for (int _x = x; _x <= x + width; _x++)
        {
            p[_y * video_info.screen_width + _x] = color;
        }
    }
}

void drawPixel(struct color24*p, int x, int y, struct color24 color)
{
    p[y * video_info.screen_width + x] = color;
}

void drawChacter(struct color24*p, int _x, int _y, char ch, struct color24 color)
{
    int mask[8] = {128, 64, 32, 16, 8, 4, 2, 1};
    uchar *gylph = fontdata_8x16 + (int) (ch) * 16;

    for (int y = 0; y < Char_Height; y++)
    {
        struct color24* _p = p + (_y + y) * video_info.screen_width + _x;
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

void drawCursor(int x, int y)
{
    struct color24* p = video_info.video_memory;

    for (int _y = 0; _y < Mouse_Shape_Height; _y++)
    {
        for (int _x = 0; _x < Mouse_Shape_Width; _x++)
        {
            struct color24 color1, color2;
            color1.R = 0;
            color1.G = 0;
            color1.B = 0;
            color2.R = 0;
            color2.G = 0;
            color2.B = 0;
            if (mouse_shape[_y][_x] == 1)
                p[(y + _y) * video_info.screen_width + x + _x] = color1;
            if (mouse_shape[_y][_x] == 2)
                p[(y + _y) * video_info.screen_width + x + _x] = color2;
        }
    }
}

void drawContentToContent(struct color24* src, struct color24* dst,
  int src_x, int src_y, int src_width, int src_height, int dst_x, int dst_y,
  int dst_width, int dst_height)
{
    for (int i = 0; i < src_height; i++)
      for (int j = 0; j < src_width; j++)
        dst[(dst_y + i) * dst_width + dst_x + j]
          = src[(src_y + i) * src_width + src_x + j];
}
