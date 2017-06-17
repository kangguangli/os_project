#ifndef GUI_FIRST_TRY
#define GUI_FIRST_TRY

extern char _binary_font_psf_start;
extern char _binary_font_psf_end;

/*
typedef struct {
    uint magic;         // magic bytes to identify PSF
    uint version;       // zero ;
    uint headersize;    // offset of bitmaps in file, 32 ;
    uint flags;         // 0 if there's no unicode table ;
    uint numglyph;      // number of glyphs ;
    uint bytesperglyph; // size of each glyph ;
    uint height;        // height in pixels ;
    uint width;         // width in pixels ;
} PSF_font;*/

#define Video_Info_Store 0x1000


struct video_info_struct
{
  unsigned short screen_height;
  unsigned short screen_width;
  struct color24* video_memory;
  struct color24* cache1;
  struct color24* cache2;
  unsigned char color_num;
};


struct color24
{
  unsigned char B;
  unsigned char G;
  unsigned char R;
};

//#define Video_Cache_Start 0xa0000
//#define Video_Cache_End   0xaffff
//
//#define Screen_Width      320
//#define Screen_Height     200


#define Char_Width 8
#define Char_Height 16


#endif
