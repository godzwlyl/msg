// ***************************************************************
//  MBuf   version:  1.0   ? date: 02/14/2007
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
// liuhuisong@zgwri.com
// ***************************************************************
// cmn_mbuf.cpp: implementation of the MBuf class.
//
///////////////////////////////////////////////////////////////////////
#include <string.h>
#include "buf.h"
/*#include "Gen.h"*/
/*#include "MBuf.h"*/
/*#include "Utility.h"*/

/*#include <glib/gprintf.h>*/

#ifdef _MEMCHECK
#include "MemoryManager.h"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZMBuf::ZMBuf(gchar* p, gsize len)
{
	m_end = 0;
	m_start = 0;
	m_p = NULL;
	m_len = 0;

	if (p)
	{
		//m_p=new char(len);
		m_p = new char[len];//(char *) malloc(len);
		memcpy(m_p,p,len);

 		m_len = len;
		m_end = len;
	}
}

ZMBuf::ZMBuf(ZMBuf& pBuf)
{
	if (pBuf.m_p && pBuf.m_end - pBuf.m_start > 0)
	{
		//m_p = (char *) malloc(pBuf.m_end - pBuf.m_start);
		m_p = new char[pBuf.m_end - pBuf.m_start];
		//m_p=new char(pBuf.m_end - pBuf.m_start);
		memcpy(m_p, pBuf.m_p + pBuf.m_start, pBuf.m_end - pBuf.m_start);
		m_end = pBuf.m_end - pBuf.m_start;
		m_start = 0;
		m_len = pBuf.m_end - pBuf.m_start;
	}
	else
	{
		m_end = 0;
		m_start = 0;
		m_p = NULL;
		m_len = 0;
	}
}

ZMBuf::ZMBuf()
{
	m_end = 0;
	m_start = 0;
	m_p = NULL;
	m_len = 0;
}

ZMBuf::~ZMBuf()
{
	Z_SAFE_DELETEVARY(m_p);
	//Z_SAFE_DELETE(m_p);
	m_end = 0;
	m_start = 0;
	m_len = 0;
}

int ZMBuf::PkData(guint8 t)
{
	Z_CHECK_FUN(EnsureSpace(1), -1);


	m_p[m_end] = t;
	m_end++;


	return 0;
}

int ZMBuf::PkData(guint16 t)
{
	gint8 hi;
	gint8 lo;

	hi = t >> 8;
	lo = t >> 0;


	Z_CHECK_FUN(PkData(lo), -2);
	Z_CHECK_FUN(PkData(hi), -1);


	return 0;
}

int ZMBuf::PkData(guint32 t)
{
	gint16 hi;
	gint16 lo;

	hi = t >> 16;
	lo = t >> 0;


	Z_CHECK_FUN(PkData(lo), -2);
	Z_CHECK_FUN(PkData(hi), -1);


	return 0;
}

int ZMBuf::UnpkData(guint8* t)
{
	return RemoveFromHead((char *) t);
}

int ZMBuf::UnpkData(guint16* t)
{
	gint8 hi = 0;
	gint8 lo = 0;

	Z_CHECK_POINT(t, -1);

	Z_CHECK_FUN(UnpkData(&lo), -3);
	Z_CHECK_FUN(UnpkData(&hi), -2);

	*t = Z_MAKE_GINT16(lo, hi);

	return 0;
}

int ZMBuf::UnpkData(guint32* t)
{
	gint16 hi = 0;
	gint16 lo = 0;

	Z_CHECK_POINT(t, -1);

	Z_CHECK_FUN(UnpkData(&lo), -3);
	Z_CHECK_FUN(UnpkData(&hi), -2);

	*t = Z_MAKE_GINT32(lo, hi);

	return 0;
}

int ZMBuf::PkData(const char* p, size_t len)
{
	if (p == NULL)
	{
		Z_CHECK_FUN(PkData((gint8) Z_MBUF_STR_NULL), -1);
		return 0;
	}

	Z_CHECK_FUN(PkData((gint8) Z_MBUF_STR_NOT_NULL), -1);

	Z_CHECK_FUN(PkData((gint16) len), -1);
	if (len > 0)
	{
		if (EnsureSpace(len) != 0)
		{
//			G_BREAKPOINT();
			m_end -= 2;
			return -3;
		}

		if (AppendToTail(p, len) != 0)
		{
//			G_BREAKPOINT();
			m_end -= 2;
			return -4;
		}
	}

	return 0;
}

int ZMBuf::PkData(std::string str)
{
	const char* p;

	p = str.c_str();

	return PkData(p, strlen(p));
}


int ZMBuf::PkData(ZMBuf* mbuf)
{
	Z_CHECK_POINT(mbuf, -1);

	if (mbuf->IsEmpty())
	{
		return 0;
	}

	if (mbuf->m_end < mbuf->m_start)
	{
		return -2;
	}

	return PkData(mbuf->m_p + mbuf->m_start, mbuf->m_end - mbuf->m_start);
}

int ZMBuf::UnpkData(std::string* str)
{
	char* p = NULL;
	size_t len = 0;

	Z_CHECK_POINT(str, -1);


	Z_CHECK_FUN(UnpkData(&p, &len), -2);
	str->erase();
	if (p && len > 0)
	{
		str->append(p, len);
	}

	Z_SAFE_FREE(p);

	return 0;
}


/*alloc mem to *p*/
int ZMBuf::UnpkData(char** p, size_t* slen)
{
	size_t len = 0;
	gint8 pres;

	Z_CHECK_POINT(p, -1);
	Z_CHECK_POINT(slen, -2);

	Z_CHECK_FUN(UnpkData((gint8 *) &pres), -3);
	if (pres == Z_MBUF_STR_NULL)
	{
		*p = NULL;
		*slen = 0;
		return 0;
	}

	//not null;
//	Z_ASSERT(pres == Z_MBUF_STR_NOT_NULL);

	Z_CHECK_FUN(UnpkData((gint16 *) &len), -4);


//	Z_ASSERT(Z_MBUF_LEN <= (Z_MBUF_LEN));

	(*p) = (char *) malloc(len + 1);
	Z_CHECK_POINT((*p), -5);
	memmove((*p), Z_MBUF_P, len);
	(*p)[len] = '\0';

	*slen = len;
	m_start += len;

	return 0;
}

gintean ZMBuf::IsEmpty(void)
{
	if (m_p == NULL)
		return 1;

	if (m_start == m_end)
		return 1;

	return 0;
}

int ZMBuf::Empty()
{
	m_start = 0;
	m_end = 0;

	return 0;
}

//Append to End
int ZMBuf::AppendToTail(const char* p, size_t len)
{
	Z_CHECK_FUN(EnsureSpace(len), -1);

	memmove(m_p + m_end, p, len);
	m_end += len;

	//maybe m_end==m_len;

	return 0;
}

//Remove from Head.
int ZMBuf::RemoveFromHead(char* p)
{
	Z_CHECK_POINT(p, -1);

	if (m_start >= m_end)
	{
		return -1;
	}

	Z_CHECK_POINT(m_p, -3);

	*p = m_p[m_start];

	m_start++;

	return 0;
}




int ZMBuf::EnsureSpace(size_t len)
{
	size_t m = 0;
	size_t n = 0;
	char* pnew = NULL;

	Optimize();

	if (len + m_end <= m_len)
	{
		return 0;//ok
	}

	n = (len + m_end - m_len);//the number grow up
	m = ((n + Z_MBUF_ITEM_LEN - 1) / Z_MBUF_ITEM_LEN) * Z_MBUF_ITEM_LEN;

	m += m_len;

	pnew = (char *) malloc(m);
	Z_CHECK_POINT(pnew, -2);

	if (m_p == NULL)
	{
		m_start = 0;
		m_end = 0;
		m_len = m;
		m_p = pnew;
	}
	else
	{
		memmove(pnew, Z_MBUF_P, Z_MBUF_LEN);

		m_end -= m_start;
		m_start = 0;
		m_len = m;

		Z_SAFE_FREE(m_p);
		m_p = pnew;
	}

	return 0;
}

//convert "41 42 43 .." to "abc.."
int ZMBuf::PkHexString(const char* p)
{
	guint ch = 0;

	Z_CHECK_POINT(p, -1);

	while (*p)
	{
		if (Z_WHITESPACE(*p) || Z_CRLF(*p))
		{
			p++;
			continue;
		}

		ch = CHAR_TO_HEX(*p);
		p++;

		if (Z_WHITESPACE(*p) || Z_CRLF(*p) || *p == '\0')
		{
			PkData(ch);
			p++;
			continue;
		}

		ch *= 16;
		ch += CHAR_TO_HEX(*p);

		PkData(ch);
		p++;
	}

	return 0;
}


/*
int ZMBuf::HexPrint(const Z_SOCKADDR_IN* s1, const Z_SOCKADDR_IN* s2, gchar* info)
{
	if (m_p == NULL || m_start >= m_end)
		return -1;

#ifdef Z_MSG_PRINT
	PrintHexMsg(Z_MBUF_P, Z_MBUF_LEN, s1, s2, info);
#endif

	return 0;
}
*/
ZMBuf* ZMBuf::Dup(void)
{
	ZMBuf* pmbuf;

	pmbuf = new ZMBuf;
	if (pmbuf == NULL)
	{
		return NULL;
	}

	if (m_p && Z_MBUF_LEN > 0)
	{
		pmbuf->m_p = (char *) malloc(Z_MBUF_LEN);
		if (pmbuf->m_p == NULL)
		{
			Z_SAFE_DELETE(pmbuf);
			return NULL;
		}
		memmove(pmbuf->m_p, Z_MBUF_P, Z_MBUF_LEN);

		pmbuf->m_start = 0;
		pmbuf->m_end = Z_MBUF_LEN;
		pmbuf->m_len = Z_MBUF_LEN;
	}

	return pmbuf;//maybe NULL
}

int ZMBuf::Optimize()
{
	if (m_p == NULL || m_start < Z_MBUF_ITEM_LEN)
		return 0;

	memmove(m_p, m_p + m_start, m_end - m_start);// 0bytes move?
	m_end = m_end - m_start;
	m_start = 0;

	return 0;
}

// int ZMBuf::SendTo(SocketOperation *sock_ASS)
// {
// 	if(sock_ASS == NULL)
// 	{
// 		return -1;
// 	}
// 	
// 	if (m_end - m_start < 0)//0 bytes maybe send.
// 	{
// 		return -4;
// 	}
// 
// 	return sock_ASS->SendData(m_p + m_start, m_end - m_start);
// }

// int ZMBuf::SendTo(SocketOperation *sock_ASS,unsigned long  Type)
// {
// 	if(sock_ASS == NULL)
// 	{
// 		return -1;
// 	}
// 	
// 	if (m_end - m_start < 0)//0 bytes maybe send.
// 	{
// 		return -4;
// 	}
// 
// 	return sock_ASS->SendData(m_p + m_start, m_end - m_start,Type);
// }

int ZMBuf::IsXml(void)
{
	size_t i = 0;

	Z_CHECK_POINT(m_p, -1);

	if (Z_MBUF_LEN < 4)
	{
		return -2;
	}

	i = m_start;

	while (i < m_end && (Z_CRLF(m_p[i]) || Z_WHITESPACE(m_p[i])))
	{
		i++;
	}

	if (i + 4 < m_end &&
		(m_p[i] == '<') &&
		(m_p[i + 1] == '?') &&
		(m_p[i + 2] == 'x' || m_p[i + 2] == 'X') &&
		(m_p[i + 3] == 'm' || m_p[i + 3] == 'M') &&
		(m_p[i + 4] == 'l' || m_p[i + 4] == 'L'))
	{
		return 0;
	}

	return -10;
}

char * ZMBuf::Get_Mp()
{
	return m_p;
}

unsigned int ZMBuf::Get_Ms()
{
	return m_start;
}

unsigned int ZMBuf::Get_Me()
{
	return m_end;
}

unsigned int ZMBuf::Get_Ml()
{
	return m_len;
}

/*
unsigned long  ZMBuf::SendData(PCVOID pBuf, unsigned long  dwLen, unsigned long  Type)
{
			sockaddr_in stRemoteAddr = {0};
			stRemoteAddr.sin_family = AF_INET;
			stRemoteAddr.sin_addr.s_addr	= m_dwRemoteIP;
			stRemoteAddr.sin_port = htons(m_dwRemotePort); 
			int iLen = 0;
			for (int i = 0; i < (int) dwLen; i += 32 * 1024)
			{
				// To Limit The Trans-speed To 1MB/s (8 Mbps)
				if (i > 0)
					API_Sleep(30);
				
				int iSendLen = 32 * 1024;
				if (iSendLen > (int) dwLen - i)
					iSendLen = (dwLen - i);
				
				int iLenThis = sendto(m_dwSocket, (LPCSTR) pBuf + i, iSendLen, 0, 
					(sockaddr*) &stRemoteAddr, sizeof(stRemoteAddr));
				if (iLenThis <= 0)
					break;
				iLen += iLenThis;
			}
			if (iLen > 0)
				m_bConnect = TRUE;
			
			if (iLen != (int) dwLen)
				SetError(getsockerror());
			
			return iLen;
}
*/