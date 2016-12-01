
#ifndef _NTP_
#define _NTP_

#include<WinSock2.h>
#include<WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib")
typedef SOCKET sock;

typedef unsigned int u_32;
typedef unsigned long long u_64;

typedef struct NTP_PACKAGE
{
	char LI_VN_MODE; //need
	char Stratum;
	char Poll; //need
	char Precision; //need
	u_32 root_delay; //4
	u_32 root_dispersion; //8
	u_32 reference_identifier;  //12
	u_64 reference_timestamp;  //16
	u_64 originate_timestamp;  //24
	u_64 receive_timestamp;  //32
	u_64 transmit_timestamp;  //40  //need
	//char authenticator[12];  //48
}ntp_package, *p_ntp_package;

#define get_li_vn_mode(li,vn,mode) (((li&0x3)<<6)|((vn&0x7)<<3)|(mode&0x7))

#define NTP_PORT 123

#endif

static const unsigned long __epoch = 2208988800UL;
static const double __ntp_scale_frac = 4294967295.0;

//ÅÐ¶Ï´óÐ¡¶Ë
static union{ char c[4]; unsigned int l; }endian_test = { { 'l', '?', '?', 'b' } };
#define ENDIANNESS ((char)endian_test.l)

#define sw32(x) \
	(x = ((x << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8) | (x >> 24)))
