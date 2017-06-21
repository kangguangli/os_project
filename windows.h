#ifndef WINDOWS_H
#define WINDOWS_H

#define Max_Window_Num 10

struct window_info;
struct window_manager_struct;
struct message;
struct timer_info;
struct timer_manager;

#define Max_Message_Num 50
#define Message_Params_Num 4
struct message
{
  //int hwnd;//window id
  unsigned char type;
  int params[Message_Params_Num];
};

#define Max_Message_Num_Single 5
struct window_info
{
  int x;
  int y;
  int width;
  int height;

  int toolbar_x;
  int toolbar_y;
  int toolbar_width;
  int toolbar_height;

  int id;//pos in the windows list
  int pid;

  struct color24* hwnd;
  struct message msg_queue[Max_Message_Num_Single];
  int msg_p;
  int msg_q;
  int msg_num;
  //void (*msgproc)(struct color24*, int, int*);

  //struct window_info* next;
  int next;
};

struct window_manager_struct
{
  struct window_info windows[Max_Window_Num];
  //int layouts[Max_Window_Num];
  struct window_info* focus;
};

struct timer_info
{
  unsigned int timer_id;
  unsigned int window_id;
  unsigned int intervals;
  unsigned int last_count;
};

#define Max_Timer_Num 10
struct timer_manager_struct
{
  unsigned int ticks;
  struct timer_info timers[Max_Timer_Num];
};

#endif
