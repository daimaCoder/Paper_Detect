/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t Flash_addr = 0x08008000;
uint32_t adc_value = 0;
uint32_t adc[100],adc_sum = 0;
float vol,limit = 0;
int vol_f;
uint8_t flag = 0 ,n1 = 0,n2 = 0,n3 = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void clear_flash(void);   //����FLASH 
void write_flash(void);   //дFLASH
void read_flash(void);  //��FLASH
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//��ԭ����ʱ��������systic�ж�
void delay_us(uint32_t nus)
{
   uint32_t temp;
   SysTick->LOAD = 9*nus;
   SysTick->VAL=0X00;//��ռ�����
   SysTick->CTRL=0X01;//ʹ�ܣ����������޶����������ⲿʱ��Դ
   do
   {
     temp=SysTick->CTRL;//��ȡ��ǰ������ֵ
   }while((temp&0x01)&&(!(temp&(1<<16))));//�ȴ�ʱ�䵽��
   SysTick->CTRL=0x00; //�رռ�����
   SysTick->VAL =0X00; //��ռ�����
}
void delay_ms(uint16_t nms)
{
   uint32_t temp;
   SysTick->LOAD = 9000*nms;
   SysTick->VAL=0X00;//��ռ�����
   SysTick->CTRL=0X01;//ʹ�ܣ����������޶����������ⲿʱ��Դ
   do
  {
    temp=SysTick->CTRL;//��ȡ��ǰ������ֵ
   }while((temp&0x01)&&(!(temp&(1<<16))));//�ȴ�ʱ�䵽��
   SysTick->CTRL=0x00; //�رռ�����
   SysTick->VAL =0X00; //��ռ�����
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
	OLED_Init();
	read_flash();  //��FLASH
	if(n1 == 0xff)
		n1 = 0;
	if(n2 == 0xff)
		n2 = 0;
  if(n3 == 0xff)
		n3 = 0;
	OLED_ShowString(0,0,"PWM:OFF");
	OLED_ShowCHinese(0,2,7);//��
	OLED_ShowCHinese(18,2,8);//ѹ
	OLED_ShowChar(37,2 ,':');
  OLED_ShowNum(0,4,n1,1,16);
	OLED_ShowChar(9,4 ,'.');
	OLED_ShowNum(16,4,n2,1,16);
	OLED_ShowNum(24,4,n3,1,16);
	OLED_ShowChar(0,6 ,'^');OLED_ShowChar(16,6 ,' ');OLED_ShowChar(24,6 ,' ');
	limit = n1 + n2/10.0 + n3/100.0;	
	HAL_ADCEx_Calibration_Start(&hadc1);  //��У׼	
	//HAL_ADC_Start_DMA(&hadc1, &adc_value, 1);
  delay_ms(5);
	uint16_t i;
//	HAL_TIM_PWM_Start(&htim2 , TIM_CHANNEL_1 );
//	delay_us(5);
//	HAL_TIM_PWM_Start(&htim1 , TIM_CHANNEL_1 );
//	
//	
//	delay_ms(10);
//	HAL_TIM_PWM_Stop(&htim2 , TIM_CHANNEL_1 );
//	delay_us(5);
//	HAL_TIM_PWM_Stop(&htim1 , TIM_CHANNEL_1 );
//	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
//		OLED_ShowCHinese(0,0,0);//?
//		OLED_ShowCHinese(18,0,1);//?
//		OLED_ShowCHinese(36,0,2);//?
//		OLED_ShowCHinese(54,0,3);//?
//		OLED_ShowCHinese(72,0,4);//?
//		OLED_ShowCHinese(90,0,5);//?
// 		OLED_ShowCHinese(108,0,6);//?
//		OLED_ShowString(0,2,"1.3' OLED TEST");
//		OLED_ShowString(8,2,"ZHONGJINGYUAN");  
//	 	OLED_ShowString(20,4,"2014/05/01");  
//		OLED_ShowString(0,6,"ASCII:");  
//		OLED_ShowString(63,6,"CODE:");  
//		OLED_ShowChar(48,6,t);//??ASCII??	   
//		t++;
//		if(t>'~')t=' ';
		//OLED_ShowNum(103,6,101,3,16);//??ASCII?????
	 adc_sum = 0;
   for(i = 0;i < 100;i++)
	 {
    HAL_ADC_Start(&hadc1);                //����ADCת��
		HAL_ADC_PollForConversion(&hadc1,10); //�ȴ�ת�����
		adc[i] = HAL_ADC_GetValue(&hadc1);
		adc_sum = adc_sum + adc[i];
		delay_ms(5);
	 }	
		adc_value = adc_sum / 100;
		
//    HAL_UART_Transmit(&huart1,"llolllooo",8,10);
//		printf("hello");
  		vol = adc_value/4095.0 * 3.3;
			vol_f = (vol - (int)vol) * 100;
  		OLED_ShowNum(48,2,vol,1,16);
      OLED_ShowChar(56,2 ,'.');	
      OLED_ShowNum(64,2,vol_f,2,16);
//    OLED_ShowNum(0,6,adc_value,4,16);

    
      // OLED_ShowString(56,4,"101");    
//    printf("ADC value = %.4f\r\n",vol);  //��ӡ���
      if(vol > limit)
			{
				HAL_GPIO_WritePin(GPIOA, LED1_Pin, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(GPIOA, LED2_Pin, GPIO_PIN_SET);
			}
			else
			{
				HAL_GPIO_WritePin(GPIOA, LED1_Pin, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(GPIOA, LED2_Pin, GPIO_PIN_RESET);
			}
			
  		//delay_ms(100);
//    OLED_Clear();
		
		   
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/*******************�ⲿ�жϻص�����***********************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	 if(GPIO_Pin & KEY1_Pin)
	 {
//		  HAL_Delay(20);// ��ʱһС��ʱ�䣬�������� 
//    if(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == 0)//����1�ж�
//    {
	    if(__HAL_TIM_GET_COUNTER(&htim1) == 0 && __HAL_TIM_GET_COUNTER(&htim2) == 0)
			{
				HAL_TIM_PWM_Start(&htim2 , TIM_CHANNEL_1 );
	      delay_us(5); 
				for(int i;i>18000;i++)
				{
					__NOP();
				
				}
	      HAL_TIM_PWM_Start(&htim1 , TIM_CHANNEL_1 );
				OLED_ShowString(0,0,"PWM: ON");
			}   	 
		  else
			{			
	      HAL_TIM_PWM_Stop(&htim1 , TIM_CHANNEL_1 );
				HAL_TIM_PWM_Stop(&htim2 , TIM_CHANNEL_1 );
	      __HAL_TIM_SET_COUNTER(&htim1 ,0);
				__HAL_TIM_SET_COUNTER(&htim2 ,0);
				OLED_ShowString(0,0,"PWM:OFF");
			}	
//		 } //Ӳ���Ѿ���������ȥ���������
	  }
	 else if(GPIO_Pin & KEY2_Pin)
	 {
		   switch(flag)
			 {
				 case 0 :
					 flag = 1;OLED_ShowChar(0,6 ,' ');OLED_ShowChar(16,6 ,'^');OLED_ShowChar(24,6 ,' ');
				   break;
				 case 1 :
					 flag = 2;OLED_ShowChar(0,6 ,' ');OLED_ShowChar(16,6 ,' ');OLED_ShowChar(24,6 ,'^');
				   break;
				 case 2 :
					 flag = 0;OLED_ShowChar(0,6 ,'^');OLED_ShowChar(16,6 ,' ');OLED_ShowChar(24,6 ,' ');
				   break;
				 default:
					 break;
			 }
			// OLED_ShowNum(0,6,flag,1,16);
	 }
	 else if(GPIO_Pin & KEY3_Pin)
	 {
	   
		 switch(flag)
				{
					case 0:
						n1 = n1 + 1;
					  if(n1 == 4) 
							n1 = 0;
						break;
					case 1:
						n2 = n2 + 1;
					  if(n2 == 10)
							n2 = 0;
					  break;
					case 2:
						n3 = n3 + 1;
					  if(n3 == 10)
							n3 = 0;
					  break;
					default:
					 break;			
				}
			
		 OLED_ShowNum(0,4,n1,1,16);
		 OLED_ShowChar(9,4 ,'.');
		 OLED_ShowNum(16,4,n2,1,16);
		 OLED_ShowNum(24,4,n3,1,16);
		 limit = n1 + n2/10.0 + n3/100.0;
		 clear_flash();
		 write_flash();
		 
	 }
	 
	 __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
}

/*****************************************FLASH��غ���**************************************************/
void clear_flash(void)     //���FLASH
{
	HAL_FLASH_Unlock();
  FLASH_EraseInitTypeDef f;
  f.TypeErase = FLASH_TYPEERASE_PAGES;
  f.PageAddress = Flash_addr;
  f.NbPages = FLASH_BANK_1;
  uint32_t PageError = 0;
  HAL_FLASHEx_Erase(&f, &PageError);
	HAL_FLASH_Lock();
}


void write_flash(void)   //дFLASH
{
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_addr +  8, (uint32_t)n1);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_addr +  16, (uint32_t)n2);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_addr +  24, (uint32_t)n3);
	HAL_FLASH_Lock();
}

void read_flash(void)   //��FLASH
{
	n1 = *(__IO uint32_t*)(Flash_addr + 8);
  n2 = *(__IO uint32_t*)(Flash_addr + 16);
  n3 = *(__IO uint32_t*)(Flash_addr + 24);	
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
