#ifndef WINDOWS_H
#define WINDOWS_H

#define Max_Window_Num 10

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

  struct color24* hwnd;
};

struct window_manager_struct
{
  struct window_info windows[Max_Window_Num];
  int layouts[Max_Window_Num];
  struct window_info* focus;
};

#define Message_Params_Num 4
struct message
{
  //int hwnd;//window id
  int type;
  int params[Message_Params_Num];
};

typedef struct message* message_queue;



struct timer_info
{

};

#endif
