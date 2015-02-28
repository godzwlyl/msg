// ***************************************************************
//  MsgHead   version:  1.0   ? date: 02/14/2007
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
// liuhuisong@zgwri.com
// ***************************************************************
// ZMsgHead.cpp: implementation of the ZMsgHead class.
//
//////////////////////////////////////////////////////////////////////

/*#include "Gen.h"*/
#include "MsgHead.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


ZMsgHead::ZMsgHead(gchar* buf, gsize len) : ZMBuf(buf, len)
{
}

ZMsgHead::ZMsgHead()
{
}

ZMsgHead::~ZMsgHead()
{
}

int ZMsgHead::UnpkPost(int bRemove, int* pSource, int* pDest,
	int* pEvt)
{
	int temp;

	size_t start=m_start;

	Z_CHECK_POINT(m_p, -1);	if (m_end - m_start < Z_MSGHEAD_POST_LEN)
	{
		return -1;
	}

	if (pSource)
	{
		UnpkData(pSource);
	}
	else
	{
		UnpkData(&temp);
	}

	if (pDest)
	{
		UnpkData(pDest);
	}
	else
	{
		UnpkData(&temp);
	}

	if (pEvt)
	{
		UnpkData(pEvt);
	}
	else
	{
		UnpkData(&temp);
	}

	if (!bRemove)
	{
		m_start=start;
	}

	return 0;
}

int ZMsgHead::PkPost(int src, int dst, int evt)
{
	Z_CHECK_FUN(PkData(src),-1);
	Z_CHECK_FUN(PkData(dst),-1);
	Z_CHECK_FUN(PkData(evt),-1);
	
	return 0;
}
