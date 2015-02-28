#include <string.h>
#include <time.h>
#include <assert.h>
#include <iostream>
#include <io.h>

#ifdef WIN32
#define getsockerror() WSAGetLastError()
#include <direct.h>
#include <stdarg.h>
#include <WinSock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/cdefs.h>
//#include <fcntl.h>
#include <unistd.h>
//#define closesocket close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#include <errno.h>
#define getsockerror() (errno)
#endif
#include <fcntl.h>
#pragma   warning(disable:4996)
using namespace std;

typedef unsigned char       BYTE;
typedef unsigned int        size_t;
typedef const char			*LPCSTR;
typedef const void			*PCVOID;
#define Z_CHECK_FUN(fun,val)\
	if((fun)!=0){fprintf(stdout,"%s:%d:%s failed.",__FILE__,__LINE__,#fun);return val;}else{}
#define Z_CHECK_POINT(p,val)\
	if((p)==NULL){fprintf(stdout,"%s:%d:%s null pointer.",__FILE__,__LINE__,#p);return val;}else{}
#define Z_SAFE_DELETE(x)\
	if(x){delete (x);(x)=NULL;}else{}
#define Z_SAFE_DELETEVARY(x)\
	if(x){delete [](x);(x)=NULL;}else{}
#define Z_SAFE_FREE(x)\
	if(x){free(x);(x)=NULL;}else{}

#define STREQ(str1, str2) (strcmp((str1), (str2)) == 0)
#define n_STREQ(str1, str2,n) (strncmp((str1), (str2),(n)) == 0)
#define STRNQ(str1, str2) (strcmp((str1), (str2)) != 0)
#define n_STRNQ(str1, str2,n) (strncmp((str1), (str2),(n)) != 0)
#define STRED(str1, str2) (strcmp((str1), (str2)) > 0)
#define STREX(str1, str2) (strcmp((str1), (str2)) < 0)
#define STREDE(str1, str2) (strcmp((str1), (str2)) >= 0)
#define STREXE(str1, str2) (strcmp((str1), (str2)) <= 0)

#define STREMPTY(str1) (strlen(str1) == 0)
#define STRNEMPTY(str1) (strlen(str1) != 0)
#define ZERO(str1) (memset((str1),0,(sizeof(str1))))

#define delZERO(str1,str2) \
	ZERO(str1);	\
	strcpy(str1,str2); \
	ltrzero(str1);

#define delAT(str1,str2) \
	ZERO(str1);	\
	strcpy(str1,str2); \
	ltrAt(str1);

class Operation_Base
{
public:
	Operation_Base()
	{
		m_dwErrorCode = 0;
		m_dwOwnerId = 0;
	}
	virtual ~Operation_Base()
	{
		m_dwErrorCode = 0;
		m_dwOwnerId = 0;
	}

public:
	void SetError(unsigned long  dwErrorCode) { m_dwErrorCode = dwErrorCode; }
	void SetOwner(unsigned long  dwOwnerId) { m_dwOwnerId = dwOwnerId; }
	unsigned long  GetLastError() const { return m_dwErrorCode; }
	unsigned long  GetOwnerId() const { return m_dwOwnerId; }

protected:
	unsigned long 	m_dwErrorCode;
	unsigned long 	m_dwOwnerId;
	unsigned long 	m_dwReserve;

};
class TimeOperation //ʱ�������
{
public:
	inline int  GetIntValue(  char * s , int n );//�������ǰnλת��Ϊ���֣�������	
	int GetDuration(char *cStartTime , char *cEndTime);//��������ʱ��֮����������	
    int GetCallEndTime( char * startday ,  char * starttime,int duration, char * endday , char * endtime);//������ʼ����,ʱ��,ʱ��,ȡ�ý�������ʱ��
	int GetSegmentTime( char * Sdate,  char * Stime ,int Duration ,char * SpotDate ,char * SpotTime ,int  Leap ,int * dt1 ,int * dt2);//�����Խ�涨ʱ���֮ǰ��֮��ָ�ʱ��
	int GetPointDurtn(string strStartDate,string strStartTime,int iCallDurtn,int iUnitLen,string strPointTime1,string strPointTime2,int *iDurtn1,int *iDurtn2,int iFreeDurtn);//����ֱ�����ʱ��æʱ��ʱ��	
	void GetDateTimeString(string& strTime);//��ȡ��ǰ����%Y-%m-%d %H:%M:%S���ַ�����ʽ
	char *TimeToString(const time_t * pstTime, char *pcBuffer);//��time_tʱ��ת��Ϊ����%Y-%m-%d %H:%M:%S���ַ�����ʽ��ʱ��
	int CompareTime(char *cATime,char *cBTime);//�Ƚ�����ʱ��Ĵ�С
	int GetCallWeekDay(char * cDay , int *iWeekDay);//��������ȡ�����Ӧ��������
    void GetCurrentMonthEnd(char *TME);//����ȡ�����µ�
	void GetCurrentMonthStart(char *TME);//��ȡ�����³�
    void GetNextMonth(char *TME);//��ȡ�����³�
    string ConvertStringFormatDate( char *in_date );//���ַ�����ʽ������ʱ��yyyymmddhh24miss ת����Ϊstring��ʽ
	time_t ConvertStringDate( char *in_date );//���ַ�����ʽ������ʱ��yyyymmddhh24miss ת����Ϊtime_t��ʽ
	int  GetDayBetweenDate( char * pStartdate, char * pEnddate );////��������������������
	int isOddMonth( char * pIndate );//�ж����ڵ���ż��(false:����,true:ż��)��1.5�޸�
	int nIsTime( char * str );//�ж�ʱ���Ƿ�Ϸ�	
	long GetInterval(char* End, char* Start);//�õ�ʱ��������λ�룬ʱ���ʽYYYYMMDDHHMMSS


};
class StringOpearation //�ַ���������
{
	int  GetIntValue(  char * s , int n );//�������ǰnλת��Ϊ���֣�������	
	void StringToHex(const string &strSource, BYTE* pBuffer, size_t nBytes);//�ַ�����ʮ�����ƻ�ת	
	string HexToString(const BYTE* pBuffer, size_t iBytes);
	void RemoveStrBlanks(string& strOuter);//ȥ���ַ����еĿո�	
	string GetStringStem(const string & strOrg);
	void DelSpace(char * str);//ɾ���ַ���β���Ŀո�
	void  rtrim(char *str );//ȥ���ַ����ұߵĿո�
	void ltrim(char * str);//ȥ���ַ�����ߵĿո�
    void trim(char * str);//ȥ���ַ������ߵĿո�
	void rpad( char * src,int len,char c );//���Ŀ���ַ�������С��len������Ŀ���ַ�������Ϊlen��β�����ַ�c���
	void StrupWord(char * lwrstr);//Сд��ĸ���д��ĸ
	void StrlWord(char * lwrstr);//��д��ĸ��Сд��ĸ
	void strlmv(char * src,int n);//��ĸ������
	void strnvl(char * src,char * content);//�����ַ�����ֵ
};
class FileOperation //�ļ�������
{
    void LOG_FILE(void *pFile, LPCSTR strMsg, ...);//д��־�ļ�
    int lockfile( char * pfile );//�����ļ�
	int unlockfile( char * pfile );
    int  fgetline(const string FileName);//�õ��ļ�����

};
enum PGM_SOCKET_TYPE
{
	PGM_SOCKET_UNKNOWN, 
	PGM_SOCKET_TCP, 
	PGM_SOCKET_UDP, 
	PGM_SOCKET_ICMP
};
union PGM_IP_ADDRESS
{
	unsigned long   dwIP;
	BYTE	ayIP[4];
};
enum PGM_ERROR_CODE
{
	ERR_PGM_OK = 0x00000000, 
	ERR_PGM_BASE = 21000, 

	ERR_PGM_PARAMETER,
	ERR_PGM_NO_SUPPORT,
	ERR_PGM_HANDLE,
	ERR_PGM_NO_INIT,

	// .....
	ERROR_PGM_LAST = 21100
};
class SocketOperation:public Operation_Base
{
public:
	SocketOperation(unsigned long  dwSocketType);
	virtual ~SocketOperation();

public:
	int Attach(unsigned long  dwSocket);
	int Detach();

	int CreateSocket(unsigned long  dwSocketType);
	int Bind(unsigned long  dwLocalIP = -1, unsigned long  dwLocalPort = -1);
	void SetRemote(unsigned long  dwIP = -1, unsigned long  dwPort = -1);
	int Listen();
	int Accept(SocketOperation *poClientSocket);
	int Connect(unsigned long  dwRemoteIP = -1, unsigned long  dwRemotePort = -1);
	int DisConnect();

	//unsigned long  RecvData(IPGMVar *poVar);
	unsigned long  SendData(PCVOID pBuf, unsigned long  dwLen);
	unsigned long  SendData(PCVOID pBuf, unsigned long  dwLen, unsigned long  Type );

public:
	void SetTimeOut(unsigned long  dwTimeOut) { m_dwTimeOut = dwTimeOut; }
	unsigned long  GetTimeOut() const { return m_dwTimeOut; }
	int IsDataSocket() const { return m_bDataSocket; }

public:
	unsigned long  GetLocalIp() const { return m_dwLocalIP; }
	unsigned long  GetLocalPort() const { return m_dwLocalPort; }
	unsigned long  GetRemoteIp() const { return m_dwRemoteIP; }
	unsigned long  GetRemotePort() const { return m_dwRemotePort; }
	unsigned long  GetSocketType() const { return m_dwSocketType; }
	void SetLocal(unsigned long  dwIP, unsigned long  dwPort)
	{
		m_dwLocalIP = dwIP;
		m_dwLocalPort = dwPort;
	}
	unsigned long  GetSocket() { return m_dwSocket; }
	//added by zw at 20090310
	char * GetRemoteIpAndPort(); 

protected:
	unsigned long  m_dwLocalIP;
	unsigned long  m_dwLocalPort;
	unsigned long  m_dwRemoteIP;
	unsigned long  m_dwRemoteIPMask;
	unsigned long  m_dwRemotePort;
	unsigned long  m_dwRemoteHAIP;  //
	unsigned long  m_dwRemoteHAPort;

protected:
	unsigned long 	m_dwSocketId;
	unsigned long 	m_dwSocket;
	unsigned long 	m_dwSocketType;
	unsigned long 	m_dwTimeOut;
	int	m_bDataSocket; // TRUE for UDP, TCP connect, TCP accept socket
	int	m_bBindRemote;
	int	m_bConnect;
	int		m_iLastFrameDataPos;

protected:

	unsigned long  sm_dwSocketIDSerialNo;
};