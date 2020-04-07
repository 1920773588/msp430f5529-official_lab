#include <msp430.h> 
#include <stdio.h>
#include "Paper_Display.h"
#define TimesNewRoman 0
#define Arial 1
#define ArialBlack 2
#define size8 0
#define size16 1
volatile unsigned char DisBuffer[250*16];
unsigned char recv_data,recv_byte[2];
unsigned char str[] = {'0','0','0','0','0','0','0','0','0','\0'};
unsigned int number = 0,index = 0,flag = 0;
void initClock()
{
	 UCSCTL6 &= ~XT1OFF; //启动XT1
	 P5SEL |= BIT2 + BIT3; //XT2引脚功能选择
	 UCSCTL6 &= ~XT2OFF;          //打开XT2 = 4MHz
	 __bis_SR_register(SCG0);
	 UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
	 UCSCTL1 = DCORSEL_4;       //DCO频率范围在28.2MHZ以下
	 UCSCTL2 = FLLD_4 + 1;       //D=16，N=1
	 UCSCTL3 = SELREF_5 + FLLREFDIV_3;    //n=8,FLLREFCLK时钟源为XT2CLK；DCOCLK=D*(N+1)*(FLLREFCLK/n) = 16MHz;DCOCLKDIV=(N+1)*(FLLREFCLK/n) = 1MHz;
	 UCSCTL4 = SELA_4 + SELS_3 +SELM_3;    //ACLK的时钟源为DCOCLKDIV,MCLK\SMCLK的时钟源为DCOCLK
	 UCSCTL5 = DIVA_5 +DIVS_1;      //ACLK由DCOCLKDIV的32分频得到，SMCLK由DCOCLK的2分频得到
	             //最终MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ

	 __bic_SR_register(SCG0);                   //Enable the FLL control loop

}

void test(){
    int i=0;
    for(i=1;i<250;i++)
        DisBuffer[i*16-1]=0xff;
}

void ini_UART(){
          P4SEL |= BIT5+BIT4;                       // P4.4,5使用外设功能 = UCA1 TXD/RXD
          UCA1CTL1 |= UCSWRST;                      // 复位USCI
          UCA1CTL1 |= UCSSEL__SMCLK;                     // 设置SMCLK时钟，用于发生特定波特率
          UCA1BR0 = 4;                            // 设置波特率， 8MHz 波特率= 115200
          UCA1BR1 = 0;
          UCA1MCTL |= 0x51;//UCBRS_1 + UCBRF_0;
          UCA1CTL1 &= ~UCSWRST;                     // 结束复位
          UCA1IE |= UCRXIE;
}
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
   // P3DIR |= BIT7;
    P8DIR |= BIT1;
    //P8OUT &=~ BIT1;
    initClock();
    ini_UART();
    PaperIO_Int();
    INIT_SSD1673();
  //  test();
   // Init_buff();
//    display("                        ", 42, 0,TimesNewRoman,size8,1,0);
 //  display("msp430 f5529", 30, 32,TimesNewRoman,size16,0,0);
//    display("0123 4 5",0, 90,TimesNewRoman,size16,0,0);
 //   display("012abcABC", 0,50,Arial,size8,1,0);
//    display("012abcABC",80,50,Arial,size16,1,0);
//    display("012abcABC", 0,90,ArialBlack,size8,1,0);
//    display("012abcABC",80,90,ArialBlack,size16,1,0);
//    SetpointXY(1,100,1,100);
   // DIS_IMG(1);
    DIS_IMG(2);
   // DIS_IMG(3);
   // DIS_IMG(5);
    __bis_SR_register(GIE);
}

/*
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    P3OUT ^= BIT7;
    char temp ;
    switch(__even_in_range(UCA1IV,4))
      {
      case 0:break;                             // Vector 0 - no interrupt
      case 2://数据接收中断
      //while(!(UCA1IFG&UCTXIFG));
      UCA1TXBUF = UCA1RXBUF;//将接收缓存器的字符产送给发送缓存器

      temp = UCA1RXBUF;
      if( temp == 'a'){
        P8OUT ^= BIT1;
      }
     // display(&temp, 30, 32,TimesNewRoman,size8,0,0);
      break;
      case 4:break;                             // Vector 4 - TXIFG
      default: break;
      }
}
*/

/*
#pragma vector=USCI_A1_VECTOR                                       //USCI中断服务函数
__interrupt void USCI_A1_ISR(void)
{
switch(__even_in_range(UCA1IV,4))
{
case 0:break;                                                   //无中断
case 2:                                                         //接收中断处理
    {
        UCA1TXBUF = UCA1RXBUF;
        if(UCRXIFG)                                             //等待完成接收
        {
            DisBuffer[number] = UCA1RXBUF;     //数据读出
            number++;
            P8OUT ^= BIT1;
            if(number == 400){
                DIS_IMG(1);
                number = 0;
                clear_buff();
            }
        }
    }
    break;
case 4:break;                                                   //发送中断不处理
default:break;                                                  //其他情况无操作

}
}*/


unsigned int H_to_D(unsigned char c){
    switch(c){
      case 0x41:case 0x61: return 10;break;
      case 'B':case 'b': return 11;break;
      case 'C':case 'c': return 12;break;
      case 'D':case 'd': return 13;break;
      case 'E':case 'e': return 14;break;
      case 'F':case 'f': return 15;break;
      default: return c-0x30;
    }
}
#pragma vector=USCI_A1_VECTOR                                       //USCI中断服务函数
__interrupt void USCI_A1_ISR(void)
{
switch(__even_in_range(UCA1IV,4))
{
case 0:break;                                                   //无中断
case 2:                                                         //接收中断处理
    {
        if(UCRXIFG)                                             //等待完成接收
        {UCA1TXBUF = UCA1RXBUF;
            if(flag){
                recv_byte[index] = UCA1RXBUF;     //数据读出
                index ++;
                if(index == 2){
                    index = 0;
                    DisBuffer[number] = H_to_D(recv_byte[0])*16+H_to_D(recv_byte[1]);
                    number++;
                }
                P8OUT ^= BIT1;
                if(number == 4000){
                    DIS_IMG(1);
                    flag = 0;
                }
            }
            else if(UCA1RXBUF == '~'){
                flag = 1;
                number = 0;
                clear_buff();
            }

        }
    }
    break;
case 4:break;                                                   //发送中断不处理
default:break;                                                  //其他情况无操作

}
}



