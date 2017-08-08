#ifndef GLOBAL_H
#define GLOBAL_H

#define UN_USE_DEV 1
#undef UN_USE_DEV


#define mWidth  320
#define mHeight 256

typedef unsigned char uint8_t;
typedef unsigned char UCHAR;
typedef unsigned char BYTE;
typedef unsigned short USHORT;
typedef unsigned long ULONG;
typedef unsigned long DWORD;

typedef struct{USHORT x,y;}CPoint;  //left-up:0,0
typedef struct{USHORT cx,cy;}CSize;
typedef ULONG TEMPTURE;  //*1024;

struct cmd_req{
    USHORT cmd;  //=1
    USHORT attach;//fujiazi
    CPoint ul;//-1,-1 zhongxinweizhi  ul.x,ul.y,sz.x,sz.y  wangluozijiexu
    CSize  sz;
    USHORT checksum;
};


struct cmd_resp{
    USHORT cmd; //=1
    USHORT attach;
    TEMPTURE ScaleLow,ScaleHigh,AreaMin,AreaMax,AreaAvg;  //wangluozijiexu
    USHORT checksum;
};


typedef void (*FUNCPTR)(...);

extern int exit_GEV();
extern int init_GEV();
extern int AcquireImages( uint8_t *pFrameRGB );
extern void savebmp(UCHAR * pdata, char * bmp_file, int width, int height );

extern void* udp_server(void*);
extern void CalcArea(uint16_t* pBuf,CPoint m_AreaUL,CSize m_AreaSize);


extern int quit_flag;

extern CPoint m_AreaUL,m_SpotPT;
extern CSize  m_AreaSize;
extern USHORT m_MinLimit,m_MaxLimit;
extern USHORT m_AreaMin,m_AreaMax,m_AreaAvg;
extern USHORT m_ScaleLow,m_ScaleHigh;
extern USHORT m_SpotValue,m_CenterSpotValue;

extern int m_bAreaActive;

extern USHORT m_SrcIamge[mWidth*mHeight];

extern float m_FPS;

#endif // GLOBAL_H

