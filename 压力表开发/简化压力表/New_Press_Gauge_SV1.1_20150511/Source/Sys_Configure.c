#include "Include.h"

/*******************************************
�������ƣ�Init_ALL
��    �ܣ���ʼ������ģ��
��    ������
����ֵ  ����
********************************************/ 
void Init_ALL()
{
Init_SYS();      //ϵͳ��س�ʼ��
Init_Led();      //ָʾ����� IO��ʼ��
Init_LSD();      //�����\�̵���\������� IO��ʼ��
Init_AD();       //��ʼ��AD��
Var_Init();
//Init_timerA();
_EINT();        //�����ж�
}

/*******************************************
�������ƣ�Init_timeA
��    �ܣ�ʱ��
��    ������
����ֵ  ����
********************************************/
void Init_timerA(void)
{
  //CCTL0= CCIE;
  //CCR0=5000;
  TACTL=TASSEL_2 + MC_1;
  TACCTL2 = OUTMOD_7; 
  P1SEL |=BIT3;//����P1.3Ϊ���
  P1DIR |=BIT3;//����P1.3Ϊ���
  TACCR0=1450;
  TACCR2=1450/2;
  //TACCR1=100;
}
/*******************************************
�������ƣ�Init_SYS
��    �ܣ���ʼ�����Ź�ģ��
��    ������
����ֵ  ����
********************************************/
void Init_SYS()
{  
  unsigned int i;  

  //WDTCTL = WDT_ADLY_1_9;                    // �����ڲ����Ź������ڶ�ʱ��ģʽ��1.9ms�ж�һ��
  WDTCTL = WDTPW+WDTCNTCL;//  ������Ź�
  WDTCTL =WDT_ARST_1000;  
  //IE1 |= WDTIE; //BoardConfig(0x88);         // ʹ�ܿ��Ź��ж�    
    
    BCSCTL1 |= XT2OFF;//�����ⲿ����ʱ�ӣ�8M Hz���ر�
     do
     {
       IFG1 &=~OFIFG;
       for (i=0xff;i>0;i--);
     }     
     while((IFG1 & OFIFG));    
   //  BCSCTL2|=SELM_2; 
     
    TBCCTL0 = CCIE;//ʹ��CCIFG��־�������ж�
    TBCCR0 = 500;//��������һ�£���䲻��Ҳ���ԣ�
    TBCTL = TBSSEL_2 + MC_2;//ѡ��timerʱ�Ӻ�ģʽ    
}

void Init_Led()    //ָʾ����� IO��ʼ��
{
    P5DIR = 0xff;  //����P5.7-P5.1Ϊ���״̬  1-���   0-����
    P5OUT = 0xff;  //��ʼ����P5.7-P5.1Ϊ�ߵ�ƽ������
}
/*******************************************
�������ƣ�Init_LSD
��    �ܣ���ʼ�������\�̵���\�������ģ��
��    ������
����ֵ  ����
********************************************/
void Init_LSD()      //�����\�̵���\������� IO��ʼ��
{
    P1DIR = 0xfe;               //����P1.0~P1.2��������Ϊ����״̬��  1-���   0-����
                                //P1.3-P1.4���̵������ƣ�Ϊ���״̬  
                                //P1.5-P1.7�������λѡ��Ϊ���״̬
    P1IES = 0x01;               // P1.0~P1.2ѡ���½����ж�
    P1IE =  0x01;                // ���ж�ʹ��   
    P1OUT = 0xe0;               //�̵����������������ѡͨ 
    P1IFG = 0X00;
    P2DIR = 0xff;               //����P2.0-P2.7Ϊ���������ܶ�ѡ��
    P2OUT = 0x00;               //��ѡȫ��
    
    P4DIR = 0xFF; 
    P4OUT = 0xFF;
}

/*******************************************
�������ƣ�Init_AD
��    �ܣ���ʼ������ADC12�ĸ����Ĵ���
��    ������
����ֵ  ����
********************************************/
void Init_AD()      //��ʼ��AD��
{
    P6SEL |= 0x01;                            // ʹ��ADͨ����P6.0�ڣ�
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
  uchar SEG1[32] = {0x03,0x9f,0x25,0x0d,0x99,0x49,0x41,0x1f, //��ʾ����ܻ������ݿ�����0-16   0.-16.  С��
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
/*
uint zhuangtai_flag;
uint z_flag;
unsigned long  Mpa,PSI,Kgcm2;

uchar SEG[32] = {0x03,0x9f,0x25,0x0d,0x99,0x49,0x41,0x1f, //��ʾ����ܻ������ݿ�����0-16   0.-16.  С��
                 0x01,0x09,0x11,0xc1,0x63,0x85,0x61,0x71,
                 0x02,0x9e,0x24,0x0c,0x98,0x48,0x40,0x1e,
                 0x00,0x08,0x10,0xc0,0x62,0x84,0x60,0x70};

uint Results[32],Results1[32],Results2[32],Results3[32],Results4[32]; //
uint zd_Results[32],zd_Results1[32],zd_Results2[32],zd_Results3[32],zd_Results4[32]; //

long Average,Average0,Average1,Average2,pingjun,average_zd;//���ڶ�ad�ļ��㡣
uint yali,shangxian_value,xiaxian_value;
uint Kgcm210,Kgcm220,PSI10,PSI20,MPa10,MPa20;//��ͬ��λʱ�������޵���ֵ
uint DispBuf[3] = {1,2,3};
uint secure1,secure2,secure3;
uint zhengding_enble;  
uint zd_ad;

 uint VIP_setting[20]={0,0,0,0,0,0,0,0,0,0,0,0,0};
 uint AD[12]={1590,1702,1791,1841,1895,1991,2061,2165,2225,0};
float shangxian_1,xiaxian_1;
uchar key_value = 0x00;   // �����ļ�ֵ
uchar key_value_ok = 0x00; // �����ļ�ֵ
uint shangxian_2,xiaxian_2;
uint danwei_label=1;
uint danwei_label_f;
uint Status=0x00;
int a=0;
uchar cnt = 5;
int account;//���ƹ��ܼ�������
uint sh_dian;
uint time_count0;
uint time_count1;
uint time_count2;
uint Correct_Vlaue; //����Ư������ʱ���м����
*/
}