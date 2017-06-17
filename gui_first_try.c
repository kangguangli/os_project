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

void setPaletee()
{
    int start = 0;
    int end = 15;
    int eflags = readeflags();
    cli();

    unsigned char *table = color_table_rgb;

    outb(0x03c8, start);
    int i = 0;
    for (i = start; i <= end; i++)
    {
        outb(0x03c9, table[i] >> 2);
        outb(0x03c9, table[i + 1] >> 2);
        outb(0x03c9, table[i + 2] >> 2);
        table += 3;
    }
    modifyeflags(eflags);
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
