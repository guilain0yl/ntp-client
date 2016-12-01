#include<process.h>
#include"display_num.h"
#include"resource.h"
#include"reg.h"

#define TIMER_1 1
static m_time g_time;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void proc(void *);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	const TCHAR szAppName[] = TEXT("DigClock");
	WNDCLASS wndclass;
	HWND hwnd;
	MSG msg;
	int cxScreen, cyScreen;
	int nWindowWidth, nWindowHight;

	cxScreen = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度 
	cyScreen = GetSystemMetrics(SM_CYSCREEN);
	nWindowWidth = cxScreen / 8; nWindowHight = cyScreen / 8;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName, TEXT("Digital Clock"),
		WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		nWindowWidth, nWindowHight,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void GetWindowSize(HWND hwnd, int *pnWidth, int *pnHeight)
{
	RECT rc;
	GetWindowRect(hwnd, &rc);
	*pnWidth = rc.right - rc.left;
	*pnHeight = rc.bottom - rc.top;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	static int nWidth, nHeight, time = 0;
	static POINT org = { 10, 10 };
	POINT mouse_point;
	static HMENU hMenu;
	static int run= 0;

	switch (message)
	{
	case WM_CREATE:
	{
		DWORD xPos = 0, yPos = 0;

		// 设置分层属性
		SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		// 设置透明色
		COLORREF clTransparent = RGB(0, 0, 0);
		SetLayeredWindowAttributes(hwnd, clTransparent, 0, LWA_COLORKEY);

		// 得到窗口大小
		GetWindowSize(hwnd, &nWidth, &nHeight);

		//加载菜单
		hMenu = LoadMenu(((LPCREATESTRUCT)lParam)->hInstance, "m_menu");
		hMenu = GetSubMenu(hMenu, 0);

		//判断是否自启动
		if (is_run() == 1)
			run = 1;
		else run = 0;
		CheckMenuItem(hMenu, IDM_RUN, run ? MF_CHECKED : MF_UNCHECKED);

		//移动窗口
		if (get_window_point(&xPos, &yPos))
			MoveWindow(hwnd, xPos, yPos, nWidth, nHeight, FALSE);
	
		init_time(&g_time);

		SetTimer(hwnd, TIMER_1, 1000, NULL);
		return 0;
	}
	case WM_TIMER:
	{
		g_time.second++;
		time++;
		if (g_time.second >= 60)
		{
			g_time.minute++;
			if (g_time.minute >= 60)
			{
				g_time.hour++;
				if (g_time.hour >= 24)
					g_time.hour = 0;
				g_time.minute = 0;
			}
			g_time.second = 0;
			if (time == 600)
				init_time(&g_time);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
		}
		return 0;
	}
	case WM_LBUTTONDOWN: //当鼠标左键点击时可以拖曳窗口
		PostMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		return TRUE;
	case WM_RBUTTONUP:
	{
		mouse_point.x = LOWORD(lParam);
		mouse_point.y = HIWORD(lParam);
		ClientToScreen(hwnd, &mouse_point);
		TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, mouse_point.x, mouse_point.y, 0, hwnd, NULL);
		return 0;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDM_RUN:
			run = !run;
			CheckMenuItem(hMenu, IDM_RUN, run ? MF_CHECKED : MF_UNCHECKED);
			if (run)
				run_reg();
			else un_run_reg();
			return 0;
		case IDM_CLEANREG:
			del_reg();
			return 0;
		case IDM_TIME://时间同步
			_beginthread(proc, 0, hwnd);
			return 0;
		case IDM_QUIT:
			SendMessage(hwnd, WM_DESTROY, 0, 0);
			return 0;
		}
	}
	case WM_PAINT:
	{
		HDC hdc;
		hdc = BeginPaint(hwnd, &ps);
		draw_time(hdc, org, nWidth - org.x * 2, nHeight - org.y * 2, &g_time);
		EndPaint(hwnd, &ps);
	}
		return 0;
	case WM_DESTROY:
		//向注册表写入窗口信息
		set_pos(hwnd);
		KillTimer(hwnd, TIMER_1);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

void proc(void *p)
{
	HWND hwnd;
	hwnd = p;
	init_ntp_time(&g_time);
	InvalidateRect(hwnd, NULL, TRUE);
	UpdateWindow(hwnd);
}