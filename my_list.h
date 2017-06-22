#ifndef MY_LIST_H_
#define MY_LIST_H_

#define mylistSize 9

typedef struct mylist
{
	int num;
    char ch;
    struct mylist* next;
}mylist_info;

#endif
