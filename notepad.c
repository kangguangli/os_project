#include "types.h"
#include "stat.h"
#include "user.h"
#include "gui.h"
#include "windows.h"
#include "message.h"
#include "font_8x16.h"
#include "my_list.h"
#include "fs.h"
#include "fcntl.h"
#include "bitmap.h"
#define KEY_UP          0xE2
#define KEY_DN          0xE3
#define KEY_LF          0xE4
#define KEY_RT          0xE5
#define KEY_DEL         0xE9



void msgproc(struct color24*, int, int*);
void drawChacter(struct color24*p, int _x, int _y, char ch, struct color24 color, int windowWidth);
void keyPress(struct color24*hwnd, int *cursor_pos, int cursor_width, int cursor_height, struct message p, int width,int height, struct mylist* ListHead);
void mousePress(struct color24*hwnd, int *cursor_pos, int cursor_width, int cursor_height, struct message p, int width, int x, int y);
void Windowclear(struct color24*hwnd, int width, int height);
void WinUpdate(struct color24*,int w,int h,int cursor_w,int cursor_h,struct mylist *);
int readfile(char *path,char **fI,char *content,int rw);
int writefile(char *path,char **fI,char *content);

int main(int argv, char** argc)
{

  int x = 200;
  int y = 200;
  int width = 500;
  int height = 500;
  int toolbar_x = x;
  int toolbar_y = y;
  int toolbar_width = width;
  int toolbar_height = 20;
  struct mylist* ListHead = 0;
  ListHead = createNode();
  int cursor_x = 0;
  int cursor_y = toolbar_height;
  int cursor_pos[2];
  cursor_pos[0] = cursor_x;
  cursor_pos[1] = cursor_y;
  int cursor_width = Char_Width;
  int cursor_height = Char_Height;
  //printf(1, "phase1\n");
  int size = width * height * 3;
  struct color24* hwnd = malloc(width * height * 3);
  struct color24 gray = {0xA8,0xA8,0xA8};
  struct color24 red = {0X14,0x96,0x8B};
  //printf(1, "phase2\n");

  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      if (i < 20)
        hwnd[i * width + j] = gray;
	    else
        hwnd[i * width + j] = red;
      if (i * width + j >= size)
        printf(1, "wrong\n");
    }
  }

  struct color24 white = {0xFF, 0xFF, 0xFF};

  int blink = 0;
  for (int i = cursor_y; i < cursor_y + cursor_height; i++)
  {
    for (int j = cursor_x; j < cursor_x + cursor_width; j++)
    {
		hwnd[i * width + j] = white;
		 if (i * width + j >= size)
        printf(1, "wrong\n");
	}
  }
  //printf(1, "phase3\n");
  printf(1, "user %x\n", hwnd);
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
  //printf(1, "phase4\n");
  //读取文件初始化
	 // 打开文件初始化
  char *content = 0;
  if(argv == 1)
  {
	printf(1, "%s\n", argc[0]);
	for(int i = 0; ; i++)
	{
		if(argc[0][i] == ' ' || argc[0][i] == '\0')
		{
			argc[0][i] = '\0';
			break;
		}
	}
	int length = readfile(argc[0], 0, content, 0);
	int num = 1;
	if(length > 0)
		{
		for(int i = 0; i < length; i++)
		{
		if(content[i] != '\n')
		{
			insert_node_to_list(ListHead, num, content[i]);
			num++;
		}
		else
		{
			while(1)
			{

				if(num % 62 == 0)
				{
					insert_node_to_list(ListHead, num, ' ');
					num++;
					break;
				}
				insert_node_to_list(ListHead, num, ' ');
				num++;
			}
		}
		}
	  }

  }

	Windowclear(hwnd, width, height);

	WinUpdate(hwnd,width,height,cursor_width,cursor_height,ListHead);
	updateWholeWindow(id);

  int timer_id = 0;
  int intervals = 800;//必须是10的整数倍
  setTimer(id, timer_id, intervals);

  //printf(1, "phase5\n");
  struct message p;
  char* txt = 0;
  while (1)
  {
    int _cursor_x, _cursor_y;
    if (getMessage(id, &p) != -1)
    {
      switch (p.type)
      {
        case WM_TIMER:
		 	for (int i = cursor_y; i < cursor_y + cursor_height; i++)
			{
			for (int j = cursor_x; j < cursor_x + cursor_width; j++)
			 {
				 if(blink == 0)
				{
					 hwnd[i * width + j] = red;
				}
				else
					 hwnd[i * width + j] = white;
				if (i * width + j >= size)
   					printf(1, "wrong\n");
			 }
			}
			if(blink == 0)
			{
				struct color24 blue = {0xFF, 0xFF, 0x00};
				int num = (cursor_x / cursor_width + 1)
					+ ((cursor_y - 20) / cursor_height) * 62;
				node_p p = search_node(ListHead,num);
				if(p!=0)
					drawChacter(hwnd, cursor_x, cursor_y, p->ch, blue, width);
				blink = 1;
			}
    		else
			  blink = 0;
          updateWholeWindow(id);
          break;
        case WM_KEYDOWN:

			keyPress(hwnd, cursor_pos, cursor_width, cursor_height,  p, width, height, ListHead);
			cursor_x = cursor_pos[0];
			cursor_y = cursor_pos[1];
			updateWholeWindow(id);
		  break;
        case WM_LBTNDOWN:
        _cursor_x = p.params[0] - x;
        _cursor_y = p.params[1] - y;
        if (_cursor_x < 18 && _cursor_y< 18
          && _cursor_x >= 0 && _cursor_y >= 0)
        {
          //printf(1, "phase2: %d\n", id);
          destroyWindow(id);
          exit();
        }
        break;
			 mousePress(hwnd, cursor_pos, cursor_width, cursor_height, p, width,  x, y);
			 Windowclear(hwnd, width, height);

			  	cursor_x = cursor_pos[0];
			  	cursor_y = cursor_pos[1];
			WinUpdate(hwnd,width,height,cursor_width,cursor_height,ListHead);
			  updateWholeWindow(id);
			  break;
        case WM_RBTNDOWN:

		case WM_MBTNDOWN:

		case WM_LDOUBLE:
			  txt = toCharArray(ListHead);
			  writefile("result", 0, txt);
			  exit();
      	      break;
        case WM_PAINT:
          //绘制窗口，结束后调用updateWholeWindow
		updateWholeWindow(id);
			  break;
      }
    }
  }
}

void msgproc(struct color24* hwnd, int type, int* params)
{
  printf(1, "h\n");
  return;
}


void drawChacter(struct color24*p, int _x, int _y, char ch, struct color24 color, int windowWidth)
{
    int mask[8] = {128, 64, 32, 16, 8, 4, 2, 1};
    uchar *gylph = fontdata_8x16 + (int) (ch) * 16;

    for (int y = 0; y < Char_Height; y++)
    {
        struct color24* _p = p + (_y + y) * windowWidth + _x;
        for (int x = 0; x < Char_Width; x++)
        {
            if (gylph[y] & mask[x])
                _p[x] = color;
        }
    }
}

void keyPress(struct color24*hwnd, int *cursor_pos, int cursor_width, int cursor_height,
          			struct message p, int width, int height, struct mylist* ListHead)
{
	int cursor_x = cursor_pos[0];
	int cursor_y = cursor_pos[1];
	struct color24 white = {0xFF, 0xFF, 0xFF};
	struct color24 red = {0X14,0x96,0x8B};
	struct color24 blue = {0xFF, 0xFF, 0x00};
	if(p.type != WM_KEYDOWN)
		return;
	if(p.params[1] == '\n')// 回车
	{

		int x1 = 62 - cursor_x / cursor_width;
		for(int i = 0; i < x1; i++)
		{
			int num = (cursor_x / cursor_width + 1)
			+ ((cursor_y - 20) / cursor_height) * 62;
			insert_node_to_list(ListHead, num, ' ');
		}
		cursor_y += cursor_height;
		cursor_x = 0;
		if(cursor_y + cursor_height >= height)
		{
			cursor_y -= cursor_height;
			cursor_x = 62 * cursor_width;
		}
		Windowclear(hwnd, width, height);

		WinUpdate(hwnd,width,height,cursor_width,cursor_height,ListHead);


		cursor_pos[0] = cursor_x;
		cursor_pos[1] = cursor_y;
		return;

	}

	if(p.params[1] == KEY_DEL) //删除
	{
		int num = (cursor_x / cursor_width + 1)
			+ ((cursor_y - 20) / cursor_height) * 62;
		if(num <= 1)
			return;
		ListHead = delete_node(ListHead, num - 1);

		cursor_x -= cursor_width;

		if(cursor_x  < 0)
		{
			if(cursor_y - cursor_height < 20)
			{
				cursor_x = 0;
			}
			else
			{
				cursor_y -= cursor_height;
				cursor_x = cursor_width * (62 - 1);
			}
		}
		Windowclear(hwnd, width, height);

		WinUpdate(hwnd,width,height,cursor_width,cursor_height,ListHead);
		cursor_pos[0] = cursor_x;
		cursor_pos[1] = cursor_y;
		return;


	}
	if(p.params[1] == KEY_RT) // 右移
	{
		for (int i = cursor_y; i < cursor_y + cursor_height; i++)
			{
			for (int j = cursor_x; j < cursor_x + cursor_width; j++)
			 {

					 hwnd[i * width + j] = red;

			 }
			}
		int num = (cursor_x / cursor_width + 1)
					+ ((cursor_y - 20) / cursor_height) * 62;
		node_p p = search_node(ListHead,num);
		if(p!=0)
			drawChacter(hwnd, cursor_x, cursor_y, p->ch, blue, width);
		cursor_x += cursor_width;
		if(cursor_x + cursor_width >= width)
		{
			cursor_x = 0;
			cursor_y += cursor_height;
		}
		for (int i = cursor_y; i < cursor_y + cursor_height; i++)
		{
		for (int j = cursor_x; j < cursor_x + cursor_width; j++)
		 {
			if (i * width + j >= width * 500)
			{
   				printf(1, "wrong\n");
				continue;
			}
			hwnd[i * width + j] = white;
		}
		}
			cursor_pos[0] = cursor_x;
			cursor_pos[1] = cursor_y;
		return;
	}
	if(p.params[1] == KEY_LF) // 左移
	{
	for (int i = cursor_y; i < cursor_y + cursor_height; i++)
			{
			for (int j = cursor_x; j < cursor_x + cursor_width; j++)
			 {

					 hwnd[i * width + j] = red;

			 }
			}
		int num = (cursor_x / cursor_width + 1)
					+ ((cursor_y - 20) / cursor_height) * 62;
		node_p p = search_node(ListHead,num);
		if(p!=0)
			drawChacter(hwnd, cursor_x, cursor_y, p->ch, blue, width);

		cursor_x -= cursor_width;

		if(cursor_x  < 0)
		{
			if(cursor_y - cursor_height < 20)
			{
				cursor_x = 0;
			}
			else
			{
				cursor_y -= cursor_height;
				cursor_x = cursor_width * (62 - 1);
			}
		}
		for (int i = cursor_y; i < cursor_y + cursor_height; i++)
		{
		for (int j = cursor_x; j < cursor_x + cursor_width; j++)
		{
			if (i * width + j >= width * 500)
			{
   				printf(1, "wrong\n");
				continue;
			}
			hwnd[i * width + j] = white;
		}
		}
			cursor_pos[0] = cursor_x;
			cursor_pos[1] = cursor_y;
		return;
	}
	if(p.params[1] == KEY_UP) // 上移
	{

		for (int i = cursor_y; i < cursor_y + cursor_height; i++)
			{
			for (int j = cursor_x; j < cursor_x + cursor_width; j++)
			 {

					 hwnd[i * width + j] = red;

			 }
			}
		int num = (cursor_x / cursor_width + 1)
					+ ((cursor_y - 20) / cursor_height) * 62;
		node_p p = search_node(ListHead,num);
		if(p!=0)
			drawChacter(hwnd, cursor_x, cursor_y, p->ch, blue, width);

			if(cursor_y - cursor_height < 20)
			{

			}
			else
			{
				cursor_y -= cursor_height;
			}
		for (int i = cursor_y; i < cursor_y + cursor_height; i++)
		{
		for (int j = cursor_x; j < cursor_x + cursor_width; j++)
		{
			if (i * width + j >= width * 500)
			{
   				printf(1, "wrong\n");
				continue;
			}
			hwnd[i * width + j] = white;
		}
		}
			cursor_pos[0] = cursor_x;
			cursor_pos[1] = cursor_y;
		return;
	}
	if(p.params[1] == KEY_DN) // 下移
	{

		for (int i = cursor_y; i < cursor_y + cursor_height; i++)
			{
			for (int j = cursor_x; j < cursor_x + cursor_width; j++)
			 {

					 hwnd[i * width + j] = red;

			 }
			}
		int num = (cursor_x / cursor_width + 1)
					+ ((cursor_y - 20) / cursor_height) * 62;
		node_p p = search_node(ListHead,num);
		if(p!=0)
			drawChacter(hwnd, cursor_x, cursor_y, p->ch, blue, width);

			cursor_y += cursor_height;
			if(cursor_y + cursor_height >= height)
			{
				cursor_y -= cursor_height;
			}

		for (int i = cursor_y; i < cursor_y + cursor_height; i++)
		{
		for (int j = cursor_x; j < cursor_x + cursor_width; j++)
		{
			if (i * width + j >= width * 500)
			{
   				printf(1, "wrong\n");
				continue;
			}
			hwnd[i * width + j] = white;
		}
		}
			cursor_pos[0] = cursor_x;
			cursor_pos[1] = cursor_y;
		return;
	}

	if(p.params[1] >= 32 && p.params[1] <= 126)	//输入字母
	{
		int num = (cursor_x / cursor_width + 1)
			+ ((cursor_y - 20) / cursor_height) * 62;
		char c = (char)p.params[1];
		if(p.params[0])
		{
			if('a' <= c && c <= 'z')
  				c += 'A' - 'a';
			else if('A' <= c && c <= 'Z')
 				 c += 'a' - 'A';
		}

		insert_node_to_list(ListHead, num, c);
		cursor_x += cursor_width;
		Windowclear(hwnd, width, height);
		node_p q1;
		q1 = ListHead;
		q1 = q1->next;
		while(1)
		{
			if(q1 == 0)
				break;
			if(q1->num >= 1)
			{

				int x1 = ((q1->num % 62) - 1) * cursor_width;
				int y1 = (q1->num / 62) * cursor_height + 20;

				if((q1->num % 62) == 0)
				{

					x1 = (62 - 1) * cursor_width;
					y1 = (q1->num / 62 - 1) * cursor_height + 20;

 				}

				drawChacter(hwnd, x1, y1, q1->ch, blue, width);
			}
			q1 = q1->next;
		}

		if(cursor_x + cursor_width >= width)
		{
			cursor_x = 0;
			cursor_y += cursor_height;
		}


	}

	cursor_pos[0] = cursor_x;
	cursor_pos[1] = cursor_y;

}



void mousePress(struct color24*hwnd, int *cursor_pos, int cursor_width, int cursor_height, struct message p, int width, int x, int y)
{

	int cursor_x = cursor_pos[0];
	int cursor_y = cursor_pos[1];
	struct color24 white = {0xFF, 0xFF, 0xFF};
	struct color24 red = {0X14,0x96,0x8B};
	if(p.type != WM_LBTNDOWN)
		return;
	if(p.params[0] - x < 0 || p.params[0] - x > width || p.params[1] - y < 20 )
		return;
	for (int i = cursor_y; i < cursor_y + cursor_height; i++)
	{
		for (int j = cursor_x; j < cursor_x + cursor_width; j++)
		{
		if (i * width + j >= width* 500)
		{
   			printf(1, "hghg\n");
			continue;
		}
		hwnd[i * width + j] = red;

	  }
	}
	cursor_height = cursor_height- 1 + 1;
	cursor_x = (p.params[0] - x) / Char_Width * Char_Width;
	cursor_y = (p.params[1] - y - 20) / Char_Height * Char_Height + 20;
	if(cursor_x >= width)
	{
		cursor_x = 0;
		cursor_y += cursor_width;
	}

	for (int i = cursor_y; i < cursor_y + cursor_height; i++)
	{
	for (int j = cursor_x; j < cursor_x + cursor_width; j++)
	 {
		if (i * width + j >= width * 500)
		{
   			printf(1, "wrong\n");
			continue;
		}
		hwnd[i * width + j] = white;
	  }
	}
	cursor_pos[0] = cursor_x;
	cursor_pos[1] = cursor_y;

}


void Windowclear(struct color24*hwnd, int width, int height)
{

  struct color24 red = {0X14,0x96,0x8B};
  //printf(1, "phase2\n");

  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      if (i < 20)
       ;
	    else
        hwnd[i * width + j] = red;
      if (i * width + j >= height * width)
        printf(1, "wrong\n");
    }
  }
}

void WinUpdate(struct color24* hwnd,int width,int height,int cursor_width,int cursor_height,struct mylist *ListHead)
{
  struct color24 blue = {0xFF, 0xFF, 0x00};
	node_p q1;
	q1 = ListHead;
	q1 = q1->next;
	while(1)
	{
	if(q1 == 0)
		break;
	if(q1->num >= 1)
	{
	int x1 = ((q1->num % 62) - 1) * cursor_width;
	int y1 = (q1->num / 62) * cursor_height + 20;
	if((q1->num % 62) == 0)
	{x1 = (62 - 1) * cursor_width;y1 = (q1->num / 62 - 1) * cursor_height + 20;}
	drawChacter(hwnd, x1, y1, q1->ch, blue, width);
	q1 = q1->next;
	}
	}
}

int readfile(char *path,char **fI,char *content,int rw)
{
char buf[512];
int fd;

struct stat st;

if((fd = open(path, 0)) < 0){
  printf(2, "ls: cannot open %s\n", path);
  return 0;
}

if(fstat(fd, &st) < 0){
  printf(2, "ls: cannot stat %s\n", path);
  close(fd);
  return 0;
}
int i = 0;
int mfd;
switch(st.type){

case T_FILE:
mfd = open(path, O_RDWR);
if(rw == 0){
i = read(mfd,buf,512);
strcpy(content,buf);
content[511] = 0;
}
else {
	if(mfd < 0)
		return 0;
	strcpy(buf,content);
	buf[511] = 0;
	i = write(mfd,buf,strlen(buf));
}
}

close(fd);
close(mfd);
return i;
}

int writefile(char *path,char **fI,char *content)
{
char *buf = 0;

int i = 0;
int mfd;
if((mfd = open(path, O_CREATE | O_WRONLY)) < 0)
	{
  printf(2, "ls: cannot open %s\n", path);
  close(mfd);
  return 0;
}
	strcpy(buf,content);
	buf[511] = 0;
	i = write(mfd,buf,strlen(buf));
close(mfd);

return i;

}
