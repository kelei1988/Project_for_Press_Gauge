#include "Include.h"
/******************��������ѹ��Ư��********************/
/*******************************************
�������ƣ�Correct_Press_Value()
��    �ܣ�������������ѹ��Ư��
��    ������
����ֵ  ����
********************************************/
/******************************************************/
/******************************************************/
void Correct_Press_Value()
{ 
        uchar i;
        uchar Correct_Vlaue;
        key_value=0x00;
        Correct_Vlaue= 0; 
        while(key_value!=0x12)//�ȴ����°���ȷ��
        { 
            if(key_value==0x11)//����
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
              AD[i]=AD[i]-Correct_Vlaue; //�����µı궨����ֵ
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

/******************�������Ư��*******************/
/*******************************************
�������ƣ�Correct_Zero_Value()
��    �ܣ������������ѹ��Ư��
��    ������
����ֵ  ����
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
        while(key_value!=0x11)//���м䰴������λѹ������0.05
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


/********************��FLASH��д������********************/
/*******************************************
keypad_Write_Setting_To_Flash()
��    �ܣ�������FLASH��д����Ҫ���������
��    ������
����ֵ  ����
********************************************/
/******************************************************/
void keypad_Write_Setting_To_Flash(void) //��FLASH��д����
{    
      unsigned char i;
      uint *tp,*write_ptr;  
      tp = (unsigned int *)0x1000;      //ָ��information flash �׵�ַ��segmentB
        _DINT();
        FCTL1  = FWKEY + ERASE;   //д��ֵ���������
        FCTL3  = FWKEY;                 //д��ֵ��lock=0,
        *tp = 0;                            //��д�룬ȷ���κ�
        FCTL1 = FWKEY + LOCK;           //lock=1,��ǰ�β������ 
        FCTL3 = FWKEY;                  //д��ֵ��lock=0  
        write_ptr = (unsigned int *) 0x1000; 
        FCTL1 = FWKEY + WRT;            //д����
        FCTL3 = FWKEY; 
        for(  i =0; i <=19; i++ )  
        *(write_ptr++)  = VIP_setting[i]; //�����Flash��д�����е�Ԫ��
        FCTL1 = FWKEY;                   //��д
        FCTL3 = FWKEY + LOCK; 
        _EINT();  
}
/******************************************************/
 //��FLASH�ж�����
/******************************************************/
void keypad_Read_Setting_FormFlash(void)          //��FLASH�ж�����
{  
    unsigned char i;
    unsigned int *tp;
    tp = (unsigned int*)0x1000;                //������ʼ��ַ
    for( i = 0; i <=19 ; i++ )        
      VIP_setting[i] = *(tp++);    
    danwei_label = VIP_setting[0];

    for(i=0;i<12;i++) 
    AD[i] = VIP_setting[i+3];                   //������ֵad������ֵ
    
    sh_dian         = VIP_setting[15];
    danwei_label_f  = VIP_setting[16];
    shangxian_1     = VIP_setting[17];
    xiaxian_1       = VIP_setting[18];
    time_count2     = VIP_setting[19];
    //  Keypad_Array_ToData( );                //�����󸳸���Ӧ����
}


/**ƽ���㷨**/ 
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
           
            P1OUT &=0x1f;                    //P1.5-P1.7�������λѡ�� 
            P2OUT = SEG[DispBuf[cnt-5]];     //�����ѡ�ź�
            P1OUT |= (1 << cnt);  
               cnt++;                        //λ����������1~3֮��ѭ��
            if(cnt == 8) cnt = 5;
      }
      sum-=max;
      sum-=min;
      sum=sum/(end_num-start_num-1);
      return((unsigned int)sum);
  }


/*******************************************
�������ƣ�delay
��    �ܣ����ڰ�����������ʱ
��    ������
����ֵ  ����
********************************************/
void delay(void)
{
    uint tmp;
    for(tmp = 1200;tmp > 0;tmp--)//12000
    // baochi_xianshi(); 
    ;
}
/*******************************************
�������ƣ�delay2
��    �ܣ����ڳ�������ʱ����ʱ
��    ������
����ֵ  ����
********************************************/
void delay2(int num)
{
    uint tmp;
    for(tmp = num;tmp > 0;tmp--)
     Display_SEG() ;
}
//

