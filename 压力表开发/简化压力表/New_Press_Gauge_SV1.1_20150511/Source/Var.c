#include "Include.h"

uint zhuangtai_flag;
uint z_flag;
unsigned long  Mpa,PSI,Kgcm2;

uchar SEG[32] ;

uint Results[32],Results1[32],Results2[32],Results3[32],Results4[32],cal_pingjun[32]; //
uint zd_Results[32],zd_Results1[32],zd_Results2[32],zd_Results3[32],zd_Results4[32]; //

long Average,Average0,Average1,Average2,pingjun,Last_pingjun,average_zd;//���ڶ�ad�ļ��㡣

uint yali,shangxian_value,xiaxian_value;
uint Kgcm210,Kgcm220,PSI10,PSI20,MPa10,MPa20;//��ͬ��λʱ�������޵���ֵ
uint DispBuf[3];
uint secure1,secure2,secure3;
uint zhengding_enble;  
uint zd_ad;
/**danwei_label**shangxian_bijiao**xiaxian_bijiao************/
 uint VIP_setting[20];
 uint AD[12];
float shangxian_1,xiaxian_1;
uchar key_value ;   // �����ļ�ֵ
uchar key_value_ok ; // �����ļ�ֵ
uint shangxian_2,xiaxian_2;
uint danwei_label;
uint danwei_label_f;
uint Status;
int a;
uchar cnt ;
int account;//���ƹ��ܼ�������
uint sh_dian;
uint time_count0;
uint time_count1;
uint time_count2;
uint Correct_Press_Flag; //����Ư������ʱ���м����
uint Correct_Press_Staus;
uint Sample_Speed_Count;//�����ٶ�