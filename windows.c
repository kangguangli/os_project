#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

#include "windows.h"
#include "gui.h"
#include "message.h"
#include "mouse.h"


struct window_manager_struct window_manager;
struct message message_queue[Max_Message_Num];
struct timer_manager_struct timer_manager;

void deviceMessageProc();
int msgPut(struct window_info* wnd, struct message data);
int msgGet(struct window_info* wnd, struct message* p);
void messageHandle(struct message msg);
void windowsManagerInit();
int _createWindow(int x, int y, int width, int height, int toolbar_x, int toolbar_y,
  int toolbar_width, int toolbar_height, struct color24* p, int pid);
int _updateWholeWindow(int id);
void _setTimer(int window_id, int id, int intervals);
//struct message _getMessage(int window_id, struct message* msg);

void deviceMessageProc()
{
  uint data = fifoGet(&device_buf);
  if (data >= Mouse_Offset)
  {
    //cprintf("\n mouse: %x\n", data);
    mouseHandle(timer_manager.ticks, data - Mouse_Offset);
  }
  else if (data >= Keyboard_Offset)
  {
    //cprintf("\n kbd: %x\n", data);
    kbdHandle(data - Keyboard_Offset);
  }
  else
    ;
}

int msgPut(struct window_info* wnd, struct message data)
{
  if (wnd->msg_num == Max_Message_Num_Single)
  {
    return -1;
  }

  wnd->msg_queue[wnd->msg_p] = data;
  wnd->msg_p++;
  if (wnd->msg_p == Max_Message_Num_Single)
    wnd->msg_p = 0;
  wnd->msg_num++;

  return 0;
}

int msgGet(struct window_info* wnd, struct message* p)
{
  if (wnd->msg_num == 0)
    return -1;

  struct message data = wnd->msg_queue[wnd->msg_q];
  wnd->msg_q++;
  if (wnd->msg_q == Max_Message_Num_Single)
    wnd->msg_q = 0;
  wnd->msg_num--;

  p->type = data.type;
  for (int i = 0; i < Message_Params_Num; i++)
    p->params[i] = data.params[i];

  return 0;
}

void messageHandle(struct message msg)
{
  // struct window_info* p = 0;
  // int cursor_x, cursor_y;
  switch (msg.type)
  {
    case WM_TIMER:
    timer_manager.ticks = msg.params[0];
      for (int i = 0; i < Max_Timer_Num; i++)
      {
        if (timer_manager.timers[i].timer_id != -1)
        {
          timer_manager.timers[i].last_count += 10;
          if (timer_manager.timers[i].last_count
            % timer_manager.timers[i].intervals == 0)
          {
            timer_manager.timers[i].last_count = 0;
            int id = timer_manager.timers[i].window_id;
            struct message _msg;
            _msg.type = WM_TIMER;
            _msg.params[0] = timer_manager.timers[i].timer_id;
            msgPut(&window_manager.windows[id], _msg);
          }
        }
      }
     break;
    case WM_KEYDOWN:
      if (window_manager.focus != 0)
        msgPut(window_manager.focus, msg);
      break;
    case WM_LBTNDOWN:
      // To do 判断窗口列表中哪一个是焦点所在
      // p = window_manager.focus;
      // cursor_x = msg.params[0];
      // cursor_y = msg.params[1];
      // while (p != 0)
      // {
      //   if (cursor_x < p->x || cursor_y < p->y ||
      //       cursor_x > p->x + p->width || cursor_y > p->y + p->height)
      //   {
      //     if (p->next != -1)
      //       p = &window_manager.windows[p->next];
      //   }
      // }



      if (window_manager.focus != 0)
        msgPut(window_manager.focus, msg);
      break;
    case WM_RBTNDOWN:
      if (window_manager.focus != 0)
        msgPut(window_manager.focus, msg);
      break;
    case WM_MBTNDOWN:
      if (window_manager.focus != 0)
        msgPut(window_manager.focus, msg);
      break;
    case WM_LDOUBLE:

      if (window_manager.focus != 0)
        msgPut(window_manager.focus, msg);
      break;
  }
  //cprintf("\n proc: %x\n", window_manager.focus->msgproc);

    //window_manager.focus->msgproc(window_manager.focus->hwnd, msg.type, msg.params);
}

void windowsManagerInit()
{
  for (int i = 0; i < Max_Window_Num; i++)
  {
    window_manager.windows[i].id = -1;
    window_manager.windows[i].next = -1;
    //window_manager.layouts[i] = -1;
  }
  window_manager.focus = 0;

  for (int i = 0; i < Max_Message_Num; i++)
    message_queue[i].type = -1;

  for (int i = 0; i < Max_Timer_Num; i++)
  {
    timer_manager.timers[i].timer_id = -1;
  }
}

void updateAllWindow()
{
  struct message msg;
  msg.type = WM_PAINT;
  for (int i = 0; i < Max_Window_Num; i++)
  {
    cprintf("\n paint, %d\n", window_manager.windows[i].id);
    if (window_manager.windows[i].id != -1)
    {
      //cprintf("\n paint, %d\n", i);
      msgPut(&window_manager.windows[i], msg);
    }
  }
}


int _createWindow(int x, int y, int width, int height, int toolbar_x, int toolbar_y,
  int toolbar_width, int toolbar_height, struct color24* p, int pid)
  //void (*msgproc)(struct color24*, int, int*))
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
  //window_manager.windows[id].msg_queue = msg_queue;
  window_manager.windows[id].pid = pid;
  //window_manager.windows[id].msgproc = msgproc;
  int last = 0;
  if (window_manager.focus != 0)
    last = window_manager.focus->id;
  window_manager.focus = &window_manager.windows[id];
  window_manager.focus->next = last;
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
  //int pid;
  //argint(9, &pid);

  int id = _createWindow(x, y, width, height, toolbar_x, toolbar_y, toolbar_width,
    toolbar_height, (struct color24*) p, proc->pid);
  return id;
}

extern struct mouse_info_struct mouse_info;
int _updateWholeWindow(int id)
{
  struct window_info* wnd = &window_manager.windows[id];

  drawContentToContent(wnd->hwnd, video_info.cache2, 0, 0, wnd->width,
    wnd->height, wnd->x, wnd->y, video_info.screen_width, video_info.screen_height);
  drawContentToContent(wnd->hwnd, video_info.video_memory, 0, 0, wnd->width,
    wnd->height, wnd->x, wnd->y, video_info.screen_width, video_info.screen_height);
  drawCursor(video_info.video_memory, mouse_info.x, mouse_info.y);

  return 0;
}

int sys_updateWholeWindow()
{
 int id;
 argint(0, &id);

 _updateWholeWindow(id);

 return 0;
}

void _setTimer(int window_id, int id, int intervals)
{
  for (int i = 0; i < Max_Timer_Num; i++)
  {
    if (timer_manager.timers[i].timer_id == -1)
    {
      timer_manager.timers[i].timer_id = id;
      timer_manager.timers[i].window_id = window_id;
      timer_manager.timers[i].intervals = intervals;
      timer_manager.timers[i].last_count = 0;
      return;
    }
  }
}

int sys_setTimer()
{
  int window_id;
  argint(0, &window_id);

  int id;
  argint(1, &id);

  int intervals;
  argint(2, &intervals);

  _setTimer(window_id, id, intervals);

  return 0;
}

int sys_getMessage()
{
  int id;
  argint(0, &id);
  int p;
  argint(1, &p);
  struct message* msg = (struct message*)p;

  return msgGet(&window_manager.windows[id], msg);

}

int sys_destroyWindow()
{
  int id;
  argint(0, &id);

  cprintf("\n id, %d\n", id);
  window_manager.windows[id].id = -1;
  if (window_manager.focus == &window_manager.windows[id])
  {
    window_manager.focus = &window_manager.windows[window_manager.focus->next];
  }
  updateAllWindow();
  return 0;

}
