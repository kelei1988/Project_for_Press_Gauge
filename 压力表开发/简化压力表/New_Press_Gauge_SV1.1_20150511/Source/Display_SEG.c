#include "Include.h"
/*******************************************
函数名称：Display_SEG
功    能：当程序在按键中断程序中时候，
          使用此程序以保持数码管持续显示
参    数：无
返回值  ：无
********************************************/
void Display_SEG()
{
    P1OUT &=0x1f;                    //P1.5-P1.7（数码管位选） 
    P2OUT = SEG[DispBuf[cnt-5]];     //输出段选信号
    P1OUT |= (1 << cnt);  
       cnt++;                        //位计数变量在1~4之间循环
    if(cnt == 8) cnt = 5;
    
     WDTCTL = WDTPW+WDTCNTCL;//  清除看门狗
}

//显示数字 辅助整定
void xianshi_shuzi(int x)
 {   
     x = x*10;
     if(x>=100)
     {
         DispBuf[0] = x/100+16;  //更新数码管显示
         DispBuf[1] = (x/10)%10;
         DispBuf[2] = x%10;
     }
     else
     {
         DispBuf[0] = 16;  //更新数码管显示
         DispBuf[1] = (x/10)%10;
         DispBuf[2] = x%10;
     }
 }

void xianshi_errcode(int error_code)
{
     DispBuf[0] = error_code/100;  //更新数码管显示
     DispBuf[1] = (error_code/10)%10;
     DispBuf[2] = error_code%10;
}