#include "Include.h"

/*******************************************
函数名称：ADC12ISR
功    能：ADC12的中断服务函数
参    数：无
返回值  ：无
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
  ad_sample_count=Sample_Speed*1024;
  P1OUT &=0x1f;                    //P1.5-P1.7（数码管位选） 
    P2OUT = SEG[DispBuf[cnt-5]];     //输出段选信号
    P1OUT |= (1 << cnt);  
       cnt++;                        //位计数变量在1~3之间循环
    if(cnt == 8) cnt = 5;
    WDTCTL = WDTPW+WDTCNTCL;
    time_count0++;
    if(time_count0>1024)
    {
       time_count0=0;
       secure1=0;
    }
    
  sample_adc12=ADC12MEM0;
  if(Sample_count_q<512) 
  {
    Sample_value_q= Sample_value_q + sample_adc12;
    Sample_count_q++;
  }
  else
  {
    Average_q=Sample_value_q/512;//计算得到 快速实际数值
    Sample_count_q=0;
    Sample_value_q=0;
  }

  if(((Average_q>Average)&&(Average_q-Average>300))
    || ((Average_q<Average)&&(Average-Average_q>300)))//比较数值 数值变化超过范围则快速响应
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
    
    P1OUT &=0x1f;                    //P1.5-P1.7（数码管位选） 
    P2OUT = SEG[DispBuf[cnt-5]];     //输出段选信号
    P1OUT |= (1 << cnt);  
       cnt++;                        //位计数变量在1~3之间循环
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
函数名称：PORT1_ISR 
功    能：P1.0-P1.2端口的中断服务函数
参    数：无
返回值  ：无
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
                    if(keyin == 0x00)   //如果是第1个按键被按下   设定上下限选项
                    {
                        delay();
                        if(keyin == 0x00)
                        { 
                          c=0; 
                           while(keyin != 0x01) //等待按键放开
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
                                  &&secure1!=2) //长按按钮调整单位
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
                                  Status=0x01;  //整定
                                  P1IFG = 0;
                                  return;
                              }
                              else   if(c>8000&&Status==0x00&&secure1==4) 
                              {
                                  Status=0x0a;  //修正采样速度
                                  P1IFG = 0;
                                  return;
                              }
                              else   if(c>8000&&Status==0x00&&secure1==5) 
                              {
                                  Status=0x09;//归零      
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
                                     Correct_Press_Flag=1;//压力偏大
                                     xianshi_errcode(c/10);
                                  }
                                  else 
                                  {
                                     xianshi_errcode(c/10);
                                     P5OUT|=0Xc0;
                                     P5OUT ^= BIT5;
                                     Correct_Press_Flag=0;//压力偏小
                                  }
                                  delay2(1000);    
                                  if(c>8040)
                                  {
                                     c=8002; 
                                  }
                              }
                              else if(c>8000&&(Status==0x08||Status==0x0a)) //整体漂移修正  / 采样速度校正 退出
                              {
                                   key_value=0x12;
                              }
                            } //
                           if(Correct_Press_Staus==1)
                           {
                             Status=0x08;//整体漂移修正
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
        case 0x02://增加键
                 P1IFG = 0;return; 
        case 0x04://确定键           
                 P1IFG = 0;return;   
        default:
                    //while(keyin != 0x07);       //等待按键放开
                    P1IFG = 0;
                    return;
        }
    }
  P1IFG = 0;
}


/*中断服务程序*/
#pragma vector = TIMERB0_VECTOR
__interrupt void Timer_B(void)
{
    Sample_Speed_Count++;            //虚拟显示速度
    
    P1OUT &=0x1f;                    //P1.5-P1.7（数码管位选） 
    P2OUT = SEG[DispBuf[cnt-5]];     //输出段选信号
    P1OUT |= (1 << cnt);  
       cnt++;                        //位计数变量在1~4之间循环
    if(cnt == 8) cnt = 5;    
}

/*******************************************
函数名称：watchdog_timer
功    能：看门狗中断服务函数，在这里输出数码管的
          段选和位选信号
参    数：无
返回值  ：无
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