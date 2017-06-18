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
  void (*msgproc)(struct color24*, int, int*);

  //struct window_info* next;
  int next;
};

struct window_manager_struct
{
  struct window_info windows[Max_Window_Num];
  //int layouts[Max_Window_Num];
  struct window_info* focus;
};

#define Max_Message_Num 50
#define Message_Params_Num 4
struct message
{
  //int hwnd;//window id
  unsigned char type;
  int params[Message_Params_Num];
};



struct timer_info
{

};

#endif
