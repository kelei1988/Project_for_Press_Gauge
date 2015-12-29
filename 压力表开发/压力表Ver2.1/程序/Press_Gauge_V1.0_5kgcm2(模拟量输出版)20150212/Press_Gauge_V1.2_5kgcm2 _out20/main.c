/************************************************************ 
//5月1号晚上，
//修改了单位切换，上下限设定，整定精度切换中间数码管闪动的毛病。
//修改了上下限设定时候，不同单位持续增减速度的问题
//增加限制功能
//5.24晚上  修改了。压力降到0.下限灯灭了，那么继电器灯也应该灭的
//10.15修改了外接输出不正确的bug
//修改了ad采样后输出有漂移的问题，降低了采样频率
//11.12因为mos输出受三极管反向控制，所以修改输出控制正反
//修改切换单位问题11.27
//基本验证了上下限的阈值问题.
/剩余问题，数码扫描慢了
//剩余问题，上下限值设置！
12.9
//12.9.1上下限的设置方式调整了
12.11
修正了整定算法为平时取值的30倍平均数
修正了整定后下限灯常亮
修正了使得泵没有压力时候下限报警
修正了调整单位后密码数字归零
剩余问题数码管扫描速度
//10公斤，需要加入时间限制
//貌似采样速度比较慢
//8.19未确定是否为前期稳定版
//8.1 改输出控制引脚电平
      上下限互相调换
//2013.11.04 修改了定时器，使用定时器B代替了原先的定时器A。
//2013.11.04定时输出一个时钟波形
//2013.11.07  4-20输出基本正确，后期需要调整rc参数
//2013.11.27  4-20输出基本正确，后期需要调整详细输出情况
//修改了数据采样方法，控制采样次数，通过控制显示采样时间来控制显示变化速率
***********************************************************/
#include  <msp430x14x.h>
#define keyin    (P1IN & 0x07)
#define uchar  unsigned char
#define uint   unsigned int
#define SHANGXIAN_EN   {P5OUT&= (~BIT0);P4OUT|= BIT7;P5OUT^=BIT2;P5OUT|=BIT4;}//上限使能
#define XIAXIAN_EN     {P4OUT&= (~BIT7);P5OUT|= BIT0;P5OUT|=BIT2;P5OUT^=BIT4;}//下限使能
#define PER  1450

uint zhuangtai_flag;
float  shangxian=0;
float  xiaxian=0;
float  PWM_Dac_Psi;
uint z_flag=0;
unsigned long  Mpa,PSI,Kgcm2;

uchar SEG[32] = {0x03,0x9f,0x25,0x0d,0x99,0x49,0x41,0x1f, //显示数码管缓存数据库数组0-16   0.-16.  小板
                 0x01,0x09,0x11,0xc1,0x63,0x85,0x61,0x71,
                 0x02,0x9e,0x24,0x0c,0x98,0x48,0x40,0x1e,
                 0x00,0x08,0x10,0xc0,0x62,0x84,0x60,0x70};

uint Results[32],Results1[32],Results2[32],Results3[32],Results4[32]; //
uint zd_Results[32],zd_Results1[32],zd_Results2[32],zd_Results3[32],zd_Results4[32]; //

long Average,Average0,Average1,Average2,pingjun,average_zd;//用于对ad的计算。
uint yali,shangxian_value,xiaxian_value;
uint Kgcm210,Kgcm220,PSI10,PSI20,MPa10,MPa20;//不同单位时候上下限的数值
uint DispBuf[3] = {1,2,3};
uint secure1,secure2,secure3;
uint zhengding_enble;  
uint zd_ad;
/**danwei_label**shangxian_bijiao**xiaxian_bijiao************/
 uint VIP_setting[20]={0,0,0,0,0,0,0,0,0,0,0,0,0};
 uint AD[12]={1590,1702,1791,1841,1895,1991,2061,2165,2225,0};
float shangxian_1,xiaxian_1;
uchar key_value = 0x00;   // 按键的键值
uchar key_value_ok = 0x00; // 按键的键值
uint shangxian_2,xiaxian_2;
uint danwei_label=1;
uint danwei_label_f;
uint zhuangtai=0x00;
int a=0;
uchar cnt = 5;
int account;//限制功能计数参数
uint sh_dian;
uint time_count0;
uint time_count1;
uint time_count2;
unsigned int ref_4Ma;//10
unsigned int Sample_Time_Count;
/*********************/
/*****初始化系统******/
/*********************/
void Init_SYS();
void Init_Led();
void Init_LSD();
void Init_AD(); 
void Init_ALL();
void  menxian_charge();
void Bijiao();
void delay();
void delay2(int num);
void Test_Board();
void menxian_jia();
void menxian_jian();
void test();
void qiehuan_danwei();
void tiaozheng_menxian();
void zhengding_jingdu();
void xianshi_yali();
void xianshi_yunsuan();
void xianshi_mpa();
void baochi_xianshi();
void chixu(int b);
void xianshi_kgcm2();
void xianshi_psi() ;
void tong_tongdanwei();
void upanddomn() ;
void menxian_xianshi();
void zhengding();
void xianshi_shuzi(int x);               //辅助整定压力
void keypad_Write_Setting_To_Flash(void);//写入数据（单位，上下限掉电存储）
void keypad_Read_Setting_FormFlash(void);//读出数据 
void xianshi_shuzi2(int x);
void yincang();
unsigned int average_fun(unsigned int * array,unsigned int start_num,unsigned int end_num) ; //50us
void xianshi_errcode(int error_code);
void mpa_kg();
void mpa_psi();
void kg_mpa();
void kg_psi();
void psi_kg();
void psi_mpa();
void f4_to_5();//四舍五入算法
void Init_timerA(void);
void adjust_4ma_ref();

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
      shangxian = 0;
      xiaxian = 0;
      shangxian_1 = 0;
      xiaxian_1 = 0;
      VIP_setting[17]=shangxian_1;
      VIP_setting[18]=xiaxian_1;
      for(a=0;a<=11;a++)
        AD[a]=1500+150*a;
      VIP_setting[19]=0; 
      ref_4Ma =10;   
    }
    
      _BIS_SR(GIE);  
      secure1=0;
      secure2=0;
      secure3=0;
      zhengding_enble=0;
      time_count0=0;
      time_count1=0;
      //zhengding();
      while(1)
      { 
          WDTCTL = WDTPW+WDTCNTCL;//  清除看门狗
          switch(zhuangtai)
          {
            case 0x00:xianshi_yali();break;
            case 0x03:qiehuan_danwei();break;
            case 0x05:tiaozheng_menxian();break;
            case 0x06:tiaozheng_menxian();break;
            case 0x01:zhengding();break;
            case 0x04:adjust_4ma_ref();break;
            default:xianshi_yali();break; 
          }
      }
}
void adjust_4ma_ref()
{
   int i;
   while(key_value!=0x11)//按确定按键，调整完毕，退出
   {
     xianshi_errcode(i);
     while(key_value==0x21)
     {
      ref_4Ma++;
      key_value=0;
     }
     while(key_value==0x31)
     {
      ref_4Ma--;
      key_value=0;
     }
     if(ref_4Ma>20)
       ref_4Ma=20;
     if(ref_4Ma<1)
       ref_4Ma=1;
   } 
   key_value=0;
   keypad_Read_Setting_FormFlash();
   VIP_setting[19]=ref_4Ma;
   keypad_Write_Setting_To_Flash();
   zhuangtai = 0x00;
}
//整定ad对应压力
void zhengding()
{
 int i = 0;
 zhengding_enble=1;
  while(i!=11)//直到整定完毕
  {
    P5OUT|=BIT2;
    P5OUT|=BIT4;
    P5OUT|=BIT5;
    P5OUT|=BIT6;
    P5OUT|=BIT7; 
    while(key_value!=0x21)//按中间按键，单位压力递增0.05
    xianshi_shuzi(i);//修改整定时的显示
   /*
   0.50 1.00 1.50 2.00 2.50 3.00 3.50 4.00 4.50 5.00
   */
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
  VIP_setting[0] = 3;
  VIP_setting[1] = 0;
  VIP_setting[2] = 0;
 
  VIP_setting[3] = AD[0] ;
  VIP_setting[4] = AD[1] ;
  VIP_setting[5] = AD[2] ;
  VIP_setting[6] = AD[3] ;
  VIP_setting[7] = AD[4] ;
  VIP_setting[8] = AD[5] ;
  VIP_setting[9] = AD[6] ;
  VIP_setting[10] = AD[7] ;
  VIP_setting[11] = AD[8] ;
  VIP_setting[12] = AD[9] ;
  VIP_setting[13] = AD[10] ;
  VIP_setting[14] = AD[11] ;
  
  VIP_setting[15] = 0x3D ; 
  VIP_setting[16] = danwei_label_f;
  VIP_setting[17] = 0;
  VIP_setting[18] = 0;
  VIP_setting[19] = 10;
  keypad_Write_Setting_To_Flash();
  keypad_Read_Setting_FormFlash();
  zhuangtai = 0x00;
  key_value = 0x00;
  danwei_label=3;
  time_count0=0;
  time_count1=0;
}

//显示数字 辅助整定
void xianshi_shuzi(int x)
 {   
     x = x*50;
   
     DispBuf[0] = x/100+16;  //更新数码管显示
     DispBuf[1] = (x/10)%10;
     DispBuf[2] = x%10;
 }

void xianshi_errcode(int error_code)
{
     DispBuf[0] = error_code/100;  //更新数码管显示
     DispBuf[1] = (error_code/10)%10;
     DispBuf[2] = error_code%10;
}
/*******************************************
函数名称：xianshi_yali()
功    能：显示压力
参    数：无
返回值  ：无
********************************************/
void xianshi_yali()
{   
    int i=0;     
    if(   secure1==3
        && secure2==3
        && secure3==3 )
    zhuangtai=0x01;
    
        if(   secure1==4
        && secure2==4
        && secure3==4 )
    zhuangtai=0x04;  //调整4ma输出电流基准数值
    
    Bijiao();

    if(Sample_Time_Count>=500)
    {
       Sample_Time_Count=0;
       Average=pingjun;                           //运算得到PSI数值
    } 
    
    if(Average>AD[0]) 
    {
     for(i=0;i<=10;i++)
      {
      if(Average>=AD[i]&&Average<AD[i+1])//修正了偶尔显示0的问题
         PSI=(Average-AD[i])*72.5/(AD[i+1]-AD[i])+72.5*i; 
      }
    }
   else PSI=0;
  
   PWM_Dac_Psi=PSI;
   TACCR2=PWM_Dac_Psi*0.914888+149.487+ref_4Ma;//*0.567435; 0.10999
   if(PSI>725) 
   {
         xianshi_errcode(888);  
         TACCR2=823;
   }
   else
   {
      if(danwei_label == 3)//mpa               //根据单位标志，显示不同单位下的压力
      {
        xianshi_mpa(); 
       
        P5OUT|=BIT5;
        P5OUT|=BIT6;
        P5OUT|=BIT7;
        
        P5OUT&=~BIT7;
      }  
      else if(danwei_label == 2)//psi
      {
        xianshi_psi();
        P5OUT|=BIT5;
        P5OUT|=BIT6;
        P5OUT|=BIT7;
        P5OUT&=~BIT6;
      } 
      else if(danwei_label == 1)//kgcm
      {
        xianshi_kgcm2();        
        P5OUT|=BIT5;
        P5OUT|=BIT6;
        P5OUT|=BIT7;
        P5OUT&=~BIT5;
      } 
      else
      {
        xianshi_mpa(); 
        P5OUT|=BIT5;
        P5OUT|=BIT6;
        P5OUT|=BIT7; 
        P5OUT&=~BIT7;
      }
    }
}

/*******************************************
函数名称：Init_AD
功    能：初始化设置ADC12的各个寄存器
参    数：无
返回值  ：无
********************************************/
void Init_AD()      //初始化AD口
{
    P6SEL |= 0x01;                            // 使能AD通道（P6.0口）
    ADC12CTL0 = ADC12ON + SHT0_15 + MSC;      // Turn on ADC12, set sampling time
    ADC12CTL1 = SHP + CONSEQ_2;               // Use sampling timer, set mode
    ADC12IE = 0x01;                           // Enable ADC12IFG.0
    ADC12CTL0 |= ENC;                         // Enable conversions
    ADC12CTL0 |= ADC12SC;                     // Start conversion
}
/*******************************************
函数名称：ADC12ISR
功    能：ADC12的中断服务函数
参    数：无
返回值  ：无
********************************************/
#pragma vector=ADC_VECTOR
__interrupt void ADC12ISR (void)
{
    static uchar index =  0;
    static uchar index2 = 0;
    static uchar index3 = 0;
    static uchar index4 = 0;   
 
    
    static uchar zd_index =  0;
    static uchar zd_index2 = 0;
    static uchar zd_index3 = 0;
    static uchar zd_index4 = 0;   
    static uchar zd_index5 = 0;
    
      P1OUT &=0x1f;                    //P1.5-P1.7（数码管位选） 
    P2OUT = SEG[DispBuf[cnt-5]];     //输出段选信号
    P1OUT |= (1 << cnt);  
       cnt++;                        //位计数变量在1~4之间循环
    if(cnt == 8) cnt = 5;
     WDTCTL = WDTPW+WDTCNTCL;
    
    if( zhengding_enble==0)
    {    //time_count0=ADC12MEM0;;
      Results[index++] = ADC12MEM0;   // Move results, IFG is cleared
          if(index == 10)
          {
              index = 0;         
              Results1[index2++] = average_fun(Results,0,9);                      // Move results, IFG is cleared
              if(index2 == 10)                  
              {
                  index2 = 0;
                  Results2[index3++] = average_fun(Results1,0,9);                      // Move results, IFG is cleared
                  if(index3 ==6 )
                  {
                  index3 = 0;                                
                  Results3[index4++] = average_fun(Results2,0,5);                      // Move results, IFG is cleared
                  if(index4 == 5)
                    {
                      index4 = 0; 
                      pingjun  = average_fun(Results3,0,4);                      // Move results, IFG is cleared                   
                      time_count0++;
                    }                
                  }    
              }
          }
    }
    else if ( zhengding_enble==1)
    {
          zd_Results[zd_index++] = ADC12MEM0;   // Move results, IFG is cleared
              if(zd_index == 6)
          {
              zd_index = 0;         
              zd_Results1[zd_index2++] = average_fun(zd_Results,0,5);                      // Move results, IFG is cleared
              if(zd_index2 == 5)                  
              {
                  zd_index2 = 0;
                  zd_Results2[zd_index3++] = average_fun(zd_Results1,0,4);                      // Move results, IFG is cleared
                  if(zd_index3 ==5 )
                  {
                  zd_index3 = 0;                                
                  zd_Results3[zd_index4++] = average_fun(zd_Results2,0,4);                      // Move results, IFG is cleared
                  if(zd_index4 == 10)//10
                    {
                      zd_index4 = 0; 
                      zd_Results4[zd_index5++] = average_fun(zd_Results3,0,9); 
                      if(zd_index5 == 8)//8
                      {
                        zd_index5 = 0;    
                        average_zd  = average_fun(zd_Results4,0,7);
                        zd_ad++;                     // Move results, IFG is cleared  
                      }                
                    }                
                  }    
              }
          }
    }
    
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
                    if(keyin == 0x06)   //减少按键
                    {
                        delay();
                        if(keyin == 0x06)
                        { 
                          c=0;
                           while(keyin != 0x07)//按键直到，松开按键
                            {  
                            
                              c++;  
                              delay2(1);
                                  if(c>9400) c=9001; 
                               if(c>=9000&&zhuangtai==0x00)
                              {
                                P5OUT|=0x04;//关闭上限指示灯
                                P5OUT^=BIT4;//下限指示灯闪烁
                                delay2(500);
                                zhuangtai_flag=1;    //状态为下限调整
                                 } 
                              else if(c>9000&&(zhuangtai == 0x05||zhuangtai == 0x06))
                               chixu(0); //如果持续
                            }
                             if(c<9000)   {key_value = 0x31;secure3++;} //松开按键后检查，如果小于3000则判断为短按键
                           
                           if(zhuangtai_flag == 1)
                             zhuangtai = 0x06;   //状态为精度调整
                            zhuangtai_flag = 0;
                            P1IFG = 0;
                            return;
                        }
                    }
        case 0x02://增加键
                    if(keyin == 0x05)   //如果是第二个按键被按下  切换
                    {
                        delay();
                        if(keyin == 0x05)
                        {  c=0;
                           while(keyin != 0x07)
                            {  
                              c++;  
                              delay2(1);
                              if(c>9400) c=9001;
                              if(c>9000&&zhuangtai==0x00)
                              {
                                P5OUT|=0x10;//关闭下限指示灯
                                P5OUT^=BIT2;//上限指示灯闪烁
                                delay2(500);
                                zhuangtai_flag=1;   //状态为上限调整
                              }
                           
                              
                              
                               else if(c>9000&&(zhuangtai==0x05||zhuangtai==0x06))
                                chixu(1); 
                            }
                            if(c<9000)   
                            {key_value = 0x21;secure2++;} 
                            if(c<9000&&zhuangtai==0x00)   
                            {
                             account=account+2;
                            } 
                            if(zhuangtai_flag==1)
                              zhuangtai=0x05;   //上限
                           
                            zhuangtai_flag=0;
                            P1IFG = 0;
                            return;
                        }
                    }
        case 0x04://确定键
                    if(keyin == 0x03)   //如果是第1个按键被按下   设定上下限选项
                    {
                        delay();
                        if(keyin == 0x03)
                        { 
                          c=0; 
                           while(keyin != 0x07) //等待按键放开
                            { 
                              c++;  
                              delay2(2);
                              if(c>9400) c=9000;
                              if(c>8000&&zhuangtai==0x00)
                              { 
                                  if(danwei_label==3)       
                                     {P5OUT|=0X60;P5OUT ^= BIT7;}//当前单位闪烁
                                  else  if(danwei_label==2) 
                                    {P5OUT|=0Xa0;P5OUT ^= BIT6;}
                                  else  if(danwei_label==1) 
                                   {P5OUT|=0Xc0;P5OUT ^= BIT5;} 
                                   delay2(500); 
                                key_value=0x12;
                                zhuangtai_flag=1;
                              }
                            } 
                           if(c<8000)  
                           {
                             key_value = 0x11;
                             key_value_ok=0x01;
                             secure1++;
                           }
                            if(c<8000&&zhuangtai==0x00)  
                              account++;
                           if(zhuangtai_flag==1)
                             zhuangtai=0x03;
                           zhuangtai_flag=0;
                           P1IFG = 0;
                           return;
                        }
                    }      
        default:
                    //while(keyin != 0x07);       //等待按键放开
                    P1IFG = 0;
                    return;
        }
    }
  upanddomn();
}
/*******************************************
函数名称：delay
功    能：用于按键消抖的延时
参    数：无
返回值  ：无
********************************************/
void delay(void)
{
    uint tmp;
    for(tmp = 12000;tmp > 0;tmp--)
    // baochi_xianshi(); 
    ;
}
/*******************************************
函数名称：delay2
功    能：用于持续增减时的延时
参    数：无
返回值  ：无
********************************************/
void delay2(int num)
{
    uint tmp;
    for(tmp = 10*num;tmp > 0;tmp--)
     baochi_xianshi() ;
}
//

/*******************************************
函数名称：chixu
功    能：设置上下限时候，按
          住增加或减少按键不放，
          数值保持自动增减效果。
参    数：无
返回值  ：无
********************************************/
void chixu(int addordec)
{  
  
   int speed;
   if(danwei_label==3)
     speed=1;
   else  if(danwei_label==2)
     speed=6;
   else  if(danwei_label==1)
     speed=6;
     
    if(addordec==1)
      a=a+speed;
    else if(addordec==0)
      a=a-speed;   
   
    if(a>=800&&zhuangtai==0x05&&shangxian>=xiaxian) 
    {
      shangxian=shangxian+1;a=11; 
      if((danwei_label==3&&shangxian>=100)
        ||(danwei_label==2&&shangxian>=1450)
        ||(danwei_label==1&&shangxian>=1000))    
       shangxian--; 
    }
    else if(a<=11&&zhuangtai==0x05&&shangxian>xiaxian)
    {
      shangxian=shangxian-1;a=800; 
      if(xiaxian>=shangxian&&shangxian!=0) 
        shangxian++;
    }
    else if(a>=800&&zhuangtai==0x06&&shangxian>xiaxian) 
    {
      xiaxian=xiaxian+1;a=11;
       if(xiaxian>=shangxian) 
         xiaxian--;
    }
    else if(a<=11&&zhuangtai==0x06&&shangxian>=xiaxian) 
    {
      if(xiaxian==0) 
         xiaxian++;
      xiaxian=xiaxian-1;a=800;     
    }
     menxian_charge();
     if(a>=820)a=0;
     if(a<=10)a=820;  
     menxian_xianshi();
    P1OUT &=0x1f;    
    P2OUT = SEG[DispBuf[cnt-5]];     //输出段选信号
    P1OUT |= (1 << cnt);  
       cnt++;                        //位计数变量在1~4之间循环
    if(cnt == 8) cnt = 5;
    
     WDTCTL = WDTPW+WDTCNTCL;//  清除看门狗
}
/*******************************************
函数名称：baochi_xianshi
功    能：当程序在按键中断程序中时候，
          使用此程序以保持数码管持续显示
参    数：无
返回值  ：无
********************************************/
void baochi_xianshi()
{
    P1OUT &=0x1f;                    //P1.5-P1.7（数码管位选） 
    P2OUT = SEG[DispBuf[cnt-5]];     //输出段选信号
    P1OUT |= (1 << cnt);  
       cnt++;                        //位计数变量在1~4之间循环
    if(cnt == 8) cnt = 5;
    
     WDTCTL = WDTPW+WDTCNTCL;//  清除看门狗
}
/*******************************************
函数名称：Init_ALL
功    能：初始化所有模块
参    数：无
返回值  ：无
********************************************/ 
void Init_ALL()
{
Init_SYS();      //系统相关初始化
Init_Led();      //指示灯相关 IO初始化
Init_LSD();      //数码管\继电器\按键相关 IO初始化
Init_AD();       //初始化AD口
Init_timerA();
_EINT();        //开总中断
}

/*******************************************
函数名称：Init_timeA
功    能：时钟
参    数：无
返回值  ：无
********************************************/
void Init_timerA(void)
{
  //CCTL0= CCIE;
  //CCR0=5000;
  TACTL=TASSEL_2 + MC_1;
  TACCTL2 = OUTMOD_7; 
  P1SEL |=BIT3;//设置P1.3为输出
  P1DIR |=BIT3;//设置P1.3为输出
  TACCR0=PER;
  TACCR2=PER/2;
  //TACCR1=100;
}
/*******************************************
函数名称：Init_SYS
功    能：初始化看门狗模块
参    数：无
返回值  ：无
********************************************/
void Init_SYS()
{  
  unsigned int i;  

  //WDTCTL = WDT_ADLY_1_9;                    // 设置内部看门狗工作在定时器模式，1.9ms中断一次
  WDTCTL = WDTPW+WDTCNTCL;//  清除看门狗
  WDTCTL =WDT_ARST_1000;
  //IE1 |= WDTIE; //BoardConfig(0x88);         // 使能看门狗中断    
    
  BCSCTL1 &= ~XT2OFF;//设置外部高速时钟
     do
     {
       IFG1 &=~OFIFG;
       for (i=0xff;i>0;i--);
     }     while((IFG1 & OFIFG));    
     BCSCTL2|=SELM_2; 
     
    TBCCTL0 = CCIE;//使能CCIFG标志产生的中断
    TBCCR0 = 500;//（试验了一下，这句不加也可以）
    TBCTL = TBSSEL_2 + MC_2;//选择timer时钟和模式    
}

/*中断服务程序*/
#pragma vector = TIMERB0_VECTOR
__interrupt void Timer_B(void)
{
    //P1OUT ^= 0x01;
    Sample_Time_Count++;
      P1OUT &=0x1f;                    //P1.5-P1.7（数码管位选） 
    P2OUT = SEG[DispBuf[cnt-5]];     //输出段选信号
    P1OUT |= (1 << cnt);  
       cnt++;                        //位计数变量在1~4之间循环
    if(cnt == 8) cnt = 5;
    
    // WDTCTL = WDTPW+WDTCNTCL;//  清除看门狗
}



void Init_Led()    //指示灯相关 IO初始化
{
    P5DIR = 0xff;  //设置P5.7-P5.1为输出状态  1-输出   0-输入
    P5OUT = 0xff;  //初始设置P5.7-P5.1为高电平，灯灭
}
/*******************************************
函数名称：Init_LSD
功    能：初始化数码管\继电器\按键相关模块
参    数：无
返回值  ：无
********************************************/
void Init_LSD()      //数码管\继电器\按键相关 IO初始化
{
    P1DIR = 0xf8;               //设置P1.0~P1.2（按键）为输入状态；  1-输出   0-输入
                                //P1.3-P1.4（继电器控制）为输出状态  
                                //P1.5-P1.7（数码管位选）为输出状态
    P1IES = 0x07;               // P1.0~P1.2选择下降沿中断
    P1IE =  0x07;                // 打开中断使能   
    P1OUT = 0xe0;               //继电器不动作；数码管选通 
    P1IFG = 0X00;
    P2DIR = 0xff;               //设置P2.0-P2.7为输出（数码管段选）
    P2OUT = 0x00;               //段选全亮
    
    P4DIR = 0xFF; 
    P4OUT = 0xFF;
}

/*******************************************
函数名称：qiehuan_danwei
功    能：调整单位
参    数：无
返回值  ：无
********************************************/ 
void qiehuan_danwei()
{      
  
  while(key_value_ok!=0x01)
  {
    if (key_value==0x21)
    danwei_label--;
    else if (key_value==0x31)
    danwei_label++;
    
    key_value = 0x00;
    
    if(danwei_label>=4)
    danwei_label=1;
    else if(danwei_label==0)
      danwei_label=3;                            
    if(danwei_label==3)        //闪烁所调整的的单位
    {
      P5OUT|=0X60;
      P5OUT ^= BIT7;//mpa
    }   
    else  if(danwei_label==2) 
    {
      P5OUT|=0Xa0;
      P5OUT ^= BIT6;//psi
    }
    else  if(danwei_label==1)
    {
      P5OUT|=0Xc0;
      P5OUT ^= BIT5;//kg
    } 
    delay();
        delay();
    delay();
    delay();
    delay();
    delay();
    delay();
    delay();
  }
    
  if(danwei_label_f==1&&danwei_label==2)
     kg_psi(); 
    else if(danwei_label_f==1&&danwei_label==3)
      kg_mpa();
    else if(danwei_label_f==2&&danwei_label==1)
      psi_kg();
    else if(danwei_label_f==2&&danwei_label==3)
     psi_mpa(); 
    else if(danwei_label_f==3&&danwei_label==1)
      mpa_kg();
    else if(danwei_label_f==3&&danwei_label==2)
     mpa_psi(); 
    else  tong_tongdanwei()    ;
//shangxian =0;//单位切换后要上下限归零（需重新设置）
//xiaxian =0;
 secure1=0;
 secure2=0;
 secure3=0;

account=0;//计数清0

P5OUT|=0x14;
key_value_ok=0x00;
zhuangtai=0x00;
VIP_setting[0]=danwei_label;
VIP_setting[1]=shangxian;
VIP_setting[2]=xiaxian;
VIP_setting[16]=danwei_label_f;
VIP_setting[17]=shangxian_1;
VIP_setting[18]=xiaxian_1;
keypad_Write_Setting_To_Flash();
keypad_Read_Setting_FormFlash();

}
/*******************************************
函数名称：tiaozheng_menxian
功    能：调整上限和下限的数值
参    数：无
返回值  ：无
********************************************/ 
void tiaozheng_menxian()
 {
   key_value_ok=0x00;
 while(key_value_ok!=0x01)  //确定键按下后 返回主压力显示函数
 {
   if(zhuangtai == 0x05&&key_value == 0x21)
     {
       if(shangxian>=1000&&danwei_label==2) shangxian=shangxian+10;
       else shangxian++;
     if((danwei_label==3&&shangxian>=100)
        ||(danwei_label==1&&shangxian>=1000))    
       shangxian--;
     else if(danwei_label==2&&shangxian>=1450)
        shangxian= shangxian-10;
      menxian_charge();
     }
   else if(zhuangtai == 0x05&&key_value == 0x31&&shangxian!=0) 
   {
       if(shangxian>=1010&&danwei_label==2) shangxian=shangxian-10;
       else shangxian--; 
     if(xiaxian>=shangxian&&shangxian!=0&&danwei_label!=2) 
        shangxian++;
     else  if(xiaxian>=shangxian&&shangxian!=0&&danwei_label==2) 
       shangxian=shangxian+10;
      menxian_charge();
   }
   else if(zhuangtai == 0x06&&key_value == 0x21) 
   {
     if(xiaxian>=1000&&danwei_label==2) xiaxian=xiaxian+10;
       else xiaxian++;
     if(xiaxian>=shangxian&&danwei_label!=2) 
         xiaxian--;
     else if(xiaxian>=shangxian&&danwei_label==2) 
         xiaxian=xiaxian-10;
      menxian_charge();
   } 
   else if(zhuangtai == 0x06&&key_value == 0x31) 
   {
    
     if(xiaxian==0) 
         xiaxian++; 
     if(xiaxian>=1010&&danwei_label==2) xiaxian=xiaxian-10;
       else xiaxian--; 
     
    menxian_charge();
   } 
  
   key_value = 0x00; 
    
  menxian_xianshi();   
 }
    key_value_ok = 0x00;                        //Ok按键清空
    zhuangtai = 0x00; //状态转变为压力显示状态
    P5OUT|= 0x1f;//使得只有单位指示灯中的一个灯亮，上下限及其他灯灭
    VIP_setting[0] = danwei_label;
    VIP_setting[1] = shangxian;
    VIP_setting[2] = xiaxian;
    
    VIP_setting[16]=danwei_label_f;
    VIP_setting[17]=shangxian_1;
    VIP_setting[18]=xiaxian_1;
    account=0;//计数清0
    keypad_Write_Setting_To_Flash();
    keypad_Read_Setting_FormFlash();
 }

void menxian_charge()
{
    danwei_label_f=danwei_label; 
    shangxian_1=shangxian;
    xiaxian_1=xiaxian;
}
/*******************************************
函数名称：menxian_xianshi
功    能：不同单位时候显示门限 
          单位切换后原先设置归零
参    数：无
返回值  ：无
********************************************/ 
void  menxian_xianshi()
{   
  if(danwei_label==3&&zhuangtai==0x05)//mpa 0.40 400
    { 
      P5OUT=P5OUT&(~BIT2);     
      //if(shangxian>=40) shangxian=40;
      MPa20=shangxian*10;   //2560    
     DispBuf[0] = 16;  //更新数码管显示
     DispBuf[1] = MPa20/100;
     DispBuf[2] = (MPa20/10)%10;
    }
     else if(danwei_label==3&&zhuangtai==0x06)//mpa 0.40 400
    { 
      P5OUT=P5OUT&(~BIT4);  
      MPa10=xiaxian*10;   
      //if(MPa20>=400) MPa20=0;
     DispBuf[0] = 16;  //更新数码管显示
     DispBuf[1] = MPa10/100;
     DispBuf[2] = (MPa10/10)%10;
    }
    
     if(danwei_label==2&&zhuangtai==0x05)//
    { 
      P5OUT=P5OUT&(~BIT2);
     //if(shangxian>=600) shangxian=600;
      
      PSI20=shangxian;
     
      if(PSI20<1000)
      { 
         DispBuf[0] =  PSI20/100;  //更新数码管显示
         DispBuf[1] = (PSI20/10)%10+16;
         DispBuf[2] =  PSI20%10;
      }
      else
      {
         DispBuf[0] =  PSI20/1000;  //更新数码管显示
         DispBuf[1] = (PSI20/100)%10;
         DispBuf[2] =  (PSI20/10)%10; 
      } 
          
    }
     else if(danwei_label==2&&zhuangtai==0x06)//
    { 
      P5OUT=P5OUT&(~BIT4);  
      PSI10=xiaxian;     
     if(PSI10<1000)
      { 
         DispBuf[0] =  PSI10/100;  //更新数码管显示
         DispBuf[1] = (PSI10/10)%10+16;
         DispBuf[2] =  PSI10%10;
      }
      else
      {
         DispBuf[0] =  PSI10/1000;  //更新数码管显示
         DispBuf[1] = (PSI10/100)%10;
         DispBuf[2] =  (PSI10/10)%10; 
      }        
    }
    
      if(danwei_label==1&&zhuangtai==0x05)//
    { 
      
      P5OUT = P5OUT&(~BIT2);
     // if(shangxian>=400) shangxian=400;
      Kgcm220 = shangxian; 
      
     DispBuf[0] = Kgcm220/100+16;  //更新数码管显示
     DispBuf[1] = (Kgcm220/10)%10;
     DispBuf[2] =  Kgcm220%10;
    }
     else if(danwei_label==1&&zhuangtai==0x06)//
    { 
      P5OUT = P5OUT&(~BIT4); 
     Kgcm210 = xiaxian;    
     DispBuf[0] = Kgcm210/100+16;  //更新数码管显示
     DispBuf[1] = (Kgcm210/10)%10;
     DispBuf[2] =  Kgcm210%10;
    }
   WDTCTL = WDTPW + WDTCNTCL;//  清除看门狗
}
/*******************************************
函数名称：上下限处理防止超出控制状态
功    能：
参    数：无
返回值  ：无
********************************************/
void upanddomn()   
{
   if(zhuangtai==0x05&&key_value == 0x21)//上限显示 按键增加按下
   {    
      if((danwei_label==3&&shangxian>=100)
        ||(danwei_label==2&&shangxian>=1450)
        ||(danwei_label==1&&shangxian>=1000))    
       shangxian--; 
   }
   else if(zhuangtai==0x05&&key_value == 0x31)//上限显示 减少按键按下
   {
       if(xiaxian>=shangxian&&shangxian!=0) 
        shangxian++;
   }
   else if(zhuangtai==0x06&&key_value == 0x31)//下限显示 减少按键按下
   {
       if(xiaxian==0) 
         xiaxian++;
   }
   else if(zhuangtai==0x06&&key_value == 0x21)//下限显示 增加按键按下
   {
       if(xiaxian>=shangxian) 
         xiaxian--;
   }
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
  /*  P1OUT &=0x1f;    
    P2OUT = SEG[DispBuf[cnt-5]];    //输出段选信号
    P1OUT |= (1 << cnt);  
       cnt++;                        //位计数变量在1~4之间循环
    if(cnt == 8)
      cnt = 5;
*/

}
/*******************************************
函数名称：xianshi_mpa()
功    能：将压力数值转化为MPA单位显示编码，
          并输出到显示缓存数组
参    数：无
返回值  ：无
********************************************/
void xianshi_mpa()  //psi to Mpa  0.40 400
{      
     Mpa = PSI*100/145;   //60000 
     DispBuf[0] =  Mpa/1000+16;  //更新数码管显示
     DispBuf[1] =  (Mpa/100)%10;
     DispBuf[2] = (Mpa/10)%10;
}
/*******************************************
函数名称： xianshi_kgcm2()
功    能：将压力数值转化为kgcm2单位显示编码，
          并输出到显示缓存数组
参    数：无
返回值  ：无
********************************************/
void xianshi_kgcm2()  //psi to kgcm2  4.22 422
{      
     Kgcm2 = PSI*50/71;  
     if(Kgcm2<1000)
     { 
       DispBuf[0] =  Kgcm2/100+16;  //更新数码管显示
       DispBuf[1] = (Kgcm2/10)%10;
       DispBuf[2] =  Kgcm2%10; 
     } 
     else    
     { 
       DispBuf[0] =  Kgcm2/1000;  //更新数码管显示
       DispBuf[1] = (Kgcm2/100)%10+16;
       DispBuf[2] = (Kgcm2/10)%10; 
     } 
     
}
/*******************************************
函数名称：xianshi_psi()
功    能：将压力数值转化为psi单位显示编码，
          并输出到显示缓存数组
参    数：无
返回值  ：无
********************************************/
void xianshi_psi()  //显示PSI
{   
  //  if(PSI>1450)PSI=1450;  
  if(PSI<1000)
  { 
     DispBuf[0] =  PSI/100;  //更新数码管显示
     DispBuf[1] = (PSI/10)%10+16;
     DispBuf[2] =  PSI%10;
  }
  else
  {
     DispBuf[0] =  PSI/1000;  //更新数码管显示
     DispBuf[1] = (PSI/100)%10;
     DispBuf[2] =  (PSI/10)%10; 
  }
}

/******************************************************/
/******************************************************/
void keypad_Write_Setting_To_Flash(void) //向FLASH中写数据
{    
      unsigned char i;
      uint *tp,*write_ptr;  
      tp = (unsigned int *)0x1000;      //指向information flash 首地址，segmentB
        _DINT();
        FCTL1  = FWKEY + ERASE;   //写键值和允许擦除
        FCTL3  = FWKEY;                 //写键值，lock=0,
        *tp = 0;                            //空写入，确定段号
        FCTL1 = FWKEY + LOCK;           //lock=1,当前段擦除完毕 
        FCTL3 = FWKEY;                  //写键值，lock=0  
        write_ptr = (unsigned int *) 0x1000; 
        FCTL1 = FWKEY + WRT;            //写参数
        FCTL3 = FWKEY; 
  for(  i =0; i <=19; i++ )  
    *(write_ptr++)  = VIP_setting[i]; //逐个往Flash中写数组中的元素
        FCTL1 = FWKEY;                   //不写
        FCTL3 = FWKEY + LOCK; 
        _EINT();  
}
/******************************************************/
 //从FLASH中读数据
/******************************************************/
void keypad_Read_Setting_FormFlash(void)          //从FLASH中读数据
{  
    unsigned char i;
    unsigned int *tp;
    tp = (unsigned int*)0x1000;                //数据起始地址
    for( i = 0; i <=19 ; i++ )        
      VIP_setting[i] = *(tp++);    
    danwei_label = VIP_setting[0];
    shangxian= VIP_setting[1];
    xiaxian= VIP_setting[2];   
    for(i=0;i<12;i++) 
    AD[i] = VIP_setting[i+3];                   //读出后赋值ad各段数值
    
    sh_dian         = VIP_setting[15];
    danwei_label_f  = VIP_setting[16];
    shangxian_1     = VIP_setting[17];
    xiaxian_1       = VIP_setting[18];
    ref_4Ma         = VIP_setting[19];
    //  Keypad_Array_ToData( );                //读出后赋给相应变量
}
/******************************************************
比较函数
作用判定是否超出设定的上下限区间，超出是输出控制量和指示灯
*****************************************************/
void Bijiao()
{
    if(shangxian!=0||xiaxian!=0) //超出设定的下限数值，下限报警
    { 
      if((danwei_label==3&&Mpa<xiaxian*10) 
         ||(danwei_label==2&&PSI<xiaxian)
         ||(danwei_label==1&&Kgcm2<xiaxian))    
         {
           XIAXIAN_EN;
           delay(); delay(); delay(); delay(); delay(); delay(); delay(); delay();
         }
      else if((danwei_label==3&&Mpa>shangxian*10) //超出设定的上限数值，上限报警
          ||(danwei_label==2&&PSI>shangxian)
          ||(danwei_label==1&&Kgcm2>shangxian)) 
         {
           SHANGXIAN_EN;
          delay(); delay(); delay(); delay(); delay(); delay(); delay(); delay();
         } 
       else //在正常工作区间 
         {
           P5OUT|=BIT2;
           P5OUT|=BIT4;
           P4OUT&= (~BIT7);
           P5OUT&= (~BIT0);         
         }
     }      
   yincang();
}


void yincang()
{ 
  if(  secure1==1 && secure2==1&& secure3==1 &&Average>AD[0]&&Average<AD[1])
      {             
        keypad_Read_Setting_FormFlash();              
               zhengding_enble=1;
        P5OUT|=BIT2;
        P5OUT|=BIT4;
        P5OUT|=BIT5;
        P5OUT|=BIT6;
        P5OUT|=BIT7; 
        zd_ad=0;
        while(zd_ad!=1)
        {};
        AD[0] =average_zd+5; 
        if(AD[0]>=AD[1])AD[0]=AD[1]-1;
         zhengding_enble=0;
        VIP_setting[0] = danwei_label;
        VIP_setting[1]=shangxian;
        VIP_setting[2]=xiaxian;         
        VIP_setting[3] = AD[0] ;
        VIP_setting[4] = AD[1] ;
        VIP_setting[5] = AD[2] ;
        VIP_setting[6] = AD[3] ;
        VIP_setting[7] = AD[4] ;
        VIP_setting[8] = AD[5] ;
        VIP_setting[9] = AD[6] ;
        VIP_setting[10] = AD[7] ;
        VIP_setting[11] = AD[8] ;
        VIP_setting[12] = AD[9] ;
        VIP_setting[13] = AD[10] ;
        VIP_setting[14] = AD[11] ;
        VIP_setting[15] = 0x3D ; 
        VIP_setting[16] = danwei_label_f;
        VIP_setting[17] = shangxian_1;
        VIP_setting[18] = xiaxian_1;
        VIP_setting[19] = 0;
        keypad_Write_Setting_To_Flash();
        keypad_Read_Setting_FormFlash(); 
        secure2=99;
        secure1=25;
       } 
   else    if(  secure1==19 && secure2==20&& secure3==13 )
      {   
          while(1)
          { 
            DispBuf[0] =  1 ;  //更新数码管显示
            DispBuf[1] =  0 ;
            DispBuf[2] =  0 ;   
          }  
       }    
}


/**平均算法**/ 
unsigned int average_fun(unsigned int * array,unsigned int start_num,unsigned int end_num)  //50us
  {
      unsigned int *p;
      unsigned int i,max,min;
      unsigned long sum=0;
      p=array;
      max=*p;
      min=*p;
      for(i=start_num;i<end_num+1;i++)
      {
      sum+=*(p+i);
      if(*(p+i)>max)max=*(p+i);
      if(*(p+i)<min)min=*(p+i);
      }
      sum-=max;
      sum-=min;
      sum=sum/(end_num-start_num-1);
      return((unsigned int)sum);
  }

void psi_kg()
{
  shangxian=shangxian_1*510/725;
  xiaxian=xiaxian_1*510/725;
   f4_to_5();//四舍五入算法
   if(xiaxian>=999)xiaxian=999;
   if(shangxian>=999)shangxian=999;
}

void psi_mpa()
{
  shangxian=shangxian_1*10/145;
  xiaxian=xiaxian_1*10/145; 
   f4_to_5();//四舍五入算法
   if(xiaxian>=99)xiaxian=99;
   if(shangxian>=99)shangxian=99;
}


void kg_mpa()
{
  shangxian=shangxian_1*10/102;
  xiaxian=xiaxian_1*10/102; 
   f4_to_5();//四舍五入算法
   if(xiaxian>=99)xiaxian=99;
   if(shangxian>=99)shangxian=99;
}


void kg_psi()
{
  shangxian=shangxian_1*725/510;
  xiaxian=xiaxian_1*725/510; 
   f4_to_5();//四舍五入算法
   if(xiaxian>=1440)xiaxian=1440;
   if(shangxian>=1440)shangxian=1440;
}
void mpa_kg()
{
  shangxian=shangxian_1*102/10;
  xiaxian=xiaxian_1*102/10; 
   f4_to_5();//四舍五入算法
   if(xiaxian>=999)xiaxian=999;
   if(shangxian>=999)shangxian=999;
}
void mpa_psi()
{
  shangxian=shangxian_1*145/10;
  xiaxian=xiaxian_1*145/10; 
   f4_to_5();//四舍五入算法
   if(xiaxian>=1440)xiaxian=1440;
   if(shangxian>=1440)shangxian=1440;
}
void tong_tongdanwei()
{
  shangxian=shangxian_1;
  xiaxian=xiaxian_1; 

}

void f4_to_5()//四舍五入算法
{
 shangxian_2=shangxian;
 if((shangxian-shangxian_2)*10>=5)
   shangxian++;
 
  xiaxian_2=xiaxian;
 if((xiaxian-xiaxian_2)*10>=5)
   xiaxian++;
}
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A(void)
{
P1OUT^=BIT3;
CCR0+=2000;
}
