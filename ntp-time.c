
#include"ntp-time.h"


static void Init_NTP_Package(char *data)
{
	time_t rawtime;
	time_t usec;
	SYSTEMTIME current_time = { 0 };

	memset(data, 0x0, sizeof(ntp_package)+1);

	data[0] = get_li_vn_mode(0x3, 0x3, 0x3);
	data[1] = 0x0;
	data[2] = 0x11;
	data[3] = -6;
	data[9] = 0x1;
	data[10] = 0x4;
	time(&rawtime);
	rawtime += __epoch;
	if ('l' == ENDIANNESS) sw32(rawtime);
	memcpy(&data[40], &rawtime, 4);
#ifdef _WIN32
	GetSystemTime(&current_time);
	usec = current_time.wMilliseconds;
#endif
	usec = ((time_t)usec*(time_t)__ntp_scale_frac) / (time_t)1000000;
	if (ENDIANNESS == 'l') sw32(usec);
	memcpy(&data[44], &usec, 4);
}
//每5min校时一次




static int hostname2ip(char *Target, char **IpList, int ip_count)
{
	struct hostent *pHost;
	int i = 0;

	pHost = gethostbyname(Target);
	if (pHost != NULL)
	{
		while (*pHost->h_addr_list != NULL)
		{
			IpList[i] = inet_ntoa(*(struct in_addr *)*pHost->h_addr_list);
			*pHost->h_addr_list++;
			i++;
			if (i > ip_count) return 1;
		}
		return 1;
	}
	else return 0;
}

//NTP协议 time.windows.com

static int recv_ntp_time(char *ip, p_ntp_package p_ntp)
{
	char *data = NULL;
	struct sockaddr_in server_info, local_info;
	int ret, len;
	sock server_sk;

	server_sk = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (server_sk == INVALID_SOCKET) return -1;

	//初始化服务器信息
	server_info.sin_addr.s_addr = inet_addr(ip);
	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(NTP_PORT);
	memset(server_info.sin_zero, 0x0, 8);
	//
	local_info.sin_addr.S_un.S_addr = NULL;
	local_info.sin_family = AF_INET;
	local_info.sin_port = htons(NTP_PORT);
	memset(local_info.sin_zero, 0x00, 8);
	//
	len = sizeof(struct sockaddr_in);
	//

	ret = bind(server_sk, (struct sockaddr*)&local_info, len);
	//打包NTP结构
	data = (char*)malloc(sizeof(ntp_package)+1);
	Init_NTP_Package(data);
	//发送
	if (sendto(server_sk, data, sizeof(ntp_package), 0, (struct sockaddr*)&server_info, len) <= 0)
		return -1;
	//接受
	memset(data, 0x0, sizeof(ntp_package)+1);
	Sleep(100);
	ret = recv(server_sk, data, sizeof(ntp_package), 0);
	//解析数据包 数据包没完全解析
	memcpy(&p_ntp->receive_timestamp, &data[32], 8);
	free(data);
	WSACleanup();
}

int get_unix_time_from_ntp(char *ip_addr)
{
	char *ip_list[10];
	ntp_package ntp = { NULL };
	WSADATA ws;
	unsigned int t = 0;

	//初始化winsock
	if (WSAStartup(MAKEWORD(2, 2), &ws))
	{
		//printf("wsastartip error\n");
		return -1;
	}

	if (!hostname2ip(ip_addr, ip_list, sizeof(ip_list) / sizeof(char*)))
		return -1;

	recv_ntp_time(ip_list[0], &ntp);
	if (ENDIANNESS == 'l')
	{
		t = (ntp.receive_timestamp & 0xffffffff);
		sw32(t);
		t -= __epoch;
	}
	if (ENDIANNESS == 'b')
		t = ntp.transmit_timestamp >> 32;

	return t;

}