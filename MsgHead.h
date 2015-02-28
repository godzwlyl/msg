// ***************************************************************
//  MsgHead   version:  1.0   ? date: 02/14/2007
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
// liuhuisong@zgwri.com
// ***************************************************************
// ZMsgHead.h: interface for the ZMsgHead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZMSGHEAD_H__78335402_8E36_4FAC_9036_744A336EAD10__INCLUDED_)
#define AFX_ZMSGHEAD_H__78335402_8E36_4FAC_9036_744A336EAD10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #include <glib.h>
// 
// #include "Gen.h"
#include "buf.h"

#define Z_MSGHEAD_POST_LEN	(sizeof(int)+sizeof(int)+sizeof(int))


class ZMsgHead : public ZMBuf
{
public:
	ZMsgHead(gchar* buf, gsize len);
	ZMsgHead();
	virtual ~ZMsgHead();

public:
	int UnpkPost(int bRemove, int* src, int* dst, int* evt);
	int PkPost(int src, int dst, int evt);

private:
};

#endif // !defined(AFX_ZMSGHEAD_H__78335402_8E36_4FAC_9036_744A336EAD10__INCLUDED_)
