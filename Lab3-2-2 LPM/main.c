
//******************************************************************************
//   MSP430f5529  - 进入低功耗模式1实验例程
//   配置 ACLK = LFXT1 = 32KHZ，MCLK = SMCLK = 默认 DCO
//   禁用 VUSB LDO 和 SLDO ，  禁用SVS, 禁用REF0;
//   main.c
//******************************************************************************
#include <msp430.h>

unsigned int i;          								//定义定时变量
unsigned char flag;     								//设置按键标志位
void  IOInit()
{
	 // 进行端口设置，降低功耗
	  P1REN |= BIT2;                     //使能S1的上下拉电阻
	  P1OUT |= BIT2;										 //设S1为上拉状态
	  P1REN |=  BIT3;                    // 使能P1.3上下拉电阻
	  P1OUT |=  BIT3;                    // P1.3口置高电平
	  P1IES &= ~BIT3;                    // 中断沿设置（下降沿触发）
	  P1IFG &= ~BIT3;                    // 清P1.3中断标志
	  P1IE  |=  BIT3;                    // 使能P1.3口中断
	  P2OUT = 0x00;
	  P3OUT = 0x00;
	  P4OUT = 0x00;
	  P5OUT = 0x00;
	  P6OUT = 0x00;
	  P7OUT = 0x00;
	  P8OUT = 0x02;                      //L1亮
	  PJOUT = 0x00;

	  P2DIR = 0xFF;
	  P3DIR = 0xFF;
	  P4DIR = 0xFF;
	  P5DIR = 0xFF;
	  P6DIR = 0xFF;
	  P7DIR = 0xFF;
	  P8DIR = 0xFF;
	  PJDIR = 0xFF;
}
int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;            					// 关闭看门狗
  P5SEL |= BIT4+BIT5;                  					//P5.4和P5.5外设功能，选择XT1
  UCSCTL6 &= ~(XT1OFF);                					// 配置寄存器打开XT1振荡器
  UCSCTL6 |= XCAP_3;                   					//内部电容选择
  TA0CTL |= MC_1 + TASSEL_2 + TACLR;   					//时钟为SMCLK,比较模式，开始时清零计数器
  TA0CCTL0 = CCIE;						         					//比较器中断使能
  TA0CCR0  = 50000;						           					//比较值设为50000，相当于50ms的时间间隔
  do 																						// 循环直到XT1和DOC振荡器稳定
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);	// 清除XT1、XT2、DOC故障标志位
    SFRIFG1 &= ~OFIFG;                     		  // 清除OSC故障标志位
  }
  while (SFRIFG1&OFIFG);                   			// 等待振荡器稳定
  UCSCTL6 &= ~(XT1DRIVE_3);                 		// 振荡器已稳定，减少驱动器
  IOInit();                                			//IO口初始化
  // 禁止 VUSB 和  SLDO的设置
  USBKEYPID   =     0x9628;           					// 设置USBKEYPID寄存器值为0x9628,USBKEYPID寄存器解锁

  USBPWRCTL &= ~(SLDOEN+VUSBEN);      					//  配置USB电源控制寄存器禁用 VUSB LDO和SLDO
  USBKEYPID   =    0x9600;            					// 非ox9628则关闭访问USB配置寄存器

  // 禁用 SVS
  PMMCTL0_H = PMMPW_H;                					// PMM 密码
  SVSMHCTL &= ~(SVMHE+SVSHE);        						// 禁用高压侧SVS
  SVSMLCTL &= ~(SVMLE+SVSLE);         					// 禁用低压侧SVS
  while(1)
      {
      	if((P1IN & BIT2) == 0)									//检测S1按下
      		{
      		flag = 1;                   					//按键标志
      		P8OUT &= ~ BIT1;            				  //L1灭
      		__bis_SR_register(LPM1_bits+ GIE);    // 进入低功耗模式1,开中断
      		}
      }

  return 0;
}

// P1中断函数
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	  flag = 0;																	 //按键标志
	  LPM1_EXIT;                             		 //退出低功耗模式
	  P8OUT |= BIT1;         			     					 //L1亮
	  P1IFG &= ~BIT3;                        		 //清P1.3中断标志位
	  __disable_interrupt();

}
/************************定时器中断函数********************************/
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)

{
	i++;
	if(flag == 1)                         			 //判断按键
	{
		if(i==1000)
		{
			LPM1_EXIT;                              //退出低功耗
			P8OUT ^= BIT1;							 						//形成闪灯效果
			__bis_SR_register(LPM1_bits+ GIE);      // 进入低功耗模式1,开中断


		}
		if(i==2000)
		{
			LPM1_EXIT;														 //退出低功耗
			i = 0;						 										 //i清零
			P8OUT ^= BIT1;							 					 //形成闪灯效果
			__bis_SR_register(LPM1_bits+ GIE);     // 进入低功耗模式1,开中断

		}
	}
	else
		if(i > 2000)
			i=0;																	//i清零
}


