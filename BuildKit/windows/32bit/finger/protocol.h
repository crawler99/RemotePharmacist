#ifndef  _PROTOCOL_Optics
#define  _PROTOCOL_Optics

#ifdef _OT_CPP_
#undef _OT_CPP_
#endif

#define FP_MAXWAITTIME 2000
#define DELAY_TIME     150
///////////////////错误返回码////////////////////
#define FP_OK                0x00
#define FP_COMM_ERR          0x01
#define FP_NO_FINGER         0x02
#define FP_GET_IMG_ERR       0x03
#define FP_FP_TOO_DRY        0x04
#define FP_FP_TOO_WET        0x05
#define FP_FP_DISORDER       0x06
#define FP_LITTLE_FEATURE    0x07
#define FP_NOT_MATCH         0x08
#define FP_NOT_SEARCHED      0x09
#define FP_MERGE_ERR         0x0a
#define FP_ADDRESS_OVER      0x0b
#define FP_READ_ERR          0x0c
#define FP_UP_TEMP_ERR       0x0d
#define FP_RECV_ERR          0x0e
#define FP_UP_IMG_ERR        0x0f
#define FP_DEL_TEMP_ERR      0x10
#define FP_CLEAR_TEMP_ERR    0x11
#define FP_SLEEP_ERR         0x12
#define FP_INVALID_PASSWORD  0x13
#define FP_RESET_ERR         0x14
#define FP_INVALID_IMAGE     0x15
#define FP_HANGOVER_UNREMOVE 0X17
 

///////////////缓冲区//////////////////////////////
#define CHAR_BUFFER_A          0x01
#define CHAR_BUFFER_B          0x02
#define MODEL_BUFFER           0x03

//////////////////串口号////////////////////////
#define COM1                   0x01
#define COM2                   0x02
#define COM3                   0x03

//////////////////波特率////////////////////////
#define BAUD_RATE_9600         0x00
#define BAUD_RATE_19200        0x01
#define BAUD_RATE_38400        0x02
#define BAUD_RATE_57600        0x03   //default
#define BAUD_RATE_115200       0x04

#ifdef _OT_CPP_
extern "C"
{
#endif
	BOOL WINAPI FPOpenDevice(int nDeviceType,int nPortNum,int nPortPara,int nPackageSize=2);
	BOOL WINAPI FPCloseDevice();
	///////////////////////////////////////////////
	//////             指令                  //////
	///////////////////////////////////////////////
	//检测手指并录取图像
	int WINAPI      FPGetImage(int nAddr);
	
	//根据原始图像生成指纹特征
	int  WINAPI    FPGenChar(int nAddr,int iBufferID);
	
	//精确比对CharBufferA与CharBufferB中的特征文件
	int WINAPI     FPMatch(int nAddr,int* iScore);
	
	//以CharBufferA或CharBufferB中的特征文件搜索整个或部分指纹库
	int  WINAPI    FPSearch(int nAddr,int iBufferID, int iStartPage, int iPageNum, int *iMbAddress);
	
	//将CharBufferA与CharBufferB中的特征文件合并生成模板存于ModelBuffer
	int  WINAPI    FPRegModule(int nAddr);
	
	//将ModelBuffer中的文件储存到flash指纹库中
	int  WINAPI    FPStoreChar(int nAddr,int iBufferID, int iPageID);
	
	//从flash指纹库中读取一个模板到ModelBuffer
	int  WINAPI     FPLoadChar(int nAddr,int iBufferID,int iPageID);
	
	//将特征缓冲区中的文件上传给上位机
	int WINAPI     FPUpChar(int nAddr,int iBufferID, unsigned char* pTemplet, int* iTempletLength);
	
	//从上位机下载一个特征文件到特征缓冲区
	int WINAPI     FPDownChar(int nAddr,int iBufferID, unsigned char* pTemplet, int iTempletLength);
	
	//上传原始图像
	int WINAPI     FPUpImage(int nAddr,unsigned char* pImageData, int* iImageLength);
	
	//下载原始图像
	int WINAPI     FPDownImage(int nAddr,unsigned char *pImageData, int iLength);
	
	//上传原始图像
	int  WINAPI     FPImgData2BMP(unsigned char* pImgData,const char* pImageFile);
	
	//下载原始图像
	
	int  WINAPI     FPGetImgDataFromBMP(const char *pImageFile,unsigned char *pImageData,int *pnImageLen);
	
	//删除flash指纹库中的一个特征文件
	int WINAPI     FPDelChar(int nAddr,int iStartPageID,int nDelPageNum);
	
	//清空flash指纹库
	int WINAPI     FPEmpty(int nAddr);
	
	//读参数表
	int WINAPI     FPReadParTable(int nAddr,unsigned char* pParTable);
	
	//休眠指令
	int WINAPI     FPPowerDown(int nAddr);
	
	//设置设备握手口令
	int WINAPI     FPSetPwd(int nAddr,unsigned char* pPassword);
	
	//验证设备握手口令
	int WINAPI     FPVfyPwd(int nAddr,unsigned char* pPassword);
	
	//系统复位，进入上电初始状态
	int WINAPI      FPReset(int nAddr);
		
	//读记事本
	int	WINAPI	    FPReadInfo(int nAddr,int nPage,unsigned char* UserContent);
	
	//写记事本
	int	WINAPI	    FPWriteInfo(int nAddr,int nPage,unsigned char* UserContent);
	
	//写模块寄存器－波特率设置
	int  WINAPI     FPSetBaud(int nAddr,int nBaudNum);
	//写模块寄存器－安全等级设置
	int  WINAPI     FPSetSecurLevel(int nAddr,int nLevel);
	//写模块寄存器－数据包大小设置
	int  WINAPI   FPSetPacketSize(int nAddr,int nSize);
	
	int  WINAPI    FPUpChar2File(int nAddr,int iBufferID, const char* pFileName);
	int  WINAPI     FPDownCharFromFile(int nAddr,int iBufferID, const char* pFileName);
	
	int  WINAPI FPGetRandomData(int nAddr,unsigned char* pRandom);
	int  WINAPI FPSetChipAddr(int nAddr,unsigned char* pChipAddr);
	
	int  WINAPI  FPDoUserDefine(int nAddr,int GPIO,int STATE);
	
	int  WINAPI  FPTemplateNum(int nAddr,int *iMbNum);

	//#define DEVICE_USB 0
	//#define DEVICE_COM 1
		
	//#define IMAGE_X 312
	//#define IMAGE_Y 230
	
	//根据错误号获取错误信息
	char* WINAPI   FPErr2Str(int nErrCode);

#ifdef _OT_CPP_
}
#endif

#endif                                                     