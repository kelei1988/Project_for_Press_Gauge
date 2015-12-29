#ifndef SYS_FUNCTION_H
#define SYS_FUNCTION_H
extern void Correct_Press_Value();
extern void Correct_Zero_Value();   //修正零点数值
extern void Charge_Sample_Speed();
extern void keypad_Write_Setting_To_Flash(void) ;
extern void keypad_Read_Setting_FormFlash(void) ;
extern unsigned int average_fun(unsigned int * array,unsigned int start_num,unsigned int end_num);  //50us
extern void delay(void);
extern void delay2(int num);
#endif