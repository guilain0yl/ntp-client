#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#ifndef _DISPLAY_
#define _DISPLAY_
#include<math.h>
#include<Windows.h>
#define angle2rad(x) (x*M_PI/180)
#define m_tan(x) tan(angle2rad(x))

typedef struct TIME
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
}m_time, *p_m_time;

void draw_time(HDC hdc, POINT org, int x, int y, p_m_time time);
void init_time(p_m_time _time);
void init_ntp_time(p_m_time _time);

#endif