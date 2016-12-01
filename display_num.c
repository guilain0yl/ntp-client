#include"display_num.h"
#include"ntp-time-out.h"

static void calc_v_point(POINT org, int length, POINT result[])
{
	int width = length / 5;
	int high;

	high = (width / 2)*m_tan(60);

	if (org.x < (width / 2)) org.x = width / 2;
	result[0].x = org.x;
	result[0].y = org.y;
	//
	result[1].x = org.x - (width / 2);
	result[1].y = org.y + high;
	result[2].x = result[1].x;
	result[2].y = result[1].y + length;
	result[3].x = org.x;
	result[3].y = result[2].y + high;
	result[4].x = result[2].x + width;
	result[4].y = result[2].y;
	result[5].x = result[1].x + width;
	result[5].y = result[1].y;
}


static void calc_h_point(POINT org, int length, POINT result[])
{
	int width = length / 5;

	int v;

	v = (width / 2)*m_tan(60);

	if (org.y < (width / 2)) org.y = width / 2;
	result[0].x = org.x;
	result[0].y = org.y;
	result[1].x = org.x + v;
	result[1].y = org.y + width / 2;
	result[2].x = result[1].x + length;
	result[2].y = result[1].y;
	result[3].x = result[2].x + v;
	result[3].y = org.y;
	result[4].x = result[2].x;
	result[4].y = result[2].y - width;
	result[5].x = result[1].x;
	result[5].y = result[1].y - width;
}

static void copy_point(POINT *dst, POINT *src, int count)
{
	int i = 0;
	for (i; i<count; i++)
	{
		dst[i].x = src[i].x;
		dst[i].y = src[i].y;
	}
}

static void _display_num(HDC hdc, POINT org, int length, char num)
{
	POINT re_h[6], re_v[6], num_point[7][6];
	int i = 0;

	calc_h_point(org, length, re_h);
	calc_v_point(org, length, re_v);//垂直
	copy_point(num_point[0], re_h, 6);
	copy_point(num_point[1], re_v, 6);
	calc_v_point(num_point[0][3], length, re_v);
	copy_point(num_point[2], re_v, 6);
	calc_v_point(num_point[2][3], length, re_v);
	copy_point(num_point[5], re_v, 6);
	calc_v_point(num_point[1][3], length, re_v);
	calc_h_point(num_point[1][3], length, re_h);
	copy_point(num_point[3], re_h, 6);
	copy_point(num_point[4], re_v, 6);
	calc_h_point(num_point[4][3], length, re_h);
	copy_point(num_point[6], re_h, 6);

	for (i; i<7; i++)
	{
		if ((num >> i) & 0x1) Polygon(hdc, num_point[i], 6);
	}
}

static void display_num(HDC hdc, POINT org, int length, int num)
{
	const char num_c[] = { 0x77, 0x24, 0x5D, 0x6D, 0x2E, 0x6B, 0x7B, 0x25, 0x7F, 0x6F };
	POINT p;
	int width = length / 5;

	if (num>9) return;

	p.x = org.x + ((width / 2)*m_tan(60));
	p.y = org.y + ((width / 2)*m_tan(60));
	_display_num(hdc, p, length, num_c[num]);
}


static void time_pot(HDC hdc, POINT f_org, POINT s_org, int diameter)
{
	Ellipse(hdc, f_org.x, f_org.y, f_org.x + diameter, f_org.y + diameter);
	Ellipse(hdc, s_org.x, s_org.y, s_org.x + diameter, s_org.y + diameter);
}

void draw_time(HDC hdc, POINT org, int x, int y, p_m_time time)
{
	POINT f_org, s_org, pot;
	HBRUSH hb;
	int num_length, num_high, num_width, pot_diameter;

	//计算数字的长度以及位置 
	num_width = (x / 14) * 2;
	num_length = num_width / (1 / 5 + 1 + ((1 / 5)*m_tan(60)));
	num_high = (((2 / 5)*m_tan(60)) + 2)*num_length;
	if (y <= num_high) y = num_high + 20;
	PatBlt(hdc, 0, 0, x, y, BLACKNESS);

	hb = CreateSolidBrush(RGB(0, 255, 0));
	SelectObject(hdc, hb);

	//时针
	f_org.y = (y - num_high) / 4;
	f_org.x = num_width / 3;
	display_num(hdc, f_org, num_length, time->hour / 10);
	f_org.x += (num_width + num_width / 2) + 10;
	display_num(hdc, f_org, num_length, time->hour % 10);
	//pot
	pot_diameter = num_width / 3;
	pot.x = f_org.x + num_width + (num_width - pot_diameter);
	pot.y = f_org.y + num_high / 3;
	s_org.x = pot.x;
	s_org.y = pot.y + num_high / 2;
	time_pot(hdc, pot, s_org, pot_diameter);
	//分针
	f_org.x += (num_width * 2);
	display_num(hdc, f_org, num_length, time->minute / 10);
	f_org.x += (num_width + num_width / 2) + 10;
	display_num(hdc, f_org, num_length, time->minute % 10);

	DeleteObject(hb);
}


void init_time(p_m_time _time)
{
	SYSTEMTIME lst;
	GetLocalTime(&lst);
	_time->year = lst.wYear;
	_time->month = lst.wMonth;
	_time->day = lst.wDay;
	_time->hour = lst.wHour;
	_time->minute = lst.wMinute;
	_time->second = lst.wSecond;
}

void init_ntp_time(p_m_time _time)
{
	struct tm tm;
	time_t t;

	t = get_unix_time_from_ntp("time.windows.com");
	tm = *localtime(&t);

	//_time->year = tm.tm_year;//
	_time->month = tm.tm_mon;
	_time->day = tm.tm_mday;
	_time->hour = tm.tm_hour;
	_time->minute = tm.tm_min;
	_time->second = tm.tm_sec;
}