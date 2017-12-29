#pragma once

#include "StdAfx.h"
#include "protocol.h"
#include "fpengine.h"
//#include <commctrl.h>

typedef int (WINAPI *fpOpenDevice)(int comnum,int nbaud,int style);
typedef int (WINAPI *fpLinkDevice)();
typedef int (WINAPI *fpCloseDevice)();
typedef void (WINAPI *fpSetMsgMainHandle)(HWND hWnd);
typedef BOOL (WINAPI *fpSetDispOption)(BOOL bSet,double iZoom,COLORREF clrBack ,COLORREF clrForce,int nThick);
typedef void (WINAPI *fpSetTimeOut)(double itime);
typedef int (WINAPI *fpDrawImage)(HDC hdc,int left,int top);
typedef void (WINAPI *fpEnrolFpChar)();
typedef void (WINAPI *fpGenFpChar)();
typedef BOOL (WINAPI *fpGetFpCharByEnl)(BYTE * fpbuf,int * fpsize);
typedef BOOL (WINAPI *fpGetFpCharByGen)(BYTE * tpbuf,int * tpsize);
typedef int (WINAPI *fpMatchTemplateOne)(BYTE *pSrcData,BYTE *pDstData,int nDstSize);

#define WM_NCLMESSAGE	WM_USER+120
#define FPM_DEVICE		0x01	//设备
#define FPM_PLACE		0x02	//请按手指
#define FPM_LIFT		0x03	//请抬起手指
#define FPM_CAPTURE		0x04	//采集图像完成
#define FPM_GENCHAR		0x05	//采集特征点
#define FPM_ENRFPT		0x06	//登记指纹
#define FPM_NEWIMAGE	0x07	//新的指纹图像
#define FPM_TIMEOUT		0x08
#define FPM_IMGVAL		0x09

class CFinger
{
private:
	HINSTANCE hDllInst;
	fpOpenDevice m_OpenFct;
	fpLinkDevice m_LinkFct;
	fpCloseDevice m_CloseFct;
	fpSetMsgMainHandle m_SetHwndFct;
	fpSetDispOption m_SetDispFct;
	fpSetTimeOut m_SetTmoFct;
	fpDrawImage  m_DrawFct;
	fpEnrolFpChar m_EnlFpCharFct;
	fpGenFpChar   m_GenFpcharFct;
	fpGetFpCharByEnl m_GetEnlFpCharFct;
	fpGetFpCharByGen m_GetGenFpCharFct;
	fpMatchTemplateOne m_MatchTempFct;
public:
	BOOL  LoadLib();
	void  UnloadLib();
	BOOL  OpenDev(HWND hwnd);
	BOOL  EnrolMask();
	BOOL  GetEnmask(unsigned char *pucBuff, int *pulLen);
	BOOL  GenMask();
	BOOL  GetGenMask(unsigned char *pucBuff, int *puLen);
	BOOL  ComMask(unsigned char *pucEnBuff, int slEnLen, unsigned char *pucGenBuff);
	BOOL  DrawImg(HDC hdc, int x, int y);
};
