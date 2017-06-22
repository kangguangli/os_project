#ifndef MOUSE
#define MOUSE

#define Mouse_Shape_Height 18
#define Mouse_Shape_Width  15

extern unsigned char
mouse_shape[Mouse_Shape_Height][Mouse_Shape_Height];

#define Port_KeyData 0x60
#define Port_KeyStatus 0x64
#define Port_KeyCmd 0x64
#define Double_Click_Intervals 60


struct mouse_dec_struct
{
  unsigned char buf[3];
  int phase;
  int delta_x;
  int delta_y;
  int button;
  unsigned int last_clcik;
  unsigned char click_flag;
};

struct mouse_info_struct
{
  int x;
  int y;
  int btn;
};

#endif
