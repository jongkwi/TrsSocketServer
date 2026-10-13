// CAlramWeb.cpp : implementation file
//

#include "stdafx.h"
#include "TrsSocketServer.h"
#include "AlramWeb.h"
#include <afxinet.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlramWeb

#define SIGINT          2       /* interrupt */



CAlramWeb::CAlramWeb()
{
}

CAlramWeb::~CAlramWeb()
{
}




UINT AlarmWorkerThread(LPVOID lpvThreadParm)
{
	CAlramWeb* pDlg = (CAlramWeb*)lpvThreadParm;
	ALRAM_OBJ alObj;
	int i = 0;
	for(;;){
		if(pDlg->m_nAlarmTh_Stop == TRUE) break;
		memset(&alObj, 0x00, sizeof(ALRAM_OBJ));				
		if(pDlg->AlarmList_Get(alObj) == TRUE){
			

			if(alObj.ao_type == AR_EMAIL){
				pDlg->AlarmList_SendEmail(alObj);
			}else{
				pDlg->AlarmList_Show(alObj);
			}			

			for(i = 0; i < 5; i++){
				Sleep(50);
				if(pDlg->m_nAlarmTh_Stop == TRUE) break;
			}
		}else{
			for(i = 0; i < 10; i++){
				Sleep(50);
				if(pDlg->m_nAlarmTh_Stop == TRUE) break;
			}			
		}	

		if(pDlg->m_nAlarmTh_Stop == TRUE) break;
		Sleep(50);
		if(pDlg->m_nAlarmTh_Stop == TRUE) break;
		Sleep(50);		
	}	
	return FALSE;
}




void CAlramWeb::AlarmTh_Start()
{
	InitializeCriticalSection(&m_csEventIndex);

	m_nAlarmTh_Stop = FALSE;
	m_pAlarmThread = AfxBeginThread(AlarmWorkerThread, (LPVOID)this);

	int j = 0;
}



void CAlramWeb::AlarmTh_Terminate()
{
	if(m_pAlarmThread == NULL ) return;
	m_nAlarmTh_Stop = TRUE;
	Sleep(100);

	// 스레드 종료
	if(WaitForSingleObject(m_pAlarmThread->m_hThread, 3000)  == WAIT_TIMEOUT){
		TerminateThread(m_pAlarmThread->m_hThread, SIGINT); 
		delete m_pAlarmThread;
		m_pAlarmThread = NULL;
	}
	AlarmList_RemoveAll();
	DeleteCriticalSection(&m_csEventIndex);

}




int CAlramWeb::AlertList_Push(char* szMsg, int nAlertType)
{
	if(m_pAlarmThread == NULL ) return TRUE;

	

	ALRAM_OBJ* pAlObj = new ALRAM_OBJ;
	pAlObj->ao_type = AR_ALERT;
	pAlObj->alert_type = nAlertType;

	if(strlen(szMsg) < 500)
		strcpy(pAlObj->ao_msg, szMsg);


	EnterCriticalSection(&m_csEventIndex);

	m_AlarmList.AddTail(pAlObj);

	LeaveCriticalSection(&m_csEventIndex);

	return FALSE;
}



int CAlramWeb::AlertList_EmailPush(EMAIL_SITE_USERS &emailObj)
{
	if(m_pAlarmThread == NULL ) return TRUE;

	

	ALRAM_OBJ* pAlObj = new ALRAM_OBJ;
	pAlObj->ao_type = AR_EMAIL;

	strcpy(pAlObj->szEMail_to, emailObj.email_to);
	strcpy(pAlObj->szEMail_from, emailObj.email_from);
	strcpy(pAlObj->szEMail_title, emailObj.email_title);
	strcpy(pAlObj->szEMail_Msg, emailObj.email_msg);
	strcpy(pAlObj->szEMail_Url, emailObj.email_url);


	EnterCriticalSection(&m_csEventIndex);

	m_AlarmList.AddTail(pAlObj);

	LeaveCriticalSection(&m_csEventIndex);

	return FALSE;
}


int CAlramWeb::AlarmList_Get(ALRAM_OBJ &alarmData)
{
	int nResult = FALSE;
	ALRAM_OBJ* pAlObj = NULL;
	EnterCriticalSection(&m_csEventIndex);

	int listCnt = m_AlarmList.GetCount();
	if(listCnt > 0){
		POSITION pos = m_AlarmList.GetHeadPosition();
		if(pos != NULL){
			pAlObj = (ALRAM_OBJ*)m_AlarmList.GetAt(pos);
			if(pAlObj != NULL){
				memcpy(&alarmData, pAlObj, sizeof(ALRAM_OBJ));	
				delete pAlObj;
				pAlObj = NULL;
				nResult = TRUE;
			}
			m_AlarmList.RemoveAt(pos);
		}
	}
	LeaveCriticalSection(&m_csEventIndex);

	return nResult;
}




void CAlramWeb::AlarmList_RemoveAll()
{
	int k = 0;
	int listCnt = m_AlarmList.GetCount();
	POSITION pos = m_AlarmList.GetTailPosition();	
	for(k = 0; k < listCnt; k++){
		ALRAM_OBJ* rnData = NULL;
		rnData = (ALRAM_OBJ*)m_AlarmList.GetAt(pos);						
		if (rnData != NULL){		
			delete rnData;
			rnData = NULL;						
			m_AlarmList.RemoveAt(pos);
		}
		m_AlarmList.GetPrev(pos);		
		if(pos == NULL)			
			break;
	}
	listCnt = m_AlarmList.GetCount();
	if(listCnt > 0)
		m_AlarmList.RemoveAll();
}



void CAlramWeb::AlarmList_Show(ALRAM_OBJ &alram_Obj)
{
	CString strUrl = "http://onpia2.cafe24.com/onpia_push_system/send_push.php";
	CString strOutMsg;	
	CString strMsg(alram_Obj.ao_msg);
	CString strEn = m_urlEncode.Utf8_UrlEncode(strMsg);

	CString strSendMsg;
	strSendMsg.Format("msg=%s", strEn);
	
	RequestPost((LPCTSTR)strUrl, strSendMsg, strOutMsg);


	return;
/*
	char szGetData[5001] = "";
	int nMaxCount = 5000;
	char szUrl[2048] = "";
	CString strMessage(alram_Obj.ao_msg);
	CString strEncodeData = m_urlEncode.Utf8_UrlEncode(strMessage);


	sprintf(szUrl, "http://onpia2.cafe24.com/onpia_push_system/send_push.php?msg=%s", strEncodeData);
//	sprintf(szUrl, "http://onpia2.cafe24.com/onpia_push_system/test_push.php?msg=%s", strEncodeData);

	BOOL bError = FALSE;		
    CInternetSession m_Session("onpia_alram");
    CHttpConnection* m_pHttpConnection=NULL;
    CHttpFile* m_pHttpFile = NULL;	 
	char szLine[2048] = "";
			
    try{
		m_Session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, 5000);		
		m_Session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 5000);		       
		m_pHttpConnection = m_Session.GetHttpConnection(szUrl,(INTERNET_PORT)80, NULL, NULL);        
	   if(m_pHttpConnection == NULL)
            throw CString("Http connection failed");				
        m_pHttpFile = (CHttpFile*)m_Session.OpenURL(szUrl,  1,INTERNET_FLAG_TRANSFER_ASCII|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD);
        if(m_pHttpFile == NULL){
            m_pHttpFile->Close();
            delete m_pHttpFile;
            throw CString("HttpFile connection failed");
        }
    }catch(CInternetException *pEx){
        if(m_pHttpFile){
            m_pHttpFile->Close();
            delete m_pHttpFile;
        }
        if(m_pHttpConnection){
            m_pHttpConnection->Close();
            delete m_pHttpConnection;
        }
        TCHAR lpszErrorMessage[255] = "";
        pEx->GetErrorMessage(lpszErrorMessage, 255);		
        pEx->Delete();
		bError = TRUE;
    }catch(CString e){		
		bError = TRUE;
	}
	if(bError == TRUE) return;
	
	DWORD dwGetByteCount = 0;
    while(m_pHttpFile->ReadString(szLine, 500) != NULL){ 
		dwGetByteCount += strlen(szLine);
		if(dwGetByteCount >= nMaxCount) break;
		strcat(szGetData, szLine);			    	    
	}	
    if(m_pHttpFile){
        m_pHttpFile->Close();
        delete m_pHttpFile;
    }    
    if(m_pHttpConnection){
        m_pHttpConnection->Close();
        delete m_pHttpConnection;
    }					
	return;	
	*/
}



void CAlramWeb::AlarmList_SendEmail(ALRAM_OBJ &alram_Obj)
{

	CTrsSocketServerApp* pApp = (CTrsSocketServerApp*)AfxGetApp();

	CString strUrl = "http://www.onpia.co.kr/onpiaMailInput.do";

	CString strMsg;
	CString strEnTo = m_urlEncode.Utf8_UrlEncode(alram_Obj.szEMail_to);
	CString strEnTitle = m_urlEncode.Utf8_UrlEncode(alram_Obj.szEMail_title);
	CString strEnMsg = m_urlEncode.Utf8_UrlEncode(alram_Obj.szEMail_Msg);

	strMsg.Format("to=%s&cc=%s&subject=%s&content=%s", 
		strEnTo, 
		"", 
		strEnTitle,
		strEnMsg);
	

	CString strOutMsg;	
	int nCode = RequestPost((LPCTSTR)strUrl, strMsg, strOutMsg);



	char szLog[1024] = "";
	sprintf(szLog, "AlarmList_SendEmail-S rul:%s, data:%s", strUrl, strMsg);
	pApp->AlarmLogging(szLog);

	sprintf(szLog, "AlarmList_SendEmail-R code:%d,  data:%s ", nCode, strOutMsg);
	pApp->AlarmLogging(szLog);

}


BOOL CAlramWeb::RequestPost(LPCTSTR lpUrl, CString strSendMsg, CString &strOutMsg)
{
  
	BOOL bRet = FALSE;    
	DWORD dwSearviceType;  
	CString strServer, strObject;
  
	INTERNET_PORT nPort;
    
	if(!AfxParseURL(lpUrl, dwSearviceType, strServer, strObject, nPort))
	{   	
		//ASSERT(0);	
		return -1;
	}
	
	
	CString strReciveMessage("");  
	DWORD dwReadSize;
  
	CInternetSession Session;
	CHttpConnection* pServer = NULL;
	CHttpFile *pFile = NULL;
  	 
	CString strHeader = "Content-Type: application/x-www-form-urlencoded\r\n";	 
	try	 
	{
		pServer = Session.GetHttpConnection(strServer, nPort);	   
		pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject);   	   
		pFile->SendRequest(strHeader, (LPVOID)strSendMsg.GetBuffer(strSendMsg.GetLength()), strSendMsg.GetLength());	   
		strSendMsg.ReleaseBuffer();	  
	}catch (CInternetException* e)	  
	{
	   TCHAR szError[255];
	   e->GetErrorMessage(szError,255);
	   e->Delete();
	   return -2;	  
	}
    
	  
	dwReadSize = pFile->Read(strReciveMessage.GetBuffer(pFile->GetLength()), pFile->GetLength());
	strReciveMessage.ReleaseBuffer();
	
	if(dwReadSize != (DWORD)strReciveMessage.GetLength())
	   bRet = FALSE;	  
	else
	   bRet = TRUE;  	  
	strOutMsg = strReciveMessage;
  	  
	if(pFile)
	{
	   pFile->Close();
	   delete pFile;
	   pFile = NULL;	  
	}
  	  
	if(pServer)
	{
	   pServer->Close();
	   delete pServer;
	   pServer = NULL;
	}  	  
	Session.Close();  	  
	return bRet;
	 
}
