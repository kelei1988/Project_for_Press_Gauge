#include "Include.h"
/*******************************************
�������ƣ�Display_SEG
��    �ܣ��������ڰ����жϳ�����ʱ��
          ʹ�ô˳����Ա�������ܳ�����ʾ
��    ������
����ֵ  ����
********************************************/
void Display_SEG()
{
    P1OUT &=0x1f;                    //P1.5-P1.7�������λѡ�� 
    P2OUT = SEG[DispBuf[cnt-5]];     //�����ѡ�ź�
    P1OUT |= (1 << cnt);  
       cnt++;                        //λ����������1~4֮��ѭ��
    if(cnt == 8) cnt = 5;
    
     WDTCTL = WDTPW+WDTCNTCL;//  ������Ź�
}

//��ʾ���� ��������
void xianshi_shuzi(int x)
 {   
     x = x*10;
     if(x>=100)
     {
         DispBuf[0] = x/100+16;  //�����������ʾ
         DispBuf[1] = (x/10)%10;
         DispBuf[2] = x%10;
     }
     else
     {
         DispBuf[0] = 16;  //�����������ʾ
         DispBuf[1] = (x/10)%10;
         DispBuf[2] = x%10;
     }
 }

void xianshi_errcode(int error_code)
{
     DispBuf[0] = error_code/100;  //�����������ʾ
     DispBuf[1] = (error_code/10)%10;
     DispBuf[2] = error_code%10;
}