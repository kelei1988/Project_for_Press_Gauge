#ifndef VAR_H
#define VAR_H

extern uint zhuangtai_flag;
extern uint z_flag;
extern unsigned long  Mpa,PSI,Kgcm2;

extern uchar SEG[32];

extern uint Results[32],Results1[32],Results2[32],Results3[32],Results4[32],cal_pingjun[32]; //
extern uint zd_Results[32],zd_Results1[32],zd_Results2[32],zd_Results3[32],zd_Results4[32]; //

extern long Average,Average0,Average1,Average2,pingjun,Last_pingjun,average_zd;//���ڶ�ad�ļ��㡣
extern uint yali,shangxian_value,xiaxian_value;
extern uint Kgcm210,Kgcm220,PSI10,PSI20,MPa10,MPa20;//��ͬ��λʱ�������޵���ֵ
extern uint DispBuf[3] ;
extern uint secure1,secure2,secure3;
extern uint zhengding_enble;  
extern uint zd_ad;
/**danwei_label**shangxian_bijiao**xiaxian_bijiao************/
extern uint VIP_setting[20];
extern uint AD[12];
extern float shangxian_1,xiaxian_1;
extern uchar key_value ;   // �����ļ�ֵ
extern uchar key_value_ok ; // �����ļ�ֵ
extern uint shangxian_2,xiaxian_2;
extern uint danwei_label;
extern uint danwei_label_f;
extern uint Status;
extern int a;
extern uchar cnt ;
extern int account;//���ƹ��ܼ�������
extern uint sh_dian;
extern uint time_count0;
extern uint time_count1;
extern uint Sample_Speed;
extern uint Correct_Press_Flag; //����Ư������ʱ���м����
extern uint Correct_Press_Staus;
extern uint Sample_Speed_Count;//������ʾ�ٶ�
#endif