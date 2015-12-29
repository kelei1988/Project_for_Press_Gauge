#include "Include.h"

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
      Var_Init();
      //Init_timerA();
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
  TACCR0=1450;
  TACCR2=1450/2;
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
  WDTCTL = WDT_ARST_1000;  
  //IE1 |= WDTIE; //BoardConfig(0x88);         // 使能看门狗中断    
    
     BCSCTL1 |= XT2OFF;//设置外部高速时钟（8M Hz）关闭
     do
     {
       IFG1 &=~OFIFG;
       for (i=0xff;i>0;i--);
     }     
     while((IFG1 & OFIFG));    
     //BCSCTL2|=SELM_2;
     
    TBCCTL0 = CCIE;//使能CCIFG标志产生的中断
    TBCCR0 = 500;//（试验了一下，这句不加也可以）
    TBCTL = TBSSEL_2 + MC_2;//选择timer时钟和模式    
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
    P1DIR = 0xfe;               //设置P1.0~P1.2（按键）为输入状态；  1-输出   0-输入
                                //P1.3-P1.4（继电器控制）为输出状态  
                                //P1.5-P1.7（数码管位选）为输出状态
    P1IES = 0x01;               // P1.0~P1.2选择下降沿中断
    P1IE =  0x01;                // 打开中断使能   
    P1OUT = 0xe0;               //继电器不动作；数码管选通 
    P1IFG = 0X00;
    P2DIR = 0xff;               //设置P2.0-P2.7为输出（数码管段选）
    P2OUT = 0x00;               //段选全亮
    
    P4DIR = 0xFF; 
    P4OUT = 0xFF;
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


void Var_Init()
{ 
  uchar i;
  uint AD1[12]={1590,1702,1791,1841,1895,1991,2061,2165,2225,2370};
  uchar SEG1[32] = {0x03,0x9f,0x25,0x0d,0x99,0x49,0x41,0x1f, //显示数码管缓存数据库数组0-16   0.-16.  小板
                 0x01,0x09,0x11,0xc1,0x63,0x85,0x61,0x71,
                 0x02,0x9e,0x24,0x0c,0x98,0x48,0x40,0x1e,
                 0x00,0x08,0x10,0xc0,0x62,0x84,0x60,0x70};
  for(i=0;i<32;i++)
  {
     SEG[i]=SEG1[i];
  }
  for(i=0;i<12;i++)
  {
     AD[i]=AD1[i];
  }
  Status=0x00;

}