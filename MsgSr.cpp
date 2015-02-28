DWORD IPGMSocket::SendData(PCVOID pBuf, DWORD dwLen, DWORD Type)
{
	if (0 == m_dwSocket || NULL == pBuf || (int) dwLen <= 0)
		return 0;
	
	SetError(0);
	
	if (PGM_SOCKET_TCP == m_dwSocketType)
	{
		int iLen = 0;
		
		for (int i = 0; i < (int) dwLen; i += 32 * 1024)
		{
			// To Limit The Trans-speed To 3MB/s (24 Mbps)
			if (i > 0)
				API_Sleep(1);
			
			int iSendLen = 32 * 1024;
			if (iSendLen > (int) dwLen - i)
				iSendLen = (dwLen - i);
			
			int iLenThis = send(m_dwSocket, (LPCSTR) pBuf + i, iSendLen, 0);
			if (iLenThis <= 0)
				break;
			iLen += iLenThis;
		}
		
		if (iLen != (int) dwLen)
			SetError(getsockerror());
		
		return iLen;
	}
	else
	{
	/*fd_set stFDS = {0};
	FD_SET(m_dwSocket, &stFDS);
	timeval stTimeOut;
	stTimeOut.tv_sec = m_dwTimeOut / 1000;
	stTimeOut.tv_usec = m_dwTimeOut % 1000;
	
		int iRet = select(0, NULL, &stFDS, NULL, &stTimeOut);*/
		
		//if (!m_bConnect)
		{
			sockaddr_in stRemoteAddr = {0};
			
			stRemoteAddr.sin_family = AF_INET;
			//if (!m_bConnect)
			// 			{
			// 				stRemoteAddr.sin_port = TO_NET_WORD((WORD) m_dwRemotePort);
			// 			}
            if (Type == ZG_EVT_HA_EXREQ || Type == ZG_EVT_HA_RESUMREQ)
			{
#ifdef WIN32
				stRemoteAddr.sin_addr.S_un.S_addr	= m_dwRemoteHAIP/*m_dwLocalIP*/;
                stRemoteAddr.sin_port = TO_NET_WORD((WORD) m_dwRemoteHAPort);
#else
				stRemoteAddr.sin_addr.s_addr	= m_dwRemoteHAIP/*m_dwLocalIP*/;
				stRemoteAddr.sin_port = htons(m_dwRemoteHAPort); 
#endif
			} 
			else
			{
#ifdef WIN32
				stRemoteAddr.sin_addr.S_un.S_addr	= m_dwRemoteIP/*m_dwLocalIP*/;
                stRemoteAddr.sin_port = TO_NET_WORD((WORD) m_dwRemotePort);
#else
				stRemoteAddr.sin_addr.s_addr	= m_dwRemoteIP/*m_dwLocalIP*/;
				stRemoteAddr.sin_port = htons(m_dwRemotePort); 
#endif
			}
			
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
	}
}