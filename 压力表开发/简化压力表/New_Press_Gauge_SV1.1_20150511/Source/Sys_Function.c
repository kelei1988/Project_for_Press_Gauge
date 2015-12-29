#include "Include.h"
/******************修正整体压力漂移********************/
/*******************************************
函数名称：Correct_Press_Value()
功    能：用于修正整体压力漂移
参    数：无
返回值  ：无
********************************************/
/******************************************************/
/******************************************************/
void Correct_Press_Value()
{ 
        uchar i;
        uchar Correct_Vlaue;
        key_value=0x00;
        Correct_Vlaue= 0; 
        while(key_value!=0x12)//等待按下按键确定
        { 
            if(key_value==0x11)//按键
            {
              Correct_Vlaue++;
              key_value=0;
            }
            if(Correct_Press_Flag==1)
            {
              P5OUT|=0X60;
              P5OUT ^= BIT7;
            }
            else
            {
              P5OUT|=0Xc0;
              P5OUT ^= BIT5;
            }
            xianshi_errcode(Correct_Vlaue);    
        }
        key_value=0x00;

         for(i=0;i<=11;i++)
         {
           if(Correct_Press_Flag==1)
           {
              AD[i]=AD[i]-Correct_Vlaue; //计算新的标定点数值
           }
           else
           {
              AD[i]=AD[i]+Correct_Vlaue;
           }
         }
        key_value=0;            
        VIP_setting[0] = danwei_label;
        //VIP_setting[1]=shangxian;
        //VIP_setting[2]=xiaxian;  
        for(i=0;i<=11;i++)
        {
          VIP_setting[i+3] = AD[i] ;
        }
        VIP_setting[15] = 0x3D ; 
        VIP_setting[16] = danwei_label_f;
        //VIP_setting[17] = shangxian_1;
        //VIP_setting[18] = xiaxian_1;
        VIP_setting[19] = 0;
        keypad_Write_Setting_To_Flash();
        keypad_Read_Setting_FormFlash(); 
        secure2=99;
        secure1=0;
        Status=0x00;
}

/******************修正零点漂移*******************/
/*******************************************
函数名称：Correct_Zero_Value()
功    能：用于修正零点压力漂移
参    数：无
返回值  ：无
********************************************/
/******************************************************/
void Correct_Zero_Value()
{ 
  unsigned char i;
  if(Average<AD[1])
      { key_value=0;            
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
        while(key_value!=0x11)//按中间按键，单位压力递增0.05
        AD[0] =average_zd+5; key_value=0;     
        if(AD[0]>=AD[1])AD[0]=AD[1]-1;
         zhengding_enble=0;
        VIP_setting[0] = danwei_label;
        //VIP_setting[1]=shangxian;
        //VIP_setting[2]=xiaxian;   
        for(i=0;i<=11;i++)
        {
          VIP_setting[i+3] = AD[i] ;
        }
        VIP_setting[15] = 0x3D ; 
        VIP_setting[16] = danwei_label_f;
        //VIP_setting[17] = shangxian_1;
        //VIP_setting[18] = xiaxian_1;
        VIP_setting[19] = 0;
        keypad_Write_Setting_To_Flash();
        keypad_Read_Setting_FormFlash(); 
        secure2=99;
        secure1=25;
       }  
  Status=0x00;
}


/********************向FLASH中写入数据********************/
/*******************************************
keypad_Write_Setting_To_Flash()
功    能：用于向FLASH中写入需要保存的数据
参    数：无
返回值  ：无
********************************************/
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

    for(i=0;i<12;i++) 
    AD[i] = VIP_setting[i+3];                   //读出后赋值ad各段数值
    
    sh_dian         = VIP_setting[15];
    danwei_label_f  = VIP_setting[16];
    shangxian_1     = VIP_setting[17];
    xiaxian_1       = VIP_setting[18];
    time_count2     = VIP_setting[19];
    //  Keypad_Array_ToData( );                //读出后赋给相应变量
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
           
            P1OUT &=0x1f;                    //P1.5-P1.7（数码管位选） 
            P2OUT = SEG[DispBuf[cnt-5]];     //输出段选信号
            P1OUT |= (1 << cnt);  
               cnt++;                        //位计数变量在1~3之间循环
            if(cnt == 8) cnt = 5;
      }
      sum-=max;
      sum-=min;
      sum=sum/(end_num-start_num-1);
      return((unsigned int)sum);
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
    for(tmp = 1200;tmp > 0;tmp--)//12000
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
    for(tmp = num;tmp > 0;tmp--)
     Display_SEG() ;
}
//

