/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ipcc.h"
#include "openamp.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "wire.h"
#include "string.h"
#include "ipcc.h"
#include "virt_uart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_BUFFER_SIZE (512) //RPMSG_BUFFER_SIZE;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
VIRT_UART_HandleTypeDef huart0;
VIRT_UART_HandleTypeDef huart1;

__IO FlagStatus VirtUart0RxMsg = RESET;
uint8_t VirtUart0ChannelBuffRx[16][16];
uint16_t VirtUart0ChannelRxSize = 0;
uint16_t VirtUart0RxMsgBeg = 0;
uint16_t VirtUart0RxMsgEnd = 0;

__IO FlagStatus VirtUart1RxMsg = RESET;
uint8_t VirtUart1ChannelBuffRx[16][16];
uint16_t VirtUart1ChannelRxSize = 0;

float temp = 24.f;
float dirt = 50.f;
float watt;
float flow;
float coef = 20.f;
float sim_speed = 20.f;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void VIRT_UART0_RxCpltCallback(VIRT_UART_HandleTypeDef *huart);
void VIRT_UART1_RxCpltCallback(VIRT_UART_HandleTypeDef *huart);

void sim_flow();
void sim_heating();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  	// HAL_Delay(Delay)
  /* USER CODE END Init */

  if(IS_ENGINEERING_BOOT_MODE())
  {
    /* Configure the system clock */
    SystemClock_Config();
  }
  else
  {
    /* IPCC initialisation */
    MX_IPCC_Init();
    /* OpenAmp initialisation ---------------------------------*/
    MX_OPENAMP_Init(RPMSG_REMOTE, NULL);
  }

  /* USER CODE BEGIN SysInit */
  log_info("Beginning Cortex-M4 peripherials initialization\r\n")
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_UART7_Init();
  /* USER CODE BEGIN 2 */

  log_info("Virtual UART0 OpenAMP-rpmsg channel creation\r\n");
  if (VIRT_UART_Init(&huart0) != VIRT_UART_OK)
  {
	  log_err("VIRT_UART_Init UART0 failed.\r\n");
	  Error_Handler();
  }

  log_info("Virtual UART1 OpenAMP-rpmsg channel creation\r\n");
  if (VIRT_UART_Init(&huart1) != VIRT_UART_OK)
  {
	  log_err("VIRT_UART_Init UART1 failed.\r\n");
	  Error_Handler();
  }

  if(VIRT_UART_RegisterCallback(&huart0, VIRT_UART_RXCPLT_CB_ID, VIRT_UART0_RxCpltCallback) != VIRT_UART_OK)
  {
	  Error_Handler();
  }
  if(VIRT_UART_RegisterCallback(&huart1, VIRT_UART_RXCPLT_CB_ID, VIRT_UART1_RxCpltCallback) != VIRT_UART_OK)
  {
	  Error_Handler();
  }

//  if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED) != HAL_OK)
//  {
//    /* Calibration Error */
//    Error_Handler();
//  }

//  if(HAL_ADC_Start(&hadc1) != HAL_OK)
//  {
//    Error_Handler();
//  }



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  OPENAMP_check_for_message();

	  sim_flow();
	  sim_heating();

//	  if(VirtUart0RxMsgBeg != VirtUart0RxMsgEnd)
//	  {
//		  char strbuf[32];
//
//		  char cmd[] = "XXX:";
//		  size_t i = VirtUart0RxMsgBeg;
//		  memcpy(cmd, VirtUart0ChannelBuffRx[i], strlen(cmd));
//		  if(strcmp(cmd, "SET:") == 0)
//		  {
//			  char var[] = "YYYY:";
//			  memcpy(var, VirtUart0ChannelBuffRx[i] + 4, strlen(var));
//			  if(strcmp(var, "TEMP:") == 0)
//			  {
//				  temp = (float)atoi(VirtUart0ChannelBuffRx[i] + 9) / 10.f;
//			  }
//			  memcpy(var, VirtUart0ChannelBuffRx[i] + 4, strlen(var));
//			  if(strcmp(var, "DIRT:") == 0)
//			  {
//				  dirt = (float)atoi(VirtUart0ChannelBuffRx[i] + 9) / 10.f;
//				  if(dirt < 0.f) dirt = 0.f;
//				  if(dirt > 100.f) dirt = 100.f;
//			  }
//			  if(strcmp(var, "WATT:") == 0)
//			  {
//				  watt = (float)atoi(VirtUart0ChannelBuffRx[i] + 9) / 10.f;
//				  if(watt < 0.f) watt = 0.f;
//
//			  }
//			  if(strcmp(var, "FLOW:") == 0)
//			  {
//				  flow = (float)atoi(VirtUart0ChannelBuffRx[i] + 9) / 10.f;
//				  if(flow < 0.f) dirt = 0.f;
//				  if(flow > 100.f) dirt = 100.f;
//			  }
//			  if(strcmp(var, "COEF:") == 0)
//			  {
//				  flow = (float)atoi(VirtUart0ChannelBuffRx[i] + 9) / 10.f;
//				  if(coef < 0.f) coef = 0.f;
//			  }
//		  }
//		  else if(strcmp(cmd, "GET:") == 0)
//		  {
//			  char var[] = "YYYY";
//			  memcpy(var, VirtUart0ChannelBuffRx[i] + 4, strlen(var));
//			  if(strcmp(var, "TEMP") == 0)
//			  {
//				  int temp1 = (int)temp;
//				  int temp2 = (int)((temp - (float)temp1) * 10);
//
//				  sprintf(strbuf, "%i.%i\r\n", temp1, temp2);
//
//				  VIRT_UART_Transmit(&huart0, strbuf, strlen(strbuf));
//			  }
//			  memcpy(var, VirtUart0ChannelBuffRx[i] + 4, strlen(var));
//			  if(strcmp(var, "DIRT") == 0)
//			  {
//				  int dirt1 = (int)dirt;
//				  int dirt2 = (int)((dirt - (float)dirt1) * 10);
//
//				  sprintf(strbuf, "%i.%i\r\n", dirt1, dirt2);
//
//				  VIRT_UART_Transmit(&huart0, strbuf, strlen(strbuf));
//			  }
//		  }
//		  VirtUart0RxMsgBeg++;
//		  if(VirtUart0RxMsgBeg >= 16) VirtUart0RxMsgBeg = 0;
//	  }

	  HAL_Delay(10);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_MEDIUMHIGH);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_CSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS_DIG;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.CSIState = RCC_CSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL2.PLLSource = RCC_PLL12SOURCE_HSE;
  RCC_OscInitStruct.PLL2.PLLM = 3;
  RCC_OscInitStruct.PLL2.PLLN = 66;
  RCC_OscInitStruct.PLL2.PLLP = 2;
  RCC_OscInitStruct.PLL2.PLLQ = 1;
  RCC_OscInitStruct.PLL2.PLLR = 1;
  RCC_OscInitStruct.PLL2.PLLFRACV = 0x1400;
  RCC_OscInitStruct.PLL2.PLLMODE = RCC_PLL_FRACTIONAL;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL3.PLLSource = RCC_PLL3SOURCE_HSE;
  RCC_OscInitStruct.PLL3.PLLM = 2;
  RCC_OscInitStruct.PLL3.PLLN = 34;
  RCC_OscInitStruct.PLL3.PLLP = 2;
  RCC_OscInitStruct.PLL3.PLLQ = 17;
  RCC_OscInitStruct.PLL3.PLLR = 37;
  RCC_OscInitStruct.PLL3.PLLRGE = RCC_PLL3IFRANGE_1;
  RCC_OscInitStruct.PLL3.PLLFRACV = 6660;
  RCC_OscInitStruct.PLL3.PLLMODE = RCC_PLL_FRACTIONAL;
  RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** RCC Clock Config
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_ACLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3|RCC_CLOCKTYPE_PCLK4
                              |RCC_CLOCKTYPE_PCLK5;
  RCC_ClkInitStruct.AXISSInit.AXI_Clock = RCC_AXISSOURCE_PLL2;
  RCC_ClkInitStruct.AXISSInit.AXI_Div = RCC_AXI_DIV1;
  RCC_ClkInitStruct.MCUInit.MCU_Clock = RCC_MCUSSOURCE_PLL3;
  RCC_ClkInitStruct.MCUInit.MCU_Div = RCC_MCU_DIV1;
  RCC_ClkInitStruct.APB4_Div = RCC_APB4_DIV2;
  RCC_ClkInitStruct.APB5_Div = RCC_APB5_DIV4;
  RCC_ClkInitStruct.APB1_Div = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2_Div = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB3_Div = RCC_APB3_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Set the HSE division factor for RTC clock
  */
  __HAL_RCC_RTC_HSEDIV(24);
}

/* USER CODE BEGIN 4 */
void VIRT_UART0_RxCpltCallback(VIRT_UART_HandleTypeDef *huart)
{
    //log_info("Msg received on VIRTUAL UART0 channel:  %s \n\r", (char *) huart->pRxBuffPtr);
	char buff[16]; //VirtUart0ChannelBuffRx[VirtUart0RxMsgEnd]
    /* copy received msg in a variable to sent it back to master processor in main infinite loop*/
    VirtUart0ChannelRxSize = huart->RxXferSize < MAX_BUFFER_SIZE? huart->RxXferSize : MAX_BUFFER_SIZE-1;
    memcpy(buff, huart->pRxBuffPtr, VirtUart0ChannelRxSize);
    //VirtUart0RxMsg = SET;
//    VirtUart0RxMsgEnd++;
//    if(VirtUart0RxMsgEnd >= 16) VirtUart0RxMsgEnd = 0;
    char cmd[] = "XXX:";
	memcpy(cmd, buff, strlen(cmd));
	if(strcmp(cmd, "SET:") == 0)
	{
	  char var[] = "YYYY:";
	  memcpy(var, buff + 4, strlen(var));
	  if(strcmp(var, "TEMP:") == 0)
	  {
		  temp = (float)atoi(buff + 9) / 10.f;
	  }
	  memcpy(var, buff + 4, strlen(var));
	  if(strcmp(var, "DIRT:") == 0)
	  {
		  dirt = (float)atoi(buff + 9) / 10.f;
		  if(dirt < 0.f) dirt = 0.f;
		  if(dirt > 100.f) dirt = 100.f;
	  }
	  if(strcmp(var, "WATT:") == 0)
	  {
		  watt = (float)atoi(buff + 9) / 10.f;
		  if(watt < 0.f) watt = 0.f;

	  }
	  if(strcmp(var, "FLOW:") == 0)
	  {
		  flow = (float)atoi(buff + 9) / 10.f;
		  if(flow < 0.f) flow = 0.f;
		  if(flow > 100.f) flow = 100.f;
	  }
	  if(strcmp(var, "COEF:") == 0)
	  {
		  //flow = (float)atoi(buff + 9) / 10.f;
		  //if(coef < 0.f) coef = 0.f;
	  }
	}
	else if(strcmp(cmd, "GET:") == 0)
	{
	  char strbuf[16];
	  char var[] = "YYYY";
	  memcpy(var, buff+ 4, strlen(var));
	  if(strcmp(var, "TEMP") == 0)
	  {
		  int temp1 = (int)temp;
		  int temp2 = (int)((temp - (float)temp1) * 10);

		  sprintf(strbuf, "%i.%i\r\n", temp1, temp2);

		  VIRT_UART_Transmit(&huart0, strbuf, strlen(strbuf));
	  }
	  memcpy(var, buff + 4, strlen(var));
	  if(strcmp(var, "DIRT") == 0)
	  {
		  int dirt1 = (int)dirt;
		  int dirt2 = (int)((dirt - (float)dirt1) * 10);

		  sprintf(strbuf, "%i.%i\r\n", dirt1, dirt2);

		  VIRT_UART_Transmit(&huart0, strbuf, strlen(strbuf));
	  }
	}
}

void VIRT_UART1_RxCpltCallback(VIRT_UART_HandleTypeDef *huart)
{
    log_info("Msg received on VIRTUAL UART1 channel:  %s \n\r", (char *) huart->pRxBuffPtr);

    /* copy received msg in a variable to sent it back to master processor in main infinite loop*/
    VirtUart1ChannelRxSize = huart->RxXferSize < MAX_BUFFER_SIZE? huart->RxXferSize : MAX_BUFFER_SIZE-1;
    memcpy(VirtUart1ChannelBuffRx, huart->pRxBuffPtr, VirtUart1ChannelRxSize);
    VirtUart1RxMsg = SET;
}


void sim_flow()
{
	static uint32_t last_t = 0;
	uint32_t dt = HAL_GetTick() - last_t;
	dt *= sim_speed;
	if(dt >= 10000)
	{
		dirt += 1.0f;
		last_t = HAL_GetTick();
	}
	static uint32_t last_t2 = 0;
	uint32_t dt2 = HAL_GetTick() - last_t2;
	dt2 *= sim_speed;
	if(dt2 >= 1000)
	{
		dirt -= (flow / 10.f);
		last_t2 = HAL_GetTick();
	}

	if(dirt < 10.f) dirt = 10.f;
    if(dirt > 100.f) dirt = 100.f;
}

void sim_heating()
{
	static uint32_t last_t = 0;
	static float mass = 150.f;
	static float heat_cap = 1000.f;
	static float air_temp = 24.f;
	static float area = 1.9f;

	if(last_t == 0)
	{
		last_t = HAL_GetTick();
		return;
	}

	float dt = (HAL_GetTick() - last_t) / 1000.f;
	dt *= sim_speed;
	last_t = HAL_GetTick();

	float coef1 = heat_cap * mass; //heat capacity of water made into J/dK - dK to zmiana temperatury w medium
	float coef2 = coef * area; //convection, made into W/dK - dK to roznica temperatury otoczenia i medium
	float dtemp1 = watt * dt / coef1;
	float dtemp2 = coef2 * (temp - air_temp) * dt / coef1; //oddawana moc do otoczenia na jule na zmiane temperatury w wodzie

	temp = temp + dtemp1 - dtemp2;
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
	log_err("Critical Error Cortex-M4 is down\r\n");
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
