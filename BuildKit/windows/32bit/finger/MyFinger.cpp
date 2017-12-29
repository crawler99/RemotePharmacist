#include "StdAfx.h"
#include "MyFinger.h"

BOOL CFinger::LoadLib()
{
	hDllInst = LoadLibrary("fpengine.dll");
	if (NULL == hDllInst)
	{
		return FALSE;
	}

	m_DrawFct = (fpDrawImage)GetProcAddress(hDllInst,"DrawImage");
	m_SetHwndFct = (fpSetMsgMainHandle)GetProcAddress(hDllInst,"SetMsgMainHandle");
	m_SetDispFct = (fpSetDispOption)GetProcAddress(hDllInst,"SetDispOption");
	m_SetTmoFct = (fpSetTimeOut)GetProcAddress(hDllInst,"SetTimeOut");
	m_GetEnlFpCharFct = (fpGetFpCharByEnl)GetProcAddress(hDllInst,"GetFpCharByEnl");
	m_GetGenFpCharFct = (fpGetFpCharByGen)GetProcAddress(hDllInst,"GetFpCharByGen");
	m_MatchTempFct = (fpMatchTemplateOne)GetProcAddress(hDllInst,"MatchTemplateOne");
	m_CloseFct = (fpCloseDevice)GetProcAddress(hDllInst,"CloseDevice");
	m_OpenFct = (fpOpenDevice)GetProcAddress(hDllInst,"OpenDevice");
	m_LinkFct = (fpLinkDevice)GetProcAddress(hDllInst,"LinkDevice");
	m_EnlFpCharFct = (fpEnrolFpChar)GetProcAddress(hDllInst,"EnrolFpChar");
	m_GenFpcharFct = (fpGenFpChar)GetProcAddress(hDllInst,"GenFpChar");
}

void CFinger::UnloadLib()
{
	if (hDllInst)
		FreeLibrary(hDllInst);
}

BOOL  CFinger::OpenDev(HWND hwnd)
{
	int ret;

	m_SetHwndFct(hwnd);
	m_SetDispFct(FALSE,1.0,RGB(255,255,255),RGB(255,0,0),0);
	m_SetTmoFct(30.0);

	ret = m_OpenFct(0, 0, 0);
	if (TRUE != ret)
	{
		return FALSE;
	}

	ret = m_LinkFct();
	if (TRUE != ret)
	{
		m_CloseFct();
		return FALSE;
	}

	return TRUE;
}

BOOL CFinger::EnrolMask()
{
	m_EnlFpCharFct();

	return TRUE;
}

BOOL CFinger::GenMask()
{
	m_GenFpcharFct();
	return TRUE;
}

BOOL CFinger::GetEnmask(unsigned char *pucBuff, int *pslLen)
{
	m_GetEnlFpCharFct(pucBuff, pslLen);
	return TRUE;
}
BOOL CFinger::GetGenMask(unsigned char *pucBuff, int *pslLen)
{
	m_GetGenFpCharFct(pucBuff, pslLen);
	return TRUE;
}
BOOL CFinger::ComMask(unsigned char *pucEnBuff, int slEnLen, unsigned char *pucGenBuff)
{
	int ret;

	ret = m_MatchTempFct(pucGenBuff, pucEnBuff, slEnLen);
	if (ret > 100)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
BOOL CFinger::DrawImg(HDC hdc, int x, int y)
{
	m_DrawFct(hdc, x, y);

	return TRUE;
}