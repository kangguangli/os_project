#include "windows.h"
#include "gui_first_try.h"

#include "types.h"
#include "defs.h"


struct window_manager_struct window_manager;

void windowsManagerInit()
{
  for (int i = 0; i < Max_Window_Num; i++)
  {
    window_manager.windows[i].id = -1;
    window_manager.layouts[i] = -1;
  }
  window_manager.focus = 0;
}

int _createWindow(int x, int y, int width, int height, int toolbar_x, int toolbar_y,
  int toolbar_width, int toolbar_height, struct color24* p)
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

  int id = _createWindow(x, y, width, height, toolbar_x, toolbar_y, toolbar_width,
    toolbar_height, (struct color24*) p);

  return id;
}

int _updateWholeWindow(int id)
{
  struct window_info* wnd;
  for (int i = 0; i < Max_Window_Num; i++)
    if (window_manager.windows[i].id == id)
    {
      wnd = &window_manager.windows[i];
      break;
    }

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
