/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//1. In reference manual we find CAN1 as master and CAN2 as slave. This is misleading as CAN bus is multi-master terminology.
//Here CAN1 referred as master because CAN2 does not have direct acess to SRAM.
//2. If we want to use CAN2 than we have to initialize CAN1 first.
//3. There are three transmit mailboxes and three receive fifo. Each mailboxes and receive fifo can hold three messages at the same time.
//4. Silent Mode: CAN tx is always maintained to recessive state. Tx line looped back to rx line. It just listens and doesn't change the bus sate
//by putting dominant bit.
//5. Loopback mode: can transmit frames on the bus. Frame looped back to the Rx line internally. Rx is disconnected from
//the bus. It does not listen to the bus. This mode ignores NACK and does not retransmit.
//6. Silent + loop back mode : both line disconnected from the bus. neither transmit nor listen to the bus. Tx line internally looped
//backed to rx line.
//7. Acceptance filter: reduces interrupt overhead. There are 28 filter banks. and each bank have two regitsters.
// Accept frames only if first 3 msb bits of the standard indenifier are 1s
// Filter bank 0: FB0_R1 : configurre as Mask mode
// Idenfifier register: 111xxxxxxxx(32:21) : fb0_r1
// Mask register: 111xxxxxxxx(32:21) : fb1_r1

//Accept frame only if first 3 msb bits are 0 and first 2 lsbs are 1.
//Identifier: 000xxxxxx11 : mask mode
//Mask regist : 111xxxxxx11

//Accept frame with exactly node 0x650 or 0x698
//List/Id mode
// Identifier1 : 0x650
// Identifier2: 0x698

//Accept only request frame
//set RTR bit recessive in identifier and mask register in mask mode.
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
 CAN_HandleTypeDef hcan;

/* USER CODE BEGIN PV */
void CAN_Tx(void);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void CAN_Tx()
{
	CAN_TxHeaderTypeDef TxHeader;
	uint32_t TxMailbox = 0;
	uint8_t message[5] = {1,2,3,4,5};

	TxHeader.DLC = 5;
	TxHeader.StdId = 0x780;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;

	if (HAL_CAN_AddTxMessage(&hcan,&TxHeader,message,&TxMailbox) != HAL_OK)
	{
		//Error_handler();
	}

	while (HAL_CAN_IsTxMessagePending(&hcan,TxMailbox)); //waits unitll it transmit // polling mode

}

void CAN_Rx()
{
	CAN_RxHeaderTypeDef RxHeader;
	uint8_t rcvd_data[8];

	while (! HAL_CAN_GetRxFifoFillLevel(&hcan,CAN_RX_FIFO0));

	if (HAL_CAN_GetRxMessage(&hcan,CAN_RX_FIFO0,&RxHeader,rcvd_data) != HAL_OK)
	{
		//handle the error
	}
}

void CAN_Filter()
{
	CAN_FilterTypeDef can_filter_init;

	can_filter_init.FilterActivation = ENABLE;
	can_filter_init.FilterBank = 0;
	can_filter_init.FilterFIFOAssignment = CAN_RX_FIFO0;
	can_filter_init.FilterIdHigh = 0;
	can_filter_init.FilterIdLow = 0;
	can_filter_init.FilterMaskIdHigh = 0;
	can_filter_init.FilterMaskIdLow = 0;
	can_filter_init.FilterMode = CAN_FILTERMODE_IDMASK;
	can_filter_init.FilterScale = CAN_FILTERSCALE_32BIT;

	if (HAL_CAN_ConfigFilter(&hcan,&can_filter_init) != HAL_OK)
	{
		//handle the error
	}

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
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */
  // MX_CAN_Init() function will configure can controller to Initialization state
  CAN_Filter();//this function should be called in initialization state
  if (HAL_CAN_Start(&hcan) != HAL_OK) // this will switch can controller to Initialization mode to normal mode
  {
	  //handle the error.
  }

  CAN_Tx();//for transmission and reception can controller should be in normal mode (look CAN NORMAL MODE is different than this)
  CAN_Rx();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
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

  /** Initializes the CPU, AHB and APB buses clocks
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
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */
//	  hcan.Instance = CAN1;
//	  hcan.Init.Prescaler = 9;
//	  hcan.Init.Mode = CAN_MODE_LOOPBACK;
//	  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
//	  hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
//	  hcan.Init.TimeSeg2 = CAN_BS2_1TQ; //here speed is 250k at 36mhz clock cycle
//	  hcan.Init.TimeTriggeredMode = DISABLE;
//	  hcan.Init.AutoBusOff = DISABLE;// once the error counter reaches certain limit the can controller will be off.
//	  hcan.Init.AutoWakeUp = DISABLE;
//	  hcan.Init.AutoRetransmission = ENABLE; //Enable auto retransmission
//	  hcan.Init.ReceiveFifoLocked = DISABLE; // if it is disabled once receive fifo is full the next incoming message will overwrite the prev one and if it is enabled new messages will be discarded.
//	  hcan.Init.TransmitFifoPriority = DISABLE;//defines priority of mailboxes messages (one mailbox can hold three messages) // if 0 then priority is driven by Identifier // if 1 then priority driven by request order

//	  if (HAL_CAN_Init(&hcan) != HAL_OK)
//	  {
//	    Error_Handler();
//	  }

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 16;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
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
