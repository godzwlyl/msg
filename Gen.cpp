#include "Gen.h"

/*********************************************************
*函数名称：GetIntValue()								        
*函数功能：将数组的前n位转化为数字，并返回	
*参数说明:
* 	s		        需要转化的数组
* 	n               转化的位数
*返回值：转化后的结果
**********************************************************/
int  TimeOperation::GetIntValue(  char * s , int n ) 
{
	char t[10];
	strncpy( t , s , n );
	t[n] = '\0';
	return atoi( t );
}
/*********************************************************
*函数名称：GetDuration()								        
*函数功能：返回两个时间之间相差的秒数	
*参数说明:
* 	cStartTime	起始时间											
* 	cEndTime	截止时间													
*返回值：小于0: 出错   >=0: 秒数	
**********************************************************/
int TimeOperation::GetDuration(char *cStartTime , char *cEndTime)
{
	struct tm sStartTime;
	struct tm sEndTime;
	time_t t1,t2;
	long lDuration;

	sStartTime.tm_year = GetIntValue( cStartTime , 4 ) - 1900;
	sStartTime.tm_mon  = GetIntValue( & cStartTime[4] , 2 ) - 1;
	sStartTime.tm_mday = GetIntValue( & cStartTime[6] , 2 );
	sStartTime.tm_hour = GetIntValue( & cStartTime[8] , 2 );
	sStartTime.tm_min  = GetIntValue( & cStartTime[10] , 2 );
	sStartTime.tm_sec  = GetIntValue( & cStartTime[12] , 2 );
	sStartTime.tm_isdst = 0;
	if ( (t1=mktime( & sStartTime )) == - 1 ) return - 3;

	sEndTime.tm_year = GetIntValue( cEndTime , 4 ) - 1900;
	sEndTime.tm_mon  = GetIntValue( & cEndTime[4] , 2 ) - 1;
	sEndTime.tm_mday = GetIntValue( & cEndTime[6] , 2 );
	sEndTime.tm_hour = GetIntValue( & cEndTime[8] , 2 );
	sEndTime.tm_min  = GetIntValue( & cEndTime[10] , 2 );
	sEndTime.tm_sec  = GetIntValue( & cEndTime[12] , 2 );
	sEndTime.tm_isdst = 0;
	if ( (t2=mktime( & sEndTime )) == - 1 ) return - 3;
	lDuration=t2-t1 ;
	return lDuration;
} 

/*********************************************************
*函数名称：GetCallEndTime()								        
*函数功能：根据起始日期时间时长取得结束日期时间	
*参数说明:
* 	StartDay	起始日期											*
* 	StartTime	起始时间											*
* 	Duration	时长												*
* 	EndDay		结束日期											*
* 	EndTime		结束时间											*
*返回值：0：成功，非0：失败	
**********************************************************/

int TimeOperation::GetCallEndTime( char * startday ,  char * starttime,int duration, char * endday , char * endtime)
{
	struct tm CallStart;

	CallStart.tm_year = GetIntValue( startday , 4 ) - 1900;
	CallStart.tm_mon  = GetIntValue( & startday[4] , 2 ) - 1;
	CallStart.tm_mday = GetIntValue( & startday[6] , 2 );
	CallStart.tm_hour = GetIntValue( starttime , 2 );
	CallStart.tm_min  = GetIntValue( & starttime[2] , 2 );
	CallStart.tm_sec  = GetIntValue( & starttime[4] , 2 );
	CallStart.tm_isdst = 0;

	CallStart.tm_sec += duration;
	if ( mktime( & CallStart ) == - 1 )return - 3;
	sprintf( endday , "%04d%02d%02d" , CallStart.tm_year + 1900 , CallStart.tm_mon + 1 , CallStart.tm_mday );
	sprintf( endtime , "%02d%02d%02d" , CallStart.tm_hour , CallStart.tm_min , CallStart.tm_sec );

	return 0;
}

/*********************************************************
*函数名称：GetSegmentTime()								        
*函数功能：计算跨越规定时间点之前和之后分割时长	
*参数说明:
* 	Sdate		起始日期										*
* 	Stime		起始时间										*
* 	Duration	时长										*
* 	SpotDate	跨越日期点										*
* 	SpotTime	跨越时间点										*
* 	Leap		跨越点之前计费跳次（秒）								*
* 	dt1		跨越前时长（秒）									*
* 	dt2		跨越后时长（秒)										*
*返回值：0：成功，非0：失败	
**********************************************************/
int TimeOperation::GetSegmentTime( char * Sdate,  char * Stime ,int Duration ,char * SpotDate ,
				   char * SpotTime ,int  Leap ,int * dt1 ,int * dt2 )
{

	char sTmpDateTime[16] , sSpotDateTime[16];
	int nLeapNum , nDurtn;

	memset(sTmpDateTime, '\0', sizeof(sTmpDateTime) );
	memset(sSpotDateTime, '\0', sizeof(sSpotDateTime) );

	strcpy(sSpotDateTime , SpotDate);
	strcat(sSpotDateTime , SpotTime);

	strcpy(sTmpDateTime , Sdate);
	strcat(sTmpDateTime , Stime);

	*dt1 = Duration;
	*dt2 = 0;

	if(Leap>=Duration)
	{
		*dt1 = Duration;
		*dt2 = 0;
	}
	else
	{
		nDurtn = GetDuration ( sTmpDateTime , sSpotDateTime );
		if(strncmp(&sSpotDateTime[10],"5959",4)==0)
			nDurtn+=1;
		nLeapNum = (nDurtn + (Leap -1))/Leap ; 
		if( ( nLeapNum * Leap ) >= Duration )
			*dt1 = Duration ;
		else
		{
			*dt1 = nLeapNum * Leap ;
			*dt2 = Duration - *dt1;
		}
	}
	return 0;
}
/*********************************************************
*函数名称：GetPointDurtn()								        
*函数功能：计算分别在闲时和忙时的时长	
*参数说明:
* 	sStartDate		起始日期	
* 	sStartTime		起始时间
* 	iCallDurtn		时长
* 	iUnitLen		计费跳次（秒）
* 	sPointTime1		忙时起始时间
* 	sPointTime2		忙时截止时间
* 	iDurtn1			落在sPointTime1和sPointTime2之间的时长(忙时）
* 	iDurtn2			落在sPointTime1和sPointTime2之外的时长(闲时）
*返回值：0：成功，非0：失败	
**********************************************************/
int TimeOperation::GetPointDurtn(string strStartDate,string strStartTime,int iCallDurtn,int iUnitLen,string strPointTime1,string strPointTime2,int *iDurtn1,int *iDurtn2,int iFreeDurtn)
{

	char sCallEndDate[9];
	char sCallEndTime[7];
	char sPointDate[9];
	char sPointTime[7];

	char sStartDate[9];
	char sStartTime[7];

	const char * sStartDate_tmp = strStartDate.c_str();
	const char * sStartTime_tmp = strStartTime.c_str();
	const char * sPointTime1 = strPointTime1.c_str();
	const char * sPointTime2 = strPointTime2.c_str();


// 	char sTmpStartDate[9];
// 	char sTmpStartTime[7];
	char sStartPointdate1[15];
	char sStartPointdate2[15];
	int  iBillDurtn1 , iBillDurtn2 , nPointDurtn;
	int nFlag1,nFlag2;

	strcpy( sStartDate, sStartDate_tmp );
	strcpy( sStartTime, sStartTime_tmp );

	///*当免费时长大于零时，减去免费时长*/
	//if( iFreeDurtn > 0 )
	//{
	//	GetCallEndTime( sStartDate , sStartTime , iFreeDurtn , sTmpStartDate , sTmpStartTime );
	//	strcpy(sStartDate , sTmpStartDate );
	//	strcpy(sStartTime , sTmpStartTime );
	//	iCallDurtn = iCallDurtn - iFreeDurtn ;
	//}

	strcpy(sStartPointdate1 , sStartDate );
	strcat(sStartPointdate1 , sPointTime1 );
	strcpy(sStartPointdate2 , sStartDate );
	strcat(sStartPointdate2 , sPointTime2 );

	/*取起始时间与截止时间之间的时差*/
	nPointDurtn = GetDuration(sStartPointdate1,sStartPointdate2)+1;

	/*取通话结束时间*/
	GetCallEndTime( sStartDate , sStartTime , iCallDurtn , sCallEndDate , sCallEndTime );

	nFlag1=0;
	nFlag2=0;
	if( strncmp(sStartTime , sPointTime1 , 6)>=0 && strncmp(sStartTime , sPointTime2 , 6)<=0 )
		nFlag1=1;
	if(	strncmp(sCallEndTime , sPointTime1 , 6)>=0 && strncmp(sCallEndTime , sPointTime2 , 6)<=0 )
		nFlag2=1;
	if( nFlag1==1 && nFlag2==1 )
	{
		if( iCallDurtn > ((24*3600) - nPointDurtn) )
		{
			iBillDurtn2 = (24*3600) - nPointDurtn;
			iBillDurtn1 = iCallDurtn - ((24*3600) - nPointDurtn);
		}
		else
		{
			iBillDurtn1 = iCallDurtn;
			iBillDurtn2 = 0;
		}	
	}
	else if( nFlag1 == 0 && nFlag2 == 0 )
	{
		if( iCallDurtn > nPointDurtn )
		{
			iBillDurtn1 = nPointDurtn;
			iBillDurtn2 = iCallDurtn - nPointDurtn;
		}
		else
		{
			iBillDurtn2 = iCallDurtn;
			iBillDurtn1 = 0;
		}	
	}
	else if( nFlag1 == 1 && nFlag2 == 0 )
	{
		strcpy( sPointDate , sStartDate);
		strcpy( sPointTime , sPointTime2);
		GetSegmentTime( sStartDate , sStartTime , iCallDurtn , sPointDate ,
			sPointTime , iUnitLen , &iBillDurtn1 , &iBillDurtn2 );		
	}
	else if( nFlag1 == 0 && nFlag2 == 1 )
	{	
		strcpy( sPointDate , sCallEndDate );
		strcpy( sPointTime , sPointTime1);
		GetSegmentTime( sStartDate , sStartTime , iCallDurtn , sPointDate ,
			sPointTime , iUnitLen , &iBillDurtn2 , &iBillDurtn1 );		
	}
	*iDurtn1 = ( iBillDurtn1 + (iUnitLen -1 ) )/iUnitLen;
	*iDurtn2 = ( iBillDurtn2 + (iUnitLen -1 ) )/iUnitLen;
	return 0;
}
/*********************************************************
*函数名称：GetDateTimeString()								        
*函数功能：获取当前形如%Y-%m-%d %H:%M:%S的字符串形式的时间
*参数说明:
* 	strTime		       返回的时间
**********************************************************/
void TimeOperation::GetDateTimeString(string& strTime)
{
	char tmpbuf[32];
	time_t aclock;
	struct tm *newtime;

	// Covert current time to ASCII string
	time(&aclock);
	newtime = localtime(&aclock);
	strftime(tmpbuf, 32, "%Y-%m-%d %H:%M:%S", newtime);
	strTime = string(tmpbuf);
}
/*********************************************************
*函数名称：TimeToString()								        
*函数功能：将time_t时间转化为形如%Y-%m-%d %H:%M:%S的字符串形式的时间
*参数说明:
* 	pstTime		       time_t时间
**********************************************************/
char * TimeOperation::TimeToString(const time_t * pstTime, char *pcBuffer)
{
	if (NULL != pcBuffer)
	{
		pcBuffer[0] = 0;
		if (NULL != pstTime && 0 != *pstTime)
		{
			struct tm *newtime = localtime(pstTime);
			strftime(pcBuffer, 20, "%Y-%m-%d %H:%M:%S", newtime);
		}
	}

	return pcBuffer;
}

/*********************************************************

*函数名称：CompareTime()

函数功能：比较两个时间的大小

参数说明: 

cATime	第一个时间A	

cBTime 　第二个时间B

返回值： >0　 时间A比时间B晚

　　　　 <0　 时间A比时间B早

	 　　　　 =0   时间A等于时间B

		  *

		  *********************************************************/
int TimeOperation::CompareTime(char *cATime,char *cBTime)
{
	return GetDuration(cATime,cBTime);
}
/*********************************************************

*函数名称：GetCallWeekDay()								        

*函数功能：根据日期取得其对应的星期数

*参数说明:

* 	cDay			日期	

* 	iWeekDay		对应星期数（0－6，星期日为0）

*返回值：0：成功，非0：失败	

**********************************************************/

int TimeOperation::GetCallWeekDay(char * cDay , int *iWeekDay)

{
	struct tm CallStart;
	CallStart.tm_year = GetIntValue( cDay , 4 ) - 1900;
	CallStart.tm_mon  = GetIntValue( & cDay[4] , 2 ) - 1;
	CallStart.tm_mday = GetIntValue( & cDay[6] , 2 );
	CallStart.tm_hour = 12;
	CallStart.tm_min  = 12;
	CallStart.tm_sec  = 12;
	CallStart.tm_isdst = 0;
	if ( mktime( & CallStart ) == - 1 )return - 3;
	*iWeekDay=CallStart.tm_wday;
	return 0;
}




/*********************************************************
*函数名称：GetCurrentMonthEnd();						        
*函数功能：获取当月月底
*参数说明:
**********************************************************/
void TimeOperation::GetCurrentMonthEnd(char *TME)
{
	time_t   t=time(NULL);   
	struct   tm   tt   =   *localtime(&t);   
	tt.tm_mon++;   
	tt.tm_mday   =   1;   
	tt.tm_hour   =   0;   
	tt.tm_min   =   0;   
	tt.tm_sec   =   0;   
	t   =   mktime(&tt);
	t--;
	/*    char   ctime[20]; */  

	struct tm *newtime = localtime(&t);
	sprintf(TME,"%04d%02d%02d%02d%02d%02d",newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour,newtime->tm_min,newtime->tm_sec);
	// 
	// 	strftime(ctime,31,"%Y-%m-%d   %H:%M:%S",localtime(&t));   
}
/*********************************************************
*函数名称：GetCurrentMonthEnd();						        
*函数功能：获取当月月初
*参数说明:
**********************************************************/
void TimeOperation::GetCurrentMonthStart(char *TME)
{
	time_t   t=time(NULL);   
	struct   tm   tt   =   *localtime(&t);   
	/*	tt.tm_mon++; */  
	tt.tm_mday   =   1;   
	tt.tm_hour   =   0;   
	tt.tm_min   =   0;   
	tt.tm_sec   =   0;   
	t   =   mktime(&tt);
	/*	t--;*/
	/*    char   ctime[20]; */  

	struct tm *newtime = localtime(&t);
	sprintf(TME,"%04d%02d%02d%02d%02d%02d",newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour,newtime->tm_min,newtime->tm_sec);
	// 
	// 	strftime(ctime,31,"%Y-%m-%d   %H:%M:%S",localtime(&t));   
}
/*********************************************************
*函数名称：GetCurrentMonthEnd();						        
*函数功能：获取下一月月初
*参数说明:
**********************************************************/
void TimeOperation::GetNextMonth(char *TME)
{
	time_t   t=time(NULL);   
	struct   tm   tt   =   *localtime(&t);   
	tt.tm_mon++;   
	tt.tm_mday   =   1;   
	tt.tm_hour   =   0;   
	tt.tm_min   =   0;   
	tt.tm_sec   =   0;   
	t   =   mktime(&tt);
	/*	t--;*/
	/*    char   ctime[20]; */  

	struct tm *newtime = localtime(&t);
	sprintf(TME,"%04d%02d%02d%02d%02d%02d",newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour,newtime->tm_min,newtime->tm_sec);
	// 
	// 	strftime(ctime,31,"%Y-%m-%d   %H:%M:%S",localtime(&t));   
}
/*********************************************************
*函数名称：ConvertStringFormatDate();						        
*函数功能：把字符串格式的日期时间yyyymmddhh24miss 转化成为string格式
*参数说明:
**********************************************************/
string TimeOperation::ConvertStringFormatDate( char *in_date ) 
{
	assert(in_date != NULL);
	struct  tm    *tmp_ptrTmTime;

	tmp_ptrTmTime = ( struct  tm    *)malloc( sizeof( struct tm ) );
	if( tmp_ptrTmTime == NULL )
	{
		return 0;
	}
	else
	{
		memset( tmp_ptrTmTime, 0, sizeof( struct tm ) );   /* 初始化分配的临时内存 */
	}

	char buf[10];
	buf[2]=0;
	buf[4]=0;
	memmove(buf,in_date+12,2);
	tmp_ptrTmTime->tm_sec   = atoi(buf);
	memmove(buf,in_date+10,2);
	tmp_ptrTmTime->tm_min   = atoi(buf);
	memmove(buf,in_date+8,2);
	tmp_ptrTmTime->tm_hour  = atoi(buf);
	memmove(buf,in_date+6,2);
	tmp_ptrTmTime->tm_mday  = atoi(buf);
	memmove(buf,in_date+4,2);
	tmp_ptrTmTime->tm_mon   = atoi(buf);
	memmove(buf,in_date,4);
	tmp_ptrTmTime->tm_year  = atoi(buf)     ;
	tmp_ptrTmTime->tm_isdst = 0;


	char strResult[20];

	memset(strResult, 0x00, 20);
	sprintf(strResult, "%04d-%02d-%02d %02d:%02d:%02d", tmp_ptrTmTime->tm_year, tmp_ptrTmTime->tm_mon, tmp_ptrTmTime->tm_mday, 
		tmp_ptrTmTime->tm_hour, tmp_ptrTmTime->tm_min, tmp_ptrTmTime->tm_sec);

	if( tmp_ptrTmTime != NULL )
	{
		free( tmp_ptrTmTime );        /* 释放分配的内存 */
	}

	return string(strResult);

}
/*********************************************************
*函数名称：ConvertStringDate();						        
*函数功能：把字符串格式的日期时间yyyymmddhh24miss 转化成为time_t格式
*参数说明:
**********************************************************/
time_t TimeOperation::ConvertStringDate( char *in_date )
{
	assert(in_date != NULL);
	struct  tm    *tmp_ptrTmTime;
	time_t        tmpLTime;

	tmp_ptrTmTime = ( struct  tm    *)malloc( sizeof( struct tm ) );
	if( tmp_ptrTmTime == NULL )
	{
		return -1;
	}
	else
	{
		memset( tmp_ptrTmTime, 0, sizeof( struct tm ) );   /* 初始化分配的临时内存 */
	}

	char buf[10];
	buf[2]=0;
	buf[4]=0;
	memmove(buf,in_date+12,2);
	tmp_ptrTmTime->tm_sec   = atoi(buf);
	memmove(buf,in_date+10,2);
	tmp_ptrTmTime->tm_min   = atoi(buf);
	memmove(buf,in_date+8,2);
	tmp_ptrTmTime->tm_hour  = atoi(buf);
	memmove(buf,in_date+6,2);
	tmp_ptrTmTime->tm_mday  = atoi(buf);
	memmove(buf,in_date+4,2);
	tmp_ptrTmTime->tm_mon   = atoi(buf)-1;
	memmove(buf,in_date,4);
	tmp_ptrTmTime->tm_year  = atoi(buf)-1900     ;
	tmp_ptrTmTime->tm_isdst = 0;

	tmpLTime = mktime( tmp_ptrTmTime );

	if( tmpLTime == -1 )
	{
		if( tmp_ptrTmTime != NULL )
		{
			free( tmp_ptrTmTime );    /* 释放分配的内存 */
		}

		return -1;
	}

	if( tmp_ptrTmTime != NULL )
	{
		free( tmp_ptrTmTime );        /* 释放分配的内存 */
	}

	return tmpLTime;

}
//////////////////////////////////////////////////////////////////////
//判断时间是否合法		
//////////////////////////////////////////////////////////////////////////
int TimeOperation::nIsTime( char * str )
{
	char tmp[10],tmp2[10];

	memset(tmp,0,sizeof(tmp));
	memset(tmp2,0,sizeof(tmp2));
	strncat(tmp,&str[4],2);
	strncat(tmp2,&str[6],2);
	switch (atoi(tmp))
	{
	case 2:
		if (!(atoi(tmp2) >= 1 && atoi(tmp2) <= 29))
			return -1;
		break;
	case 1:	case 3:	case 5:	case 7:	case 8:	case 10: case 12:
		if (!(atoi(tmp2) >= 1 && atoi(tmp2) <= 31))
			return -1;
		break;
	case 4:	case 6:	case 9:	case 11:
		if (!(atoi(tmp2) >= 1 && atoi(tmp2) <= 30))
			return -1;
		break;
	default:
		return -1;
	}	

	/*hour*/
	memset(tmp,0,sizeof(tmp));
	strncat(tmp,&str[8],2);
	if (atoi(tmp)<0 || atoi(tmp)>24)
	{
		return -1;
	}

	/*minute*/
	memset(tmp,0,sizeof(tmp));
	strncat( tmp,&str[10],2 );
	if (atoi(tmp)<0 || atoi(tmp)>62)
	{
		return -1;
	}

	/*second*/
	memset(tmp,0,sizeof(tmp));
	strncat( tmp,&str[12],2 );
	if (atoi(tmp)<0 || atoi(tmp)>62)
	{
		return -1;
	}
	return 0;
}
/*******************************************************************************************/
/*函数名称：GetInterval */
/*函数功能：得到时简间隔，单位秒，时间格式YYYYMMDDHHMMSS*/
/*******************************************************************************************/
long TimeOperation::GetInterval(char* End, char* Start)
{
	tm tmEnd,tmStart;
	long lEnd,lStart,InterTime;
	char tmp[5] = {0};

	strncpy(tmp,Start,4);
	tmStart.tm_year = atoi(tmp) - 1900;
	ZERO(tmp);
	strncpy(tmp,&Start[4],2);
	tmStart.tm_mon = atoi(tmp) - 1;
	ZERO(tmp);
	strncpy(tmp,&Start[6],2);
	tmStart.tm_mday = atoi(tmp);
	ZERO(tmp);
	strncpy(tmp,&Start[8],2);
	tmStart.tm_hour = atoi(tmp);
	ZERO(tmp);
	strncpy(tmp,&Start[10],2);
	tmStart.tm_min = atoi(tmp);
	ZERO(tmp);
	strncpy(tmp,&Start[12],2);
	tmStart.tm_sec = atoi(tmp);
	tmStart.tm_isdst = 0;
	lStart = mktime(&tmStart);

	strncpy(tmp,End,4);
	tmEnd.tm_year = atoi(tmp) - 1900;
	ZERO(tmp);
	strncpy(tmp,&End[4],2);
	tmEnd.tm_mon = atoi(tmp) - 1;
	ZERO(tmp);
	strncpy(tmp,&End[6],2);
	tmEnd.tm_mday = atoi(tmp);
	ZERO(tmp);
	strncpy(tmp,&End[8],2);
	tmEnd.tm_hour = atoi(tmp);
	ZERO(tmp);
	strncpy(tmp,&End[10],2);
	tmEnd.tm_min = atoi(tmp);
	ZERO(tmp);
	strncpy(tmp,&End[12],2);
	tmEnd.tm_sec = atoi(tmp);
	tmEnd.tm_isdst = 0;
	lEnd = mktime(&tmEnd);

	InterTime = lEnd - lStart;
	return InterTime;
}
///////////////////////////////////////////////////////////////////////////////
//返回两个日期相差的天数
int TimeOperation:: GetDayBetweenDate( char * pStartdate, char * pEnddate )
{
	assert(pStartdate != NULL);
	assert(pEnddate != NULL);
	char sTmpstartdate[15];
	char sTmpenddate[15];
	time_t Tmps1, Tmps2;

	sprintf( sTmpstartdate, "%s000000", pStartdate);
	sprintf( sTmpenddate, "%s000000", pEnddate);
	Tmps1 = ConvertStringDate( sTmpstartdate );
	Tmps2 = ConvertStringDate( sTmpenddate );
	return ( Tmps2 - Tmps1)/3600/24;

}
//判断日期的奇偶月(0:奇月,1:偶月)，1.5修改
int TimeOperation::isOddMonth( char * pIndate )
{
	assert(pIndate != NULL);
	char  sTmp[3];
	memset( sTmp, 0, sizeof(sTmp) );
	strncpy( sTmp, &pIndate[4], 2 );
	return atoi( sTmp)%2==0 ? 0:1;
}

/*********************************************************
*函数名称：GetIntValue()								        
*函数功能：将数组的前n位转化为数字，并返回	
*参数说明:
* 	s		        需要转化的数组
* 	n               转化的位数
*返回值：转化后的结果
**********************************************************/
int  StringOpearation::GetIntValue(  char * s , int n ) 
{
	char t[10];
	strncpy( t , s , n );
	t[n] = '\0';
	return atoi( t );
}
/*********************************************************
*函数名称：StringToHex();		HexToString						        
*函数功能：字符串与十六进制互转	
*参数说明:
* 	s		        需要转化的数组
* 	n               转化的位数
*返回值：转化后的结果
**********************************************************/
void StringOpearation::StringToHex(const string &strSource, BYTE* pBuffer, size_t nBytes)
{
	const char* s = strSource.c_str();

	for (size_t i = 0; i < nBytes; i++)
	{
		const size_t stringOffset = i * 2;
		BYTE val = 0;

		// BYTE 1
		const BYTE b = s[stringOffset];

		if (isdigit(b))
		{
			val = (BYTE)((b - '0') * 16);
		}
		else 
		{
			val = (BYTE)(((toupper(b) - 'A') + 10) * 16);
		}

		// BYTE 2
		const BYTE b1 = s[stringOffset + 1];

		if (isdigit(b1))
		{
			val += b1 - '0';
		}
		else 
		{
			val += (BYTE)((toupper(b1) - 'A') + 10);
		}

		pBuffer[i] = val;
	}
}

string StringOpearation::HexToString(const BYTE* pBuffer, size_t iBytes)
{
	string result;

	for (size_t i = 0; i < iBytes; i++)
	{
		BYTE c ;

		BYTE b = pBuffer[i] >> 4;

		if (9 >= b)
		{
			c = b + '0';
		}
		else
		{
			c = (b - 10) + 'A';
		}

		result += (char)c;

		b = pBuffer[i] & 0x0f;

		if (9 >= b)
		{
			c = b + '0';
		}
		else
		{
			c = (b - 10) + 'A';
		}

		result += (char)c;
	}

	return result;
}
/*********************************************************
*函数名称：RemoveStrBlanks();		GetStringStem（）						        
*函数功能：去掉字符串中的空格	
*参数说明:
**********************************************************/
void StringOpearation::RemoveStrBlanks(string& strOuter)
{
	string s = strOuter;
	size_t start = s.find_first_not_of(' ');
	if (start == s.npos)
		return;

	size_t end = s.find_last_not_of(' ') + 1;
	s = s.substr(start, end - start);

	strOuter = s;

}
string StringOpearation::GetStringStem(const string & strOrg)
{
	string _copy = strOrg;
	RemoveStrBlanks(_copy);
	return _copy;
}
//////////////////////////////////////////////////////////////////////////////////////
//函数功能：删除字符串尾部的空格
///////////////////////////////////////////////////////////////////////////////////////
void StringOpearation::DelSpace(char * str)
{
	assert(str != NULL);
	unsigned long len,i;
	len=strlen(str);
	for (i=len;i>0;i--)
	{
		if (str[i-1]==' ') str[i-1]='\0';
		else if	(str[i-1]=='\r') str[i-1]='\0';
		else if	(str[i-1]=='\n') str[i-1]='\0';
		else break;
	}
}
/*******************************************************************************************/
/*函数名称：rtrim */
/*函数功能：去掉字符串右边的空格*/
/*******************************************************************************************/
void  StringOpearation:: rtrim(char *str )
{
	int		i;

	for(i = strlen(str) - 1; (i >= 0) && (' ' == str[i]); i --)
		str[i] = 0;
}

/*******************************************************************************************/
/*函数名称：ltrim */
/*函数功能：去掉字符串左边的空格*/
/*******************************************************************************************/
void  StringOpearation::ltrim(char * str)
{
	int		i;
	char * str1;


	for(i = 0; (i < strlen(str)) && (' ' == str[i]); i ++);

	str1 = &str[i];

	for( ; *str++ = *str1++; );
}

/*******************************************************************************************/
/*函数名称：trim */
/*函数功能：去掉字符串两边的空格*/
/*******************************************************************************************/
void  StringOpearation::trim(char * str)
{
	ltrim(str);
	rtrim(str);
}

/**********************************************************************************************/
/*      函数名称：rpad										   */
/*      函数功能：如果目标字符串长度小于len，设置目标字符串长度为len，尾部以字符c填充	   */
/*      返回值：0，成功，非0，失败          							   */
/**********************************************************************************************/

void  StringOpearation::rpad( char * src,int len,char c )
{
	int nLen;
	nLen = strlen( src );
	for( ;nLen<len;nLen++  )
	{
		*(src+nLen) = c ;
	}
	*( src+nLen ) = '\0' ;
}
/*******************************************************************************************/
/*函数名称：strupr */
/*函数功能：小写字母变大写字母*/
/*******************************************************************************************/
void  StringOpearation::StrupWord(char * lwrstr)
{
	char * pcStr;
	pcStr = lwrstr ;
	while (*pcStr)
	{
		if (*pcStr >= 97 && *pcStr <=122)
		{
			*pcStr = *pcStr - 32;	
		}

		pcStr++;	
	}
}
/*******************************************************************************************/
/*函数名称：strupr */
/*函数功能：大写字母变小写字母*/
/*******************************************************************************************/

void StringOpearation::StrlWord(char * lwrstr)
{
	char * pcStr;
	pcStr = lwrstr ;
	while (*pcStr)
	{
		if ( *pcStr >= 65 && *pcStr <=90 )
		{
			*pcStr = *pcStr + 32;	
		}

		pcStr++;	
	}
}
/*******************************************************************************************/
/*函数名称：strlmv */
/*函数功能：字母串左移*/
/*******************************************************************************************/

void StringOpearation::strlmv(char * src,int n)
{
	while(*src)
	{
		*src=*(src+n);
		if (!(*(src+n))) break;
		src++;
	}
}

/*******************************************************************************************/
/*函数名称：strnvl */
/*函数功能：给空字符串赋值*/
/*******************************************************************************************/

void StringOpearation::strnvl(char * src,char * content)
{
	if (*src=='\0')
		strcpy(src,content);

}


/*********************************************************
*函数名称：LOG_FILE();								        
*函数功能：写日志文件	
*参数说明:
* 	pFile		    日志文件描述符
* 	strMsg               日志内容
**********************************************************/
void FileOperation::LOG_FILE(void *pFile, LPCSTR strMsg, ...)
{
	if (NULL == pFile)
		return;

	if (NULL == strMsg)
	{
		fclose((FILE*) pFile);
		return;
	}

	int ics = ((clock() ) % 1000) / 10;

	time_t tmNow = time(&tmNow);
	tm *p = localtime(&tmNow);

	char acBuf[1024];
	char *pcBuf = acBuf;

	int iLen = (((LPCSTR) -1) != strMsg ? strlen(strMsg) : 0);

	if (iLen > 1024 - 128)
	{
		pcBuf = new char[iLen + 128];
	}

	static int s_iLastDay = 0;

	if (((LPCSTR) -1) == strMsg || s_iLastDay != p->tm_yday)
	{
		s_iLastDay = p->tm_yday;
		sprintf(pcBuf, 
			"\n[DATE:%04d-%02d-%02d]\n[%02d:%02d:%02d.%02d] %s\n", 
			p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, 
			p->tm_hour, p->tm_min, p->tm_sec, ics,
			((LPCSTR) -1 == strMsg ? "\n" : strMsg ));
	}
	else
	{
		sprintf(pcBuf, "[%02d:%02d:%02d.%02d] %s\n", 
			p->tm_hour, p->tm_min, p->tm_sec, ics, strMsg);
	}

	va_list pArg;
	va_start(pArg, strMsg);
	vfprintf((FILE *) pFile, pcBuf, pArg);
	va_end(pArg);

	fflush((FILE *) pFile);

	if (pcBuf != acBuf)
	{
		delete [] pcBuf;
	}

	return;
}


///////////////////////////////////////////////////////////////////////////////////////
int FileOperation::lockfile( char * pfile )
{
	int fd;
	fd = open( pfile,O_WRONLY|O_CREAT|O_EXCL,0666 );
	if ( fd > 0 )
	{
		close( fd );
		return 0;
	}
	return -1;
}
///////////////////////////////////////////////////////////////////////////////////////
int unlockfile( char * pfile )
{
	return unlink( pfile );
}
/*******************************************************************************************/
/*函数名称：fgetline */
/*函数功能：得到文件行数*/
/*******************************************************************************************/
int  FileOperation::fgetline(const string FileName)
{
	int  i, j; 
	char ch1[500] = {0}; 
	FILE *fp;

	if ((fp = fopen(FileName.c_str(), "r")) == NULL)
		return 0;

	for(i = 0, j = 0; ; j++) 
	{ 
		if(!fgets(ch1,sizeof(ch1),fp)) 
		{ 
			if(feof(fp) != 0)  
			{
				fclose(fp);
				return j; 
			}
		} 
	} 
}
SocketOperation::SocketOperation(DWORD dwSocketType)
{
	m_dwLocalIP = 0;
	m_dwRemoteIP = 0;
	m_dwLocalPort = 0;
	m_dwRemotePort = 0;
	m_dwRemoteHAIP = 0;  //
	m_dwRemoteHAPort = 0;
	m_bBindRemote = FALSE;
	m_bConnect = FALSE;

	m_dwSocket = 0;
	m_dwSocketType = dwSocketType;
	m_dwTimeOut = 1000;

	if (PGM_SOCKET_UNKNOWN != dwSocketType)
		CreateSocket(dwSocketType);

	SetError(ERR_PGM_OK);

	m_dwSocketId = sm_dwSocketIDSerialNo++;
	m_iLastFrameDataPos = 0;
}

///////////////////////////////////////////////////////////////////////////////////////
SocketOperation::~SocketOperation()
{
	DisConnect();
	Detach();
}

int SocketOperation::Attach(unsigned long dwSocket)
{
	Detach();

	m_dwSocket = dwSocket;
	m_bDataSocket = 1;

	return (0 != m_dwSocket);
}

///////////////////////////////////////////////////////////////////////////////////////
int SocketOperation::Detach()
{
	if (0 != m_dwSocket)
	{
		close(m_dwSocket);

		m_dwLocalIP = 0;
		m_dwRemoteIP = 0;
		m_dwLocalPort = 0;
		m_dwRemotePort = 0;
		m_dwRemoteHAIP = 0;  //
		m_dwRemoteHAPort = 0;

		m_dwSocket = 0;
		m_dwSocketType = 0;
		m_dwTimeOut = 1000;

		m_bDataSocket = 0;
	}

	return 0;
}
int SocketOperation::CreateSocket(unsigned long dwSocketType)
{
	Detach();

	if (PGM_SOCKET_TCP == dwSocketType)
		dwSocketType = SOCK_STREAM;
	else if (PGM_SOCKET_UDP == dwSocketType)
		dwSocketType = SOCK_DGRAM;
	else if (PGM_SOCKET_ICMP == dwSocketType)
		dwSocketType = SOCK_RAW;
	else
		dwSocketType = -1;

#ifdef WIN32
	int s = socket(AF_INET, dwSocketType, 0);
#else
	int s = socket(PF_INET, dwSocketType, 0);
#endif

#ifdef  WIN32
#else
	//	fcntl( s, F_SETFL, O_NONBLOCK);
#endif

	if (INVALID_SOCKET == s || 0 == s)
	{
		return 0;
	}
	else
	{
		DWORD i = 1;
		Attach(s);

		return 1;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
int SocketOperation::Bind(DWORD dwLocalIP, DWORD dwLocalPort)
{
	if (-1 != dwLocalIP)
		m_dwLocalIP = dwLocalIP;

	if (-1 != dwLocalPort)
		m_dwLocalPort = dwLocalPort;

	sockaddr_in stServerAddr = {0};

	stServerAddr.sin_family				= AF_INET;

#ifdef WIN32
	stServerAddr.sin_addr.S_un.S_addr	= m_dwLocalIP;
	stServerAddr.sin_port				= htons((WORD) m_dwLocalPort);
#else
	stServerAddr.sin_addr.s_addr	= m_dwLocalIP;
	stServerAddr.sin_port			= htons(m_dwLocalPort); 

#endif

	m_dwSocket=socket(AF_INET,SOCK_DGRAM,0);
	//#ifdef WIN32
	//#else
	//int val = 0;
	//val = fcntl(m_dwSocket,F_GETFL,0) ;
	//fcntl(m_dwSocket,F_SETFD,val|FD_CLOEXEC);
	//#endif
	SetError(bind(m_dwSocket, (sockaddr*) &stServerAddr, sizeof(stServerAddr)));
	/*   printf("%d\n",GetLastError());*/
	if (SOCKET_ERROR == GetLastError())
	{
		SetError(getsockerror());

		return errno;
	}
	else
	{
		if (0 == m_dwLocalPort)
		{

#ifdef WIN32
			int iLen = sizeof(stServerAddr);
#else
			socklen_t iLen = sizeof(stServerAddr);
#endif
			getsockname(m_dwSocket, (sockaddr*) &stServerAddr, &iLen);
			m_dwLocalPort = (DWORD) htons(stServerAddr.sin_port);
		}
		return errno;
	}
}
/************************************************************************/
/* added by zw at 20090310                                                                     */
/************************************************************************/

char * SocketOperation::GetRemoteIpAndPort()
{
	in_addr   aa;   
	aa.s_addr   =  m_dwRemoteIP ;   
	return inet_ntoa(aa); 
}


/************************************************************************/
/* ip地址转换                                                                     */
/************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////
void SocketOperation::SetRemote(DWORD dwIP, DWORD dwPort)
{
	if (-1 != dwIP)
		m_dwRemoteIP = dwIP;

	if (-1 != dwPort)
		m_dwRemotePort = dwPort;

	m_bBindRemote = TRUE;

	m_dwRemoteIPMask = 0xFFFFFFFF;
	PGM_IP_ADDRESS *pAddr = (PGM_IP_ADDRESS *) &m_dwRemoteIP;
	PGM_IP_ADDRESS *pAddrMask = (PGM_IP_ADDRESS *) &m_dwRemoteIPMask;
	if (255 == pAddr->ayIP[3])
	{
		if (255 == pAddr->ayIP[2])
		{
			if (255 == pAddr->ayIP[1])
			{
				if (255 == pAddr->ayIP[0])
				{
					pAddrMask->ayIP[0] = 0;
					m_bBindRemote = FALSE;
				}
				pAddrMask->ayIP[1] = 0;
			}
			pAddrMask->ayIP[2] = 0;
		}
		pAddrMask->ayIP[3] = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
BOOL SocketOperation::Listen()
{
	SetError(ERR_PGM_OK);

	if (PGM_SOCKET_TCP == m_dwSocketType && 0 != m_dwSocket)
	{
		if (listen(m_dwSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			SetError(getsockerror());
			return FALSE;
		}
		else
		{
			m_bDataSocket = FALSE;
			return TRUE;
		}
	}
	else
		return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////
BOOL SocketOperation::Accept(SocketOperation *poClientSocket)
{
	if (NULL == poClientSocket)
	{
		SetError(ERR_PGM_PARAMETER);
		return FALSE;
	}
	if (0 == m_dwSocket)
	{
		SetError(ERR_PGM_NO_INIT);
		return FALSE;
	}

	SetError(ERR_PGM_OK);


	fd_set stFDS = {0};
	FD_SET(m_dwSocket, &stFDS);
	timeval stTimeOut;
	stTimeOut.tv_sec = 0;
	stTimeOut.tv_usec = 500;

	int iRet = select(0, &stFDS, NULL, NULL,&stTimeOut);

	if (1 != iRet)
	{
		return FALSE;
	}


	SOCKET s = 0;
	sockaddr_in stClientAddr = {0};

#ifdef WIN32
	int iAddrLen = sizeof(stClientAddr);
#else
	socklen_t iAddrLen = sizeof(stClientAddr);
#endif

	s = accept(m_dwSocket, (sockaddr*) &stClientAddr, &iAddrLen);

	if (INVALID_SOCKET == s || 0 == s)
	{
		SetError(getsockerror());

		return FALSE;
	}
	else
	{

#ifdef WIN32
		m_dwRemoteIP = stClientAddr.sin_addr.S_un.S_addr;
#else
		m_dwRemoteIP = stClientAddr.sin_addr.s_addr;
#endif

		m_dwRemotePort = (DWORD) htons(stClientAddr.sin_port);

		poClientSocket->Attach(s);

		poClientSocket->SetLocal(m_dwLocalIP, m_dwLocalPort);
		poClientSocket->SetRemote(m_dwRemoteIP, m_dwRemotePort);

		return TRUE;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
BOOL SocketOperation::Connect(DWORD dwRemoteIP, DWORD dwRemotePort)
{
	if (m_dwSocket == 0)
		return FALSE;

	if (-1 != dwRemoteIP)
		m_dwRemoteIP = dwRemoteIP;

	if (-1 != dwRemotePort)
		m_dwRemotePort = dwRemotePort;

	sockaddr_in stServerAddr = {0};

	stServerAddr.sin_family				= AF_INET;
	stServerAddr.sin_port				= htons((WORD) m_dwRemotePort);

#ifdef WIN32
	stServerAddr.sin_addr.S_un.S_addr	= m_dwLocalIP;
#else
	stServerAddr.sin_addr.s_addr		= m_dwLocalIP;
#endif

	SetError(connect(m_dwSocket, (sockaddr*) &stServerAddr, sizeof(stServerAddr)));

	if (SOCKET_ERROR == GetLastError()) 
	{
		SetError(getsockerror());

		return FALSE;
	}
	else
	{
		m_bConnect = TRUE;

		SetError(ERR_PGM_OK);

		return TRUE;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
BOOL SocketOperation::DisConnect()
{
	if (0 != m_dwSocket)
	{
		shutdown(m_dwSocket, 0);
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////
DWORD SocketOperation::SendData(PCVOID pBuf, DWORD dwLen)
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
				Sleep(1);

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
			// 				stRemoteAddr.sin_port = htons((WORD) m_dwRemotePort);
			// 			}
			//             if (dwLen = 28)
			// 			{
			// #ifdef WIN32
			// 				stRemoteAddr.sin_addr.S_un.S_addr	= m_dwRemoteIP/*m_dwLocalIP*/;
			//                 stRemoteAddr.sin_port = htons((WORD) m_dwRemotePort);
			// #else
			// 				stRemoteAddr.sin_addr.s_addr	= m_dwRemoteIP/*m_dwLocalIP*/;
			// 				stRemoteAddr.sin_port = htons(m_dwRemotePort); 
			// #endif
			// 			} 
			// 			else
			// 			{
			// #ifdef WIN32
			// 				stRemoteAddr.sin_addr.S_un.S_addr	= m_dwRemoteHAIP/*m_dwLocalIP*/;
			//                 stRemoteAddr.sin_port = htons((WORD) m_dwRemoteHAPort);
			// #else
			// 				stRemoteAddr.sin_addr.s_addr	= m_dwRemoteHAIP/*m_dwLocalIP*/;
			// 				stRemoteAddr.sin_port = htons(m_dwRemoteHAPort); 
			// #endif
			// 			}
#ifdef WIN32
			stRemoteAddr.sin_addr.S_un.S_addr	= m_dwRemoteIP/*m_dwLocalIP*/;
			stRemoteAddr.sin_port = htons((WORD) m_dwRemotePort);
#else
			stRemoteAddr.sin_addr.s_addr	= m_dwRemoteIP/*m_dwLocalIP*/;
			stRemoteAddr.sin_port = htons(m_dwRemotePort); 
#endif
			int iLen = 0;
			for (int i = 0; i < (int) dwLen; i += 32 * 1024)
			{
				// To Limit The Trans-speed To 1MB/s (8 Mbps)
				if (i > 0)
					Sleep(30);

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

DWORD SocketOperation::SendData(PCVOID pBuf, DWORD dwLen, DWORD Type)
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
				Sleep(1);

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
			// 				stRemoteAddr.sin_port = htons((WORD) m_dwRemotePort);
			// 			}
#ifdef WIN32
				stRemoteAddr.sin_addr.S_un.S_addr	= m_dwRemoteIP/*m_dwLocalIP*/;
				stRemoteAddr.sin_port = htons((WORD) m_dwRemotePort);
#else
				stRemoteAddr.sin_addr.s_addr	= m_dwRemoteIP/*m_dwLocalIP*/;
				stRemoteAddr.sin_port = htons(m_dwRemotePort); 
#endif
			

			int iLen = 0;
			for (int i = 0; i < (int) dwLen; i += 32 * 1024)
			{
				// To Limit The Trans-speed To 1MB/s (8 Mbps)
				if (i > 0)
					Sleep(30);

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
///////////////////////////////////////////////////////////////////////////////////////
//DWORD SocketOperation::RecvData(IPGMVar *poVar)
//{
//	int src = 0;
//	int dst = 0;
//	int evt = 0;
//
//	if (NULL == poVar)
//	{
//		SetError(ERR_PGM_PARAMETER);
//		return -1;
//	}
//	if (0 == m_dwSocket)
//	{
//		SetError(ERR_PGM_NO_INIT);
//		return -1;
//	}
//
//	SetError(0);
//
//	fd_set stFDS = {0};
//	FD_SET(m_dwSocket, &stFDS);
//	timeval stTimeOut;
//	stTimeOut.tv_sec = 5000 / 1000;
//	stTimeOut.tv_usec = 5000 % 1000;
//
//	int iRet = select(m_dwSocket+1, &stFDS, NULL, NULL, &stTimeOut);
//	//int iRet = select(m_dwSocket+1, &stFDS, NULL, NULL, NULL);
//
//	if (0 == iRet)
//	{
//		return 0;
//	}
//	else if (1 != iRet)
//	{
//		SetError(getsockerror());
//		return -1;
//	}
//
//	int iBufLen = poVar->GetFreeSize();
//	if (iBufLen <= 32 * 1024)
//		poVar->IncreaseBufferSize(32 * 1024 - iBufLen);
//
//	char *pBuf = (char*) poVar->GetFreeBuffer();
//	iBufLen = poVar->GetFreeSize();
//
//	if (PGM_SOCKET_UDP == m_dwSocketType)
//	{
//		sockaddr_in stAddr = {0};
//
//#ifdef WIN32
//		int iAddrLen = sizeof(stAddr);
//#else
//		socklen_t iAddrLen = sizeof(stAddr);
//#endif
//
//		iRet = recvfrom(m_dwSocket, pBuf, iBufLen, 0, (sockaddr*) &stAddr, &iAddrLen);
//
//		if (iRet > 0)
//		{
//
//#ifdef WIN32
//			DWORD dwRemoteIP = stAddr.sin_addr.S_un.S_addr;
//			DWORD dwRemotePort = (DWORD) htons(stAddr.sin_port);
//#else
//			DWORD dwRemoteIP = stAddr.sin_addr.s_addr;
//			DWORD dwRemotePort = htons(stAddr.sin_port); 
//
//#endif
//
//			if (m_bBindRemote)
//			{
//				// Reject Unauthorized IP Or Port
//				if (0 != m_dwRemotePort && dwRemotePort != m_dwRemotePort)
//					iRet = 0;
//				else if (0 != ((m_dwRemoteIP ^ dwRemoteIP) & m_dwRemoteIPMask))
//					iRet = 0;
//			}
//
//			ZMsgHead *mbuf = new ZMsgHead((char*)poVar->GetBuffer(),iRet);
//			if(mbuf->UnpkPost(TRUE, &src, &dst, &evt))  //锟斤拷头
//			{
//				Z_SAFE_DELETE(mbuf);
//				return ERR_MESSAGE_UNPACKET;
//			}
//
//			if (evt == ZG_EVT_HA_EXREQ || evt == ZG_EVT_HA_RESUMREQ)
//			{
//				m_dwRemoteHAIP = dwRemoteIP;
//				m_dwRemoteHAPort = dwRemotePort; 
//			}
//			else
//			{
//				m_dwRemoteIP = dwRemoteIP;
//				m_dwRemotePort = dwRemotePort; 
//			}
//
//
//			// 			if (iRet > 0)
//			// 			{
//			// 				m_dwRemoteIP = dwRemoteIP;
//			// 				m_dwRemotePort = dwRemotePort;
//			// 			}
//			// 			if (iRet == 113 || iRet == 142)
//			// 			{
//			//                 m_dwRemoteIP = dwRemoteIP;
//			//  				m_dwRemotePort = dwRemotePort;
//			// 			} 
//			// 			else
//			// 			{
//			// 				m_dwRemoteHAIP = dwRemoteIP;
//			//  				m_dwRemoteHAPort = dwRemotePort;
//			// 
//			// 			}
//			Z_SAFE_DELETE(mbuf); 
//		}
//	}
//	else
//	{
//		iRet = recv(m_dwSocket, pBuf, iBufLen, 0);
//	}
//
//	if (iRet > 0)
//	{
//		poVar->SetBufferLen(poVar->GetBufferLen() + iRet);
//	}
//	else
//	{
//		SetError(getsockerror());
//	}
//
//
//	return iRet;
//}
