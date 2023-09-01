/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "jansson.h"
//#include "retarget.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void _Add_array_to_json(json_t *obj,const char* name,const int *marr,size_t dim1,size_t dim2);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	json_t *root;
	json_t *jdata;
	
	json_t *Test_obj;
	json_t *Array_obj;
	json_t *item1_obj;
	json_t *item2_obj;
	
	json_t *_Test_obj;
	json_t *_Array_obj;
	json_t *_item1_obj;
	json_t *_item2_obj;	
	
	char *out;
	char *s;
	
	int array_1[2][3] = {{1,2,3},{4,5,6}};
	int array_2[4][4] = {{1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16}};
	json_error_t error;
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	root = json_pack("{}");
	
	out = json_dumps(root, JSON_ENCODE_ANY);
	printf("out:%s\r\n",out);
	free(root);
	free(out);
	
	root = json_pack("{sisi}", "foo", 42 ,"bar", 7);
	out = json_dumps(root, JSON_ENCODE_ANY);
	printf("out:%s\r\n",out);
	free(root);
	free(out);
	
	root = json_pack("{s:i,s:i}", "foo", 42 ,"bar", 7);
	out = json_dumps(root, JSON_ENCODE_ANY);
	printf("out:%s\r\n",out);
	free(root);
	free(out);	

  root = json_pack("[[i,i],{s:b}]", 1, 42 ,"cool", 7);
	out = json_dumps(root, JSON_ENCODE_ANY);
	printf("out:%s\r\n",out);
	free(root);
	free(out);
	
	char buffer[4] = {'t','e','s','t'};
	root = json_pack("s#",buffer,4);
	out = json_dumps(root, JSON_ENCODE_ANY);
	printf("out:%s\r\n", out);
	free(root);
	free(out);	
	
	root = json_pack("s++","foo","bar","baz");
	out = json_dumps(root, JSON_ENCODE_ANY);
	printf("out:%s\r\n", out);
	free(root);
	free(out);		
	
	root = json_pack("{s:s*,s:o*,s:O*}","foo",NULL,"bar",NULL,"baz",NULL);
	out = json_dumps(root, JSON_ENCODE_ANY);
	printf("out:%s\r\n", out);
	free(root);
	free(out);	
	
	root = json_pack("[s*,o*,O*]",NULL,NULL,NULL);
	out = json_dumps(root, JSON_ENCODE_ANY);
	printf("out:%s\r\n", out);
	free(root);
	free(out);		
	
	jdata = json_object();
	_Add_array_to_json(jdata,"arr1",&array_1[0][0],2,3);
	_Add_array_to_json(jdata,"arr2",&array_2[0][0],4,4);
  s = json_dumps(jdata, 0);
	puts(s);
	free(s);
	
	json_decref(jdata);
	json_decref(jdata);
	
	Test_obj = json_object();
	item1_obj = json_object();
	item2_obj = json_object();
	
	json_object_set_new(Test_obj,"Title",json_string("Hello World"));
  json_object_set_new(Test_obj,"name",json_string("Test_obj"));

	json_object_set_new(Test_obj,"num_1",json_true());
  json_object_set_new(Test_obj,"num_2",json_false());
	
	json_object_set_new(Test_obj,"num_3",json_integer(1));
  json_object_set_new(Test_obj,"num_4",json_real(1.55));
	
	json_object_set_new(item1_obj,"item1",json_string("item1_obj"));
  json_object_set_new(item2_obj,"item1",json_string("item2_obj"));
	
	Array_obj = json_array();
	
	json_array_append_new(Array_obj,item1_obj);
	json_array_append_new(Array_obj,item2_obj);
	
	json_object_set_new(Test_obj,"Children_1",Array_obj);
	
	out = json_dumps(Test_obj, JSON_ENCODE_ANY);
	printf("out:%s\r\n", out);	
	
	json_decref(Test_obj);
	
	_Test_obj = json_loads(out , 0, &error);
	_Array_obj = json_object_get(_Test_obj,"Children_1");
	int size = json_array_size(_Array_obj);
	
	_item1_obj = json_object_get(_Test_obj,"num1");
	printf("Test num1 type is : %d, val: %d,%f\r\n", json_typeof(_item1_obj),json_boolean_value(_item1_obj),json_number_value(_item1_obj));
	
	free(out);
	json_decref(_Test_obj);
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

/* USER CODE BEGIN 4 */
void _Add_array_to_json(json_t *obj,const char* name,const int *marr,size_t dim1,size_t dim2)
{
	size_t i,j;
	json_t* j_arr1 = json_array();
	for(i = 0;i < dim1; i++)
	{
		json_t* j_arr2 = json_array();
		for(j = 0;j < dim2; j++)
		{
			int val = marr[i*dim2 + j];
			json_t* jval = json_integer(val);
			json_array_append_new(j_arr2,jval);
		}
		json_array_append_new(j_arr1,j_arr2);
	}
	json_object_set_new(obj, name, j_arr1);
	return;
}

int fputc(int data,FILE *file)
{
	uint8_t ch = data;
	HAL_UART_Transmit(&huart1,&ch,1,100);
	return 1;
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
