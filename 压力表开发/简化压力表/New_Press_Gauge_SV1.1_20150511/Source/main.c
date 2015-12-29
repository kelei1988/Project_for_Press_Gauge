#include "Include.h"
#define PER  1450
//20141124 基本测试完毕
//20150115
void main( void )
{
  Init_ALL();
  account = 0;
  z_flag=0;
  keypad_Read_Setting_FormFlash(); //根据flash初始状态数值判断是第一次下载程序还是重启复位   
    if(sh_dian!=0x3D)
    {
      danwei_label = 3;
      danwei_label_f = 3;
      for(a=0;a<=11;a++)
        AD[a]=1500+150*a;
      VIP_setting[19]=0;      
    }
      _BIS_SR(GIE);  
      secure1=0;
      secure2=0;
      secure3=0;
      zhengding_enble=0;
      time_count0=0;
      time_count1=0;
      Correct_Press_Staus=0;
      while(1)
      { 
          WDTCTL = WDTPW+WDTCNTCL;//  清除看门狗
          switch(Status)
          {
            case 0x00:Display_Pressure();break;   //显示压力
            case 0x01:Pressure_Calibrate();break; //标定各个压力点
            case 0x08:Correct_Press_Value();break;//压力整体漂移修正
            case 0x09:Correct_Zero_Value();break; //压力零点漂移修正
           // case 0x0a:Charge_Sample_Speed();break;//修改采样速度
            default:Display_Pressure();break; 
          }
      }
}


