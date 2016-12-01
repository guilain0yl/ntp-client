#include"reg.h"


static int set_window_point(WORD x, WORD y)
{
	HKEY hKEY;
	DWORD pos;

	pos=MAKEDWORD(x, y);
	if (RegCreateKeyEx(HKEY_CURRENT_USER, "ALARM_CLOCK", 0, "REG_DWORD", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKEY, NULL) != ERROR_SUCCESS)
		return 0;
	if (RegSetValueEx(hKEY, "clock_pos", 0, REG_DWORD, &pos, sizeof(DWORD)) != ERROR_SUCCESS)
		return 0;
	RegCloseKey(hKEY);
	return 1;
}

int set_pos(HWND hwnd)
{
	RECT client;
	if (GetWindowRect(hwnd, &client) == 0)
		return 0;
	if (set_window_point(client.left, client.top) == 1)
		return 1;
	return 0;
}

int get_window_point(WORD *x, WORD *y)
{
	DWORD pos, len, type;
	HKEY hKEY;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, "ALARM_CLOCK", 0, KEY_ALL_ACCESS, &hKEY) != ERROR_SUCCESS)
		return 0;
	if (RegQueryValueEx(hKEY, "clock_pos", 0, &type, &pos, &len) != ERROR_SUCCESS)
		return 0;
	*x = HIWORD(pos);
	*y = LOWORD(pos);

	RegCloseKey(hKEY);
	return 1;
}

int del_reg()
{
	if (RegDeleteKey(HKEY_CURRENT_USER, "ALARM_CLOCK") != ERROR_SUCCESS)
		return 0;

	return 1;
}

int un_run_reg()
{
	HKEY hKEY;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKEY) != ERROR_SUCCESS)
		return 0;
	if (RegDeleteValue(hKEY, "clock") != ERROR_SUCCESS)
		return 0;
	RegCloseKey(hKEY);

	return 1;
}

int is_run()
{
	HKEY hKEY;
	DWORD type,len;
	TCHAR buf[MAX_PATH];

	if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKEY) != ERROR_SUCCESS)
		return 0;
	if (RegQueryValueEx(hKEY, "clock", 0, &type, &buf, &len) != ERROR_SUCCESS)
		return 0;
	RegCloseKey(hKEY);

	return 1;
}

int run_reg()
{
	HKEY hKEY;
	TCHAR szPath[MAX_PATH];

	if (!GetModuleFileName(NULL, szPath, MAX_PATH))
		return 0;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKEY) != ERROR_SUCCESS)
		return 0;
	if (RegSetValueEx(hKEY, "clock", 0, REG_SZ, szPath, strlen(szPath)) != ERROR_SUCCESS)
		return 0;
	RegCloseKey(hKEY);

	return 1;
}