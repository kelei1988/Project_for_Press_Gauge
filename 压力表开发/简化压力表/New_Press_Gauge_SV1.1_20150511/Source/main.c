#include "Include.h"
#define PER  1450
//20141124 �����������
//20150115
void main( void )
{
  Init_ALL();
  account = 0;
  z_flag=0;
  keypad_Read_Setting_FormFlash(); //����flash��ʼ״̬��ֵ�ж��ǵ�һ�����س�����������λ   
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
          WDTCTL = WDTPW+WDTCNTCL;//  ������Ź�
          switch(Status)
          {
            case 0x00:Display_Pressure();break;   //��ʾѹ��
            case 0x01:Pressure_Calibrate();break; //�궨����ѹ����
            case 0x08:Correct_Press_Value();break;//ѹ������Ư������
            case 0x09:Correct_Zero_Value();break; //ѹ�����Ư������
           // case 0x0a:Charge_Sample_Speed();break;//�޸Ĳ����ٶ�
            default:Display_Pressure();break; 
          }
      }
}


