#include<Windows.h>

#define MAKEDWORD(x,y) \
	((DWORD)(((DWORD)x << 16) & 0xFFFF0000) | (DWORD)y & 0xFFFF)



int set_pos(HWND hwnd);

int get_window_point(WORD *x, WORD *y);

int del_reg();

int un_run_reg();

int is_run();

int run_reg();