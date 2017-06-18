#include "windows.h"
#include "gui.h"
#include "message.h"

#include "types.h"
#include "defs.h"


struct window_manager_struct window_manager;
struct message message_queue[Max_Message_Num];

void deviceMessageProc()
{
  uint data = fifoGet(&device_buf);
  if (data >= Mouse_Offset)
  {
    cprintf("\n mouse: %x\n", data);
    mouseHandle(0, data - Mouse_Offset);
  }
  else if (data >= Keyboard_Offset)
  {
    cprintf("\n kbd: %x\n", data);
    kbdHandle(data - Keyboard_Offset);
  }
  else
    ;
}


void messageHandle(struct message msg)
{

}

void windowsManagerInit()
{
  for (int i = 0; i < Max_Window_Num; i++)
  {
    window_manager.windows[i].id = -1;
    //window_manager.layouts[i] = -1;
  }
  window_manager.focus = 0;

  for (int i = 0; i < Max_Message_Num; i++)
    message_queue[i].type = -1;
}

int _createWindow(int x, int y, int width, int height, int toolbar_x, int toolbar_y,
  int toolbar_width, int toolbar_height, struct color24* p,
  void (*msgproc)(struct color24*, int, int*))
{
  int id;
  for (id = 0; id < Max_Window_Num; id++)
    if (window_manager.windows[id].id == -1)
        break;

  window_manager.windows[id].id = id;
  window_manager.windows[id].x = x;
  window_manager.windows[id].y = y;
  window_manager.windows[id].width = width;
  window_manager.windows[id].height = height;

  window_manager.windows[id].toolbar_x = toolbar_x;
  window_manager.windows[id].toolbar_y = toolbar_y;
  window_manager.windows[id].toolbar_width = toolbar_width;
  window_manager.windows[id].toolbar_height = toolbar_height;

  window_manager.windows[id].hwnd = p;
  window_manager.windows[id].msgproc = msgproc;

  window_manager.focus = &window_manager.windows[id];
  //window_manager.focus = id;
  return id;
}

int sys_createWindow()
{
  int x, y, width, height;
  int toolbar_x, toolbar_y, toolbar_width, toolbar_height;

  argint(0, &x);
  argint(1, &y);
  argint(2, &width);
  argint(3, &height);

  argint(4, &toolbar_x);
  argint(5, &toolbar_y);
  argint(6, &toolbar_width);
  argint(7, &toolbar_height);

  int p;
  argint(8, &p);
  int proc;
  argint(9, &proc);

  int id = _createWindow(x, y, width, height, toolbar_x, toolbar_y, toolbar_width,
    toolbar_height, (struct color24*) p, (void(*)(struct color24*, int, int*))proc);
  return id;
}

int _updateWholeWindow(int id)
{
  struct window_info* wnd = &window_manager.windows[id];
  // for (int i = 0; i < Max_Window_Num; i++)
  //   if (window_manager.windows[i].id == id)
  //   {
  //     wnd = &window_manager.windows[i];
  //     break;
  //   }
  drawContentToContent(wnd->hwnd, video_info.cache2, 0, 0, wnd->width,
    wnd->height, wnd->x, wnd->y, video_info.screen_width, video_info.screen_height);
  drawContentToContent(wnd->hwnd, video_info.video_memory, 0, 0, wnd->width,
    wnd->height, wnd->x, wnd->y, video_info.screen_width, video_info.screen_height);

  return 0;
}

int sys_updateWholeWindow()
{
 int id;
 argint(0, &id);

 _updateWholeWindow(id);

 return 0;
}
