#include "Include.h"
//����ad��Ӧѹ��
void Pressure_Calibrate()
{
 int i = 0;
 zhengding_enble=1;
  while(i!=11)//ֱ���������
  {
    P5OUT|=BIT2;
    P5OUT|=BIT4;
    P5OUT|=BIT5;
    P5OUT|=BIT6;
    P5OUT|=BIT7; 
   while(key_value!=0x11)//���м䰴������λѹ������0.05
     xianshi_shuzi(i);
   key_value = 0x00;
   zd_ad=0;
   while(zd_ad!=2)
   {};
   AD[i] =average_zd; 
   i++;
  }
    P5OUT|=BIT2;
    P5OUT|=BIT4;
    P5OUT|=BIT5;
    P5OUT|=BIT6;
    P5OUT|=BIT7;  
  zhengding_enble=0;
  //P5OUT|=BIT4;
  AD[11] = 2*AD[10]-AD[9];
  VIP_setting[0] = danwei_label;
  VIP_setting[1] = 0;
  VIP_setting[2] = 0;
  for(i=0;i<=11;i++)
  {
    VIP_setting[i+3] = AD[i] ;
  }
  
  VIP_setting[15] = 0x3D ; 
  VIP_setting[16] = danwei_label_f;
  VIP_setting[17] = 0;
  VIP_setting[18] = 0;
  VIP_setting[19] = Sample_Speed;
  keypad_Write_Setting_To_Flash();
  keypad_Read_Setting_FormFlash();
  Status = 0x00;
  key_value = 0x00;
 // danwei_label=3;
  time_count0=0;
  time_count1=0;
}


/*******************************************
�������ƣ�Display_Pressure()
��    �ܣ���ʾѹ��
��    ������
����ֵ  ����
********************************************/
void Display_Pressure()
{   
    int i=0;
    if(Sample_Speed_Count>10)//������ʾ�ٶ�
    {
      Average=pingjun;
      Sample_Speed_Count=0;
    }    
                               //����õ�PSI��ֵ
    if(danwei_label_f!=danwei_label)
    {
      danwei_label_f=danwei_label;
      keypad_Write_Setting_To_Flash();
    }
    if(Average>AD[0]) 
    {
     for(i=0;i<=10;i++)
      {
      if(Average>=AD[i]&&Average<AD[i+1])//������ż����ʾ0������
         PSI=(Average-AD[i])*145/(AD[i+1]-AD[i])+145*i; 
      }
    }
   else PSI=0;
  
   //PWM_Dac_Psi=PSI;
   //TACCR2=PWM_Dac_Psi*0.457444+159.487;//*0.567435; 0.10999
    if(PSI>1450) 
   {
         xianshi_errcode(888);  
         TACCR2=823;
   }
    else{
        P5OUT|=BIT5;
        P5OUT|=BIT6;
        P5OUT|=BIT7;
 
      if(danwei_label == 3)      //mpa               //���ݵ�λ��־����ʾ��ͬ��λ�µ�ѹ��
      {
        P5OUT&=~BIT7;
        xianshi_mpa();   
      }  
      else if(danwei_label == 2)//psi
      {
        P5OUT&=~BIT6;
        xianshi_psi(); 
      } 
      else if(danwei_label == 1)//kgcm
      {
        P5OUT&=~BIT5;
        xianshi_kgcm2();        
      } 
      else
      {
        P5OUT&=~BIT7;
        xianshi_mpa(); 
      }
    }
}

/*******************************************
�������ƣ�xianshi_mpa()
��    �ܣ���ѹ����ֵת��ΪMPA��λ��ʾ���룬
          ���������ʾ��������
��    ������
����ֵ  ����
********************************************/
void xianshi_mpa()  //psi to Mpa  0.40 400
{      
     Mpa = PSI*100/145;   //60000 
     DispBuf[0] =  Mpa/1000+16;  //�����������ʾ
     DispBuf[1] =  (Mpa/100)%10;
     DispBuf[2] = (Mpa/10)%10;
}
/*******************************************
�������ƣ� xianshi_kgcm2()
��    �ܣ���ѹ����ֵת��Ϊkgcm2��λ��ʾ���룬
          ���������ʾ��������
��    ������
����ֵ  ����
********************************************/
void xianshi_kgcm2()  //psi to kgcm2  4.22 422
{      
     Kgcm2 = PSI*50/71;  
     if(Kgcm2<1000)
     { 
       DispBuf[0] =  Kgcm2/100+16;  //�����������ʾ
       DispBuf[1] = (Kgcm2/10)%10;
       DispBuf[2] =  Kgcm2%10; 
     } 
     else    
     { 
       DispBuf[0] =  Kgcm2/1000;  //�����������ʾ
       DispBuf[1] = (Kgcm2/100)%10+16;
       DispBuf[2] = (Kgcm2/10)%10; 
     } 
     
}
/*******************************************
�������ƣ�xianshi_psi()
��    �ܣ���ѹ����ֵת��Ϊpsi��λ��ʾ���룬
          ���������ʾ��������
��    ������
����ֵ  ����
********************************************/
void xianshi_psi()  //��ʾPSI
{   
  //  if(PSI>1450)PSI=1450;  
  if(PSI<1000)
  { 
     DispBuf[0] =  PSI/100;  //�����������ʾ
     DispBuf[1] = (PSI/10)%10+16;
     DispBuf[2] =  PSI%10;
  }
  else
  {
     DispBuf[0] =  PSI/1000;  //�����������ʾ
     DispBuf[1] = (PSI/100)%10;
     DispBuf[2] =  (PSI/10)%10; 
  }
}
