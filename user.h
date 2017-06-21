struct stat;
struct rtcdate;
struct color24;
struct message;
struct bitmap_file_struct;

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(char*, int);
int mknod(char*, short, short);
int unlink(char*);
int fstat(int fd, struct stat*);
int link(char*, char*);
int mkdir(char*);
int chdir(char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);

int createWindow(int x, int y, int width, int height, int toolbar_x, int toolbar_y,
  int toolbar_width, int toolbar_height, struct color24* p);
int updateWholeWindow(int id);
int setTimer(int window_id, int id, int intervals);
int getMessage(int id, struct message* p);

// ulib.c
int stat(char*, struct stat*);
char* strcpy(char*, char*);
void *memmove(void*, void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void printf(int, char*, ...);
char* gets(char*, int max);
uint strlen(char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);

//bitmap.c
void readBitmap24(char* img_name, struct bitmap_file_struct* img);
