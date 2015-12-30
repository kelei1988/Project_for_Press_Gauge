#include "Include.h"

/*******************************************
�������ƣ�ADC12ISR
��    �ܣ�ADC12���жϷ�����
��    ������
����ֵ  ����
********************************************/
#pragma vector=ADC_VECTOR
__interrupt void ADC12ISR (void)
{
  unsigned long sample_adc12;
  static unsigned int  Sample_count_q;
  static unsigned long Sample_value_q;
  static unsigned long Average_q;
  static unsigned long Sample_value;
  //static unsigned int  Sample_count;
  unsigned int ad_sample_count;
  ad_sample_count=1024;
  P1OUT &=0x1f;                    //P1.5-P1.7�������λѡ�� 
    P2OUT = SEG[DispBuf[cnt-5]];     //�����ѡ�ź�
    P1OUT |= (1 << cnt);  
       cnt++;                        //λ����������1~3֮��ѭ��
    if(cnt == 8) cnt = 5;
    WDTCTL = WDTPW+WDTCNTCL;
    time_count0++;
    if(time_count0>1024)
    {
       time_count0=0;
       secure1=0;
    }
    
  sample_adc12=ADC12MEM0;
  if(Sample_count_q<128) 
  {
    Sample_value_q= Sample_value_q + sample_adc12;
    Sample_count_q++;
  }
  else
  {
    Average_q=Sample_value_q/128;//����õ� ����ʵ����ֵ
    Sample_count_q=0;
    Sample_value_q=0;
  }

  if(((Average_q>Average)&&(Average_q-Average>300))
    || ((Average_q<Average)&&(Average-Average_q>300)))//�Ƚ���ֵ ��ֵ�仯������Χ�������Ӧ
  {
   Average=Average_q;
   Sample_value=0;
   Sample_count=0;
   if( zhengding_enble==1)
    {
      zd_ad++; 
    }
  }
  else
  {
    if(Sample_count<ad_sample_count)
    {
       Sample_value= Sample_value + sample_adc12;
       Sample_count++;
    }
    if(Sample_count>=ad_sample_count)
    { 
       Average = Sample_value/ad_sample_count;          
       Sample_count=0;
       Sample_value=0; 
       if( zhengding_enble==1)
       {
          zd_ad++; 
       }     
    }      
  }
  
  
  
  
   /* static uchar index =  0;
    
    P1OUT &=0x1f;                    //P1.5-P1.7�������λѡ�� 
    P2OUT = SEG[DispBuf[cnt-5]];     //�����ѡ�ź�
    P1OUT |= (1 << cnt);  
       cnt++;                        //λ����������1~3֮��ѭ��
    if(cnt == 8) cnt = 5;
    WDTCTL = WDTPW+WDTCNTCL;
    if(time_count0>10)
    {
       time_count0=0;
       secure1=0;
    }
          if(index<100)
          Results[index++] = ADC12MEM0;   // Move results, IFG is cleared
          if(index == 50)
          {
              index = 0;
              pingjun = average_fun(Results,0,49);
              if( zhengding_enble==1)
              {
                   zd_ad++; 
                   average_zd= pingjun;
              }
          }*/   
}
/*******************************************
�������ƣ�PORT1_ISR 
��    �ܣ�P1.0-P1.2�˿ڵ��жϷ�����
��    ������
����ֵ  ����
********************************************/
#pragma vector=PORT1_VECTOR
__interrupt void  PORT1_ISR(void)
{
  uint c; 
  key_value_ok=0x00;
  if(P1IFG & 0x07)
    {    
        switch(P1IFG&0x07)
        {
        case 0x01:
                    if(keyin == 0x00)   //����ǵ�1������������   �趨������ѡ��
                    {
                        delay();
                        if(keyin == 0x00)
                        { 
                          c=0; 
                           while(keyin != 0x01) //�ȴ������ſ�
                            { 
                              c++;  
                             // if( Status!=0x08)
                                 delay2(2);
                              if(c>9412) c=9000;
                              if(c>=8000
                                  &&Status==0x00
                                  &&secure1!=3
                                  &&secure1!=4
                                  &&secure1!=5
                                  &&secure1!=2) //������ť������λ
                              {
                                  if(c==8020||c==8040||c==8060)
                                  {
                                     danwei_label++;
                                     if(danwei_label>3)
                                       danwei_label=1;
                                     //keypad_Read_Setting_FormFlash();
                                     //VIP_setting[0] = danwei_label;
                                     //keypad_Write_Setting_To_Flash();
                                  }
                                  if(c==8062)
                                  {
                                     c=8000;
                                  }
                                  if(danwei_label==3)       
                                     {
                                       P5OUT|=0X60;
                                       P5OUT ^= BIT7;
                                     }
                                  else  if(danwei_label==2) 
                                    {
                                      P5OUT|=0Xa0;
                                      P5OUT ^= BIT6;
                                    }
                                  else  if(danwei_label==1) 
                                   {
                                     P5OUT|=0Xc0;
                                     P5OUT ^= BIT5;
                                   } 
                                   delay2(1000); 
                                   key_value=0x12;
                              }
                              else   if(c>8000&&Status==0x00&&secure1==3) 
                              {
                                  Status=0x01;  //����
                                  P1IFG = 0;
                                  return;
                              }
                              else   if(c>8000&&Status==0x00&&secure1==4) 
                              {
                                  Status=0x0a;  //���������ٶ�
                                  P1IFG = 0;
                                  return;
                              }
                              else   if(c>8000&&Status==0x00&&secure1==5) 
                              {
                                  Status=0x09;//����      
                                  P5OUT|=BIT2;
                                  P5OUT|=BIT4;
                                  P5OUT|=BIT5;
                                  P5OUT|=BIT6;
                                  P5OUT|=BIT7; 
                                  P1IFG = 0;
                                  return;
                              }
                              else   if(c>=8000&&Status==0x00&&secure1==2) 
                              {
                                  Correct_Press_Staus=1;
                                  if(c<8020) 
                                  {
                                     P5OUT|=0X60;
                                     P5OUT ^= BIT7;
                                     Correct_Press_Flag=1;//ѹ��ƫ��
                                     xianshi_errcode(c/10);
                                  }
                                  else 
                                  {
                                     xianshi_errcode(c/10);
                                     P5OUT|=0Xc0;
                                     P5OUT ^= BIT5;
                                     Correct_Press_Flag=0;//ѹ��ƫС
                                  }
                                  delay2(1000);    
                                  if(c>8040)
                                  {
                                     c=8002; 
                                  }
                              }
                              else if(c>8000&&(Status==0x08||Status==0x0a)) //����Ư������  / �����ٶ�У�� �˳�
                              {
                                   key_value=0x12;
                              }
                            } //
                           if(Correct_Press_Staus==1)
                           {
                             Status=0x08;//����Ư������
                             Correct_Press_Staus=0;
                           }
                           
                           if(c<8000)  
                           {
                             key_value = 0x11;
                             key_value_ok=0x01;
                             secure1++;
                           }
                            if(c<8000&&Status==0x00)
                              account++;
                           zhuangtai_flag=0;
                           P1IFG = 0;
                           time_count0=0;
                           return;
                        }
                    }
        case 0x02://���Ӽ�
                 P1IFG = 0;return; 
        case 0x04://ȷ����           
                 P1IFG = 0;return;   
        default:
                    //while(keyin != 0x07);       //�ȴ������ſ�
                    P1IFG = 0;
                    return;
        }
    }
  P1IFG = 0;
}


/*�жϷ������*/
#pragma vector = TIMERB0_VECTOR
__interrupt void Timer_B(void)
{
    Sample_Speed_Count++;            //������ʾ�ٶ�
    
    P1OUT &=0x1f;                    //P1.5-P1.7�������λѡ�� 
    P2OUT = SEG[DispBuf[cnt-5]];     //�����ѡ�ź�
    P1OUT |= (1 << cnt);  
       cnt++;                        //λ����������1~4֮��ѭ��
    if(cnt == 8) cnt = 5;    
}

/*******************************************
�������ƣ�watchdog_timer
��    �ܣ����Ź��жϷ��������������������ܵ�
          ��ѡ��λѡ�ź�
��    ������
����ֵ  ����
********************************************/
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{  

}



#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A(void)
{
P1OUT^=BIT3;
CCR0+=2000;
}