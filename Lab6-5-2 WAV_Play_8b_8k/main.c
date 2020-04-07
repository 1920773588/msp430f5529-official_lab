#include <msp430.h>
#include "ad.h"
#include "dac7571.h"
#include "IIC.h"
#include "dr_sdcard.h"
#include "dr_sdcard/integer.h"
#include "dr_sdcard/ff.h"
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "dr_sdcard/HAL_SDCard.h"
#include "dr_tft.h"
#include "task.h"
#include "Paper_Display.h"


//typedef unsigned int    uint16_t;
UINT br;


void initClock()
{
	P5SEL |= BIT4+BIT5;                       					// ѡ�� XT1
		UCSCTL6 &= ~(XT1OFF);                     					// XT1 ����
		UCSCTL6 |= XCAP_3;                        					// Internal load cap
		P5SEL |= BIT2 + BIT3; 										//XT2���Ź���ѡ��
		UCSCTL6 &= ~XT2OFF; 										//����XT2
		while (SFRIFG1 & OFIFG)
		{ 															//�ȴ�XT1��XT2��DCO�ȶ�
			UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT2OFFG);
			SFRIFG1 &= ~OFIFG;
		}

		UCSCTL4 = SELA__XT1CLK + SELS__XT2CLK + SELM__XT2CLK; 		//����DCO�������ܷ�

		UCSCTL1 = DCORSEL_5; 										//6000kHz~23.7MHz
		UCSCTL2 = 20000000 / (4000000 / 8); 						//XT2Ƶ�ʽϸߣ���Ƶ����Ϊ��׼�ɻ�ø��ߵľ���
		UCSCTL3 = SELREF__XT2CLK + FLLREFDIV__8; 					//XT2����16��Ƶ����Ϊ��׼
		while (SFRIFG1 & OFIFG)
		{ 															//�ȴ�XT1��XT2��DCO�ȶ�
			UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT2OFFG);
			SFRIFG1 &= ~OFIFG;
		}
		UCSCTL5 = DIVA__1 + DIVS__1 + DIVM__1; 						//�趨����CLK�ķ�Ƶ
		UCSCTL4 = SELA__XT1CLK + SELS__DCOCLK + SELM__DCOCLK; 		//�趨����CLK��ʱ��Դ
}

#define BUFFER_SIZE 2048
#define Buf 3
#define Speed 150
uint8_t buffer[BUFFER_SIZE];



FRESULT rc;                                       					/* Result code */
FATFS fatfs;                                      					/* File system object */
DIRS dir;                                         					/* Directory object */
FILINFO fno;                                      					/* File information object */
FIL fil;                                          					/* File object */

#define FBUF_SIZE 256
char fbuf[FBUF_SIZE];

void strupr(char* str)
{
  char *ptr = str;
  while(!*ptr)
  {
    if(*ptr >= 'a' && *ptr <='z')
      *ptr -= 'a' - 'A';
    ptr++;
  }
}

int __low_level_init(void)
{
  WDTCTL = WDTPW + WDTHOLD ;//stop wdt
  return 1;
}



int main( void )
{

  WDTCTL = WDTPW + WDTHOLD;												//�ؿ��Ź�
  _DINT();
  initClock();
//  PaperIO_Int();
//  INIT_SPD2701();
//  display(0,1);

  unsigned char res,clear;
  unsigned int count;
  unsigned int MusicData;
  FILINFO finfo;			 											/*�ļ���Ϣ*/
  DIRS dirss;
  char path[100] = {""};
  char FileName[100][13];
  char *result3;
  unsigned char PlayNum=0;
  unsigned char MusicNum;
   volatile unsigned int value = 0;										//�����жϱ���

  
  IOInit();

  _EINT();
  AD_Init();															//AD��ʼ��
  SDCard_init();														//SD��ʼ��
  TimeInit();															//��ʱ����ʼ��
  f_mount(0, &fatfs);													//�ļ�ϵͳ����
  if (f_opendir(&dirss,path) == FR_OK)									//��Ŀ¼
  {
  	PlayNum=0;
	while (f_readdir(&dirss, &finfo) == FR_OK)    						/* ���ζ�ȡ�ļ��� */
	{
		if ( finfo.fattrib & AM_ARC )       							/* �ж��Ƿ�Ϊ�浵���ĵ� */
		{
				if( !finfo.fname[0] )	 								/* �ļ���Ϊ�ռ�������Ŀ¼��ĩβ���˳� */
				break; 
				result3 = strstr( finfo.fname, ".WAV" );
				if(result3!=NULL)
				{
					memcpy(&FileName[PlayNum][0],&finfo.fname,sizeof(finfo.fname));
					PlayNum++;

				}

		}
	}
	MusicNum=PlayNum;
	PlayNum=0;
	while(1)
	{
		memcpy(&finfo.fname,&FileName[PlayNum][0],13);			//��ȡ�ļ�����
		res=f_open(&fil,finfo.fname,FA_OPEN_EXISTING|FA_READ);
		br=1;
		clear=0;
		while(1)
	    {
		  res=f_read(&fil,buffer,(sizeof(buffer)),&br);
		  if(br==0)
			  break;
		  while(count<2048)
		  {
			  if(clear==0)
			  {
				  clear=1;
				  break;
			  }
			 if(KeyFlag.S1==0)
			 {
				  MusicData=buffer[count];
//				  value = ADC12MEM0;
				  MusicData = MusicData<<Buf;
				  delay1(Speed);
				  DACValue(MusicData);
				  count++;
			 }
		  }
		  //DACValue(0);
		  if(KeyFlag.S2==1)
		  {
		  	KeyFlag.S2=0;
			break;
		  }
		  if(KeyFlag.S3==1)
		  {
			break;
		  }
		  count=0;
	  	}
		f_close(&fil);
	    if(KeyFlag.S3==1)
	    {
			KeyFlag.S3=0;
			if(PlayNum>=1)
				PlayNum-=1;
			else
				PlayNum=MusicNum-1;
			
	    }
	    else
		PlayNum++;
		if(PlayNum>=MusicNum)
			PlayNum=0;
	}
  }
}
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)

{
	ScanKey();
}


