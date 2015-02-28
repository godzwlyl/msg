
#if !defined(AFX_CMN_MBUF_H__63C4BCFB_DDE9_42CC_A67D_964D362B1AF4__INCLUDED_)
#define AFX_CMN_MBUF_H__63C4BCFB_DDE9_42CC_A67D_964D362B1AF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <stdio.h>
#include <string>


typedef void				*LPVOID;
typedef void				*PVOID;
typedef const void			*PCVOID;

typedef unsigned char guint8;
typedef signed char gint8;

typedef unsigned short guint16;
typedef signed short gint16;

typedef unsigned int guint32;
typedef signed int gint32;

typedef char   gchar;
typedef short  gshort;
typedef long   glong;
typedef int    gint;
typedef gint   gintean;

typedef unsigned char   guchar;
typedef unsigned short  gushort;
typedef unsigned long   gulong;
typedef unsigned int    guint;

typedef float   gfloat;
typedef double  gdouble;

typedef signed int gssize;
typedef unsigned int gsize;

#define CHAR_TO_HEX(ch) ((ch>='0' && ch <='9') ? (ch-'0') :( (ch>='a' && ch <='z') ? (ch-'a'+10) :( (ch>='A' && ch<='Z') ? (ch-'A'+10) : 0 )))


#define Z_CHECK_FUN(fun,val)\
  if((fun)!=0)\
{\
  fprintf(stdout,"%s:%d:%s failed.",__FILE__,__LINE__,#fun);\
  return val;\
}else{}

#define Z_CHECK_POINT(p,val)\
  if((p)==NULL)\
{\
  fprintf(stdout,"%s:%d:%s null pointer.",__FILE__,__LINE__,#p);\
  return val;\
}else{}

#define Z_SAFE_DELETE(x)\
	if(x) {delete (x);(x)=NULL;}else{}

#define Z_SAFE_DELETEVARY(x)\
	if(x) {delete [](x);(x)=NULL;}else{}

#define Z_SAFE_FREE(x)\
	if(x) {free(x);(x)=NULL;}else{}

//Bytes convert
#define Z_MAKE_GINT8(lo)\
((gint8)(lo))

#define Z_MAKE_GINT16(lo,hi)\
(gint16)((guint16)(guint8)(lo) | ((guint16)(guint8)(hi)<<8))

#define Z_MAKE_GINT32(lo,hi)\
(gint32)((guint32)(guint16)(lo) | ((guint32)(guint16)(hi)<<16))


#define  Z_WHITESPACE(x)	((x)=='\t' || (x)==' ')
#define  Z_CRLF(x)	((x)=='\n' || (x)=='\r')

#define Z_MBUF_ITEM_LEN		64
#define Z_MBUF_STR_NULL		((gint8)0)
#define Z_MBUF_STR_NOT_NULL	((gint8)1)


#define Z_MBUF_LEN	(m_end-m_start)
#define Z_MBUF_P	(m_p+m_start)
#define Z_CHAR(x,i)	(m_p[m_start+i]==x)
//

#ifdef Z_MSG_PRINT
#define Z_MSG_PRINT_LEN	512
#endif

//class SocketOperation;

class ZMBuf
{
public:
	ZMBuf();
	ZMBuf(gchar* p, gsize len);
	ZMBuf(ZMBuf& pBuf);
	virtual ~ZMBuf();

public:
	ZMBuf* Dup(void);
//	int HexPrint(const Z_SOCKADDR_IN* s1, const Z_SOCKADDR_IN* s2, gchar* info);
	int EnsureSpace(size_t len);
//	int SendTo(SocketOperation *sock_ASS);
//	int SendTo(SocketOperation *sock_ASS,unsigned long  Type);

	int PkHexString(const char* str);
	int IsXml(void);

	/*Pk Data*/
	int PkData(guint8 t);
	int PkData(gint8 t)
	{
		return PkData((guint8) t);
	}

	int PkData(guint16 t);
	int PkData(gint16 t)
	{
		return PkData((guint16) t);
	}

	int PkData(guint32 t);
	int PkData(gint32 t)
	{
		return PkData((guint32) t);
	}

	int PkData(const char* p, size_t len);
	int PkData(std::string str);
	int PkData(ZMBuf* mbuf);

	/************************************************************************/
	/* Unpack   															*/
	/************************************************************************/
	int UnpkData(guint8* t);
	int UnpkData(gint8* t)
	{
		return UnpkData((guint8 *) t);
	}

	int UnpkData(guint16* t);
	int UnpkData(gint16* t)
	{
		return UnpkData((guint16 *) t);
	}

	int UnpkData(guint32* t);
	int UnpkData(gint32* t)
	{
		return UnpkData((guint32 *) t);
	}

	int UnpkData(char** pp, size_t* len);
	int UnpkData(std::string* str);

	int Empty(void);
	gintean IsEmpty(void);

	//added by zw at 20090417
    char * Get_Mp();
    unsigned int Get_Ms();
    unsigned int Get_Me();
	unsigned int Get_Ml();
protected:
	int AppendToTail(const char* p, size_t len);
	int RemoveFromHead(char* p);
	int Optimize(void);


	char* m_p;
	size_t m_start;
	size_t m_end;
	size_t m_len;
};

#endif // !defined(AFX_CMN_MBUF_H__63C4BCFB_DDE9_42CC_A67D_964D362B1AF4__INCLUDED_)