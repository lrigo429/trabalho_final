/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "display.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define yes 1
#define no 0
#define NUMBER_OF_CONVERSION 12
#define scala_temperatura (3.3/4095)/0.005
#define MAX_DUTY_CYCLE_VALUE_TIM2 3000
#define MAX_DUTY_CYCLE_VALUE_TIM1 3000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

DMA_HandleTypeDef hdma_memtomem_dma1_channel2;

/* USER CODE BEGIN PV */

//Variáveis para as saídas e entradas da cafeteira
uint16_t P1 = GPIO_PIN_5;//pino PB5
uint8_t state_P1;
uint16_t P2 = GPIO_PIN_4;//pino PB4
uint8_t state_P2;
uint16_t B1;//pino PA11
uint16_t R1;//pino PA15
uint16_t T1;//pino PB0
uint16_t CP1;//pino PB3
uint16_t T2;//pino PB1
uint16_t Y1 = GPIO_PIN_15;//pino PB15
uint16_t Y2 = GPIO_PIN_14;//pino PB14
uint16_t Y3 = GPIO_PIN_13;//pino PB13
uint16_t Y4 = GPIO_PIN_12;//pino PB12
uint8_t Bn1;
uint16_t Bn1_bit1 = GPIO_PIN_8;//pino PA8
uint16_t Bn1_bit2 = GPIO_PIN_9;//pino PA9
uint16_t Bn1_bit3 = GPIO_PIN_10;//pino PA10
uint8_t state_Bn1_bit1;
uint8_t state_Bn1_bit2;
uint8_t state_Bn1_bit3;

//Variáveis para o DMA e ADC
uint8_t adcDataReady;
uint32_t adcData[NUMBER_OF_CONVERSION];

//Variáveis para os botões
uint16_t btn_c = GPIO_PIN_6;//pino PB6
uint16_t btn_mais = GPIO_PIN_7;//pino PB7
uint16_t btn_menos = GPIO_PIN_8;//pino PB8
uint16_t btn_s = GPIO_PIN_9;//pino PB9
uint8_t state_btn_c;
uint8_t state_btn_mais;
uint8_t state_btn_menos;
uint8_t state_btn_s;

//Variávais para os pinos do display
uint16_t D4 = GPIO_PIN_2;//pino PA2
uint16_t D5 = GPIO_PIN_3;//pino PA3
uint16_t D6 = GPIO_PIN_6;//pino PA6
uint16_t D7 = GPIO_PIN_7;//pino PA7

uint16_t E = GPIO_PIN_1;//pino PA1
uint16_t RS = GPIO_PIN_0;//pino PA0

//Variáveis usadas para auxílio
int32_t i;
int32_t j;
uint8_t state_relogio;
int8_t selecao = -1;
uint8_t estado;
uint16_t aux1;
uint16_t aux2;
uint32_t dutyCycle_TIM1_CH4;
uint32_t dutyCycle_TIM2_CH1;
uint32_t dutyCycle_TIM2_CH2;
uint16_t horas;
uint16_t minutos;
uint16_t segundos;
char aux3[2];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */

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


  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */

  //Calibração do ADC
  HAL_ADCEx_Calibration_Start(&hadc1);

  //Inicializa timers
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_Base_Start_IT(&htim4);

   //captura a hora do sistema e converte para uint
   strcpy(aux3, __TIME__);
   horas = atoi(aux3);
   strcpy(aux3, __TIME__ +3);
   minutos = atoi(aux3);
   strcpy(aux3, __TIME__ +6);
   segundos = atoi(aux3);

  //Inicializa o display
  variaveis_display(E, RS, D4, D5, D6, D7);
  inicializar_display();
  limpar_display();
  escrever_texto_display(0xC0, "Inicializando...");
  HAL_Delay(2000);

  //Estrutura para guardra os dados das capsulas
  typedef struct
  {
    char nome[17];
    char tipo_agua[10];
    float tempo_agua;
    float temperatura_agua;
    int8_t com_gas;
    float tempo_gas;
  }info_capsula;

 //Insere as informações das capsulas na estrutura
 info_capsula capsulas[8];

 	strcpy(capsulas[0].nome, "                ");
	strcpy(capsulas[0].tipo_agua, '\0');
	capsulas[0].tempo_agua = 0;
	capsulas[0].temperatura_agua = 0;
	capsulas[0].com_gas = no;
	capsulas[0].tempo_gas = 0;

	strcpy(capsulas[1].nome, "      Agua      ");
	strcpy(capsulas[1].tipo_agua, '\0');
	capsulas[1].tempo_agua = 1.5;
	capsulas[1].temperatura_agua = 0;
	capsulas[1].com_gas = no;
	capsulas[1].tempo_gas = 0;

	strcpy(capsulas[2].nome, "  Agua com gas  ");
	strcpy(capsulas[2].tipo_agua, "gelada");
	capsulas[2].tempo_agua = 3;
	capsulas[2].temperatura_agua = 15;
	capsulas[2].com_gas = yes;
	capsulas[2].tempo_gas = 1.5;

	strcpy(capsulas[3].nome, "   Cha gelado   ");
	strcpy(capsulas[3].tipo_agua, "gelada");
	capsulas[3].tempo_agua = 2.7;
	capsulas[3].temperatura_agua = 20;
	capsulas[3].com_gas = no;
	capsulas[3].tempo_gas = 0;

	strcpy(capsulas[4].nome, "   Cha quente   ");
	strcpy(capsulas[4].tipo_agua, "quente");
	capsulas[4].tempo_agua = 2.7;
	capsulas[4].temperatura_agua = 60;
	capsulas[4].com_gas = no;
	capsulas[4].tempo_gas = 0;

	strcpy(capsulas[5].nome, "  Refrigerante  ");
	strcpy(capsulas[5].tipo_agua, "gelada");
	capsulas[5].tempo_agua = 4;
	capsulas[5].temperatura_agua = 17;
	capsulas[5].com_gas = yes;
	capsulas[5].tempo_gas = 2.56;

	strcpy(capsulas[6].nome, "Cap. nao cadast.");
	strcpy(capsulas[6].tipo_agua, '\0');
	capsulas[6].tempo_agua = 0;
	capsulas[6].temperatura_agua = 0;
	capsulas[6].com_gas = no;
	capsulas[6].tempo_gas = 0;

	strcpy(capsulas[7].nome, "Cap. nao cadast.");
	strcpy(capsulas[7].tipo_agua, '\0');
	capsulas[7].tempo_agua = 0;
	capsulas[7].temperatura_agua = 0;
	capsulas[7].com_gas = no;
	capsulas[7].tempo_gas = 0;




  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	limpar_display();


  while (1)
  {


	  //Leitura da capsula
	  state_Bn1_bit1 = HAL_GPIO_ReadPin(GPIOA, Bn1_bit1);
	  state_Bn1_bit2 = HAL_GPIO_ReadPin(GPIOA, Bn1_bit2);
	  state_Bn1_bit3 = HAL_GPIO_ReadPin(GPIOA, Bn1_bit3);
	  Bn1 = (state_Bn1_bit3 * 4) + (state_Bn1_bit2 * 2) + state_Bn1_bit1;

	  ////Leitura pressostato/////
	  state_P1 = HAL_GPIO_ReadPin(GPIOB, P1);
	  state_P2 = HAL_GPIO_ReadPin(GPIOB, P2);


	  if(state_P1){//Verifica se o filtro estiver limpo
		  escrever_texto_display(0x80, capsulas[Bn1].nome);
	  }else{

		  escrever_texto_display(0x80, "Filt. agua satu.");
		  estado = 0;
	  }
	  if(capsulas[Bn1].com_gas == yes && !state_P2 && state_P1){//Se o cinlintro de CO2 estiver vazio e for inserida uma capsula q usa gas
		  HAL_Delay(2000);
		  escrever_texto_display(0x80, "Cilin. CO2 vazio");
		  HAL_Delay(2000);
		  estado = 0;
	  }
	  if((Bn1 == 0 && !state_P2) ||(!state_P1 && !state_P2)){//Se o cilindro de CO2 estiver vazio e não tiver capsulas inseridas ou o filtro esta sujo e o cilindro vazio
		  if(!state_P1 && !state_P2)HAL_Delay(2000);
		  escrever_texto_display(0x80, "Cilin. CO2 vazio");
		  if(!state_P1 && !state_P2)HAL_Delay(2000);
	  }


	  switch(estado){

	  	  	  	  case 0://Verificação inicial da capsula e relogio

	  	  	  		  //Mostra a hora
	  	  	  		  escrever_texto_display(0xC0, "    ");
	  	  	  		  if(horas < 10){
	  	  	  			  escrever_texto_display(0xC4, "0");
	  	  	  			  escrever_numero_display(0xC5, horas);
	  	  	  			  escrever_texto_display(0xC6, ":");
	  	  	  		  }else{
		  	  	  		  escrever_numero_display(0xC4, horas);
		  	  	  		  escrever_texto_display(0xC6, ":");
	  	  	  		  }
	  	  	  		  if(minutos < 10){
	  	  	  			  escrever_texto_display(0xC7, "0");
	  	  	  			  escrever_numero_display(0xC8, minutos);
	  	  	  		  	  escrever_texto_display(0xC9, ":");
	  	  	  		  }else{
	  	  	  			  escrever_numero_display(0xC7, minutos);
	  	  	  		  	  escrever_texto_display(0xC9, ":");
	  	  	  		  }
	  	  	  		  if(segundos < 10){
	  	  	  			  escrever_texto_display(0xCA, "0");
	  	  	  			  escrever_numero_display(0xCB, segundos);
	  	  	  		  }else{
	  	  	  			  escrever_numero_display(0xCA, segundos);
	  	  	  		  }
	  	  	  		  escrever_texto_display(0xCC, "    ");


	  	  	  		  if(Bn1 != 0 && state_P1){//Se houver capsula e o filtro limpo avança para o próximo estado
	  	  	  			  estado = 1;
	  	  	  		  }else{
	  	  	  			  state_relogio = 0;
	  	  	  		  }
					  break;

	              case 1://Detecta qual o tipo da capsula e confima se esta cadastrada

	            	  if(strcmp(capsulas[Bn1].nome, "Cap. nao cadast.") == 0 ||  Bn1 == 0){//Se a capsula não estiver cadastrada ou se o usuario tirou a capsula capsula
						estado = 0;
						state_relogio = 0;
						break;
	            	  }

					  if(capsulas[Bn1].com_gas == yes && !state_P2){//Se o cinlintro de CO2 estiver vazio não deixa avançar no código
							estado = 0;
							break;
					  }

					  if(strcmp(capsulas[Bn1].nome, "      Agua      ") == 0 && selecao == -1){//Se for a capsula de Agua vai para estado 5
						  estado = 5;
						  selecao = 0;
						  break;
					  }


					  if(state_relogio){//Se o usuário colocou a capsula e cancelou - mostra o relógio
						  estado = 0;
					  }else{//Se não mostra o texto
						  escrever_texto_display(0xC0, " S para iniciar ");
					  }

					  if(state_btn_s){//Avança para p próximo estado de tudo certo
						  if(strcmp(capsulas[Bn1].nome, "      Agua      ") == 0 && selecao == 4){//Caso o usuario tenha inserido a capsula de Agua e não selecionato o tipo de água
							  estado = 5;
							  state_btn_s = 0;
							  state_relogio = 0;
							  break;
						  }else{
							  selecao = -1;
						  }
						  estado = 2;
						  state_relogio = 0;
						}

					  if(state_btn_c){//Se o usuário inseriu a capsula e cancelou o início - muda o status do relógio
						  state_relogio = 1;
					  	 }

	                  break;

	              case 2://Identifica qual o tipo de água da capsula e faz o aquecimento ou resfriamento se necessário

	            	  //Leitura das temperaturas ////
	            	  	  T1 = 0;
	            	  	  T2 = 0;
	            	  	  for(j=0; j < 6; j++){//Lê 6 vezes os 12 canias para ter 36 amostra de cada sensor de temperatura
	            	  		  HAL_ADC_Start_DMA(&hadc1, adcData, NUMBER_OF_CONVERSION);
	            	  		  while(!adcDataReady){}
	            	  		  if(adcDataReady){
	            	  			  adcDataReady = 0;
	            	  			  aux1 = 0;
	            	  			  aux1 = 0;
	            	  			  for(i=0; i < NUMBER_OF_CONVERSION; i++){
	            	  				  if(i<6){
	            	  					  aux1 += adcData[i];
	            	  				  }else{
	            	  					  aux2 += adcData[i];
	            	  				  }
	            	  			  }
	            	  			  aux1 /= (NUMBER_OF_CONVERSION/2);
	            	  			  aux2 /= (NUMBER_OF_CONVERSION/2);
	            	  			  aux1 *= scala_temperatura;
	            	  			  aux2 *= scala_temperatura;
	            	  			  T1 += aux1;
	            	  			  T2 += aux2;
	            	  		  }
	            	  	  }
	            	  	  T1 /= 6;
	            	  	  T2 /= 6;
	            	  	  if(T1 > 100) T1 = 100;
	            	  	  if(T1 < 5) T1 = 5;
	            	  	  if(T2 > 50) T2 = 50;
	            	  	  if(T2 < 5) T2 = 5;

	                  if(strcmp(capsulas[Bn1].tipo_agua, "quente") == 0){
	        			  escrever_texto_display(0xC0, " Aquecendo agua ");
	        			  if(T1 >= capsulas[Bn1].temperatura_agua){//Se a temperatura da água estiver menor ou igual a temperatura especificada na capsula
	        			  	  HAL_GPIO_WritePin(GPIOB, Y1, GPIO_PIN_SET);//Abre valvula de aguá gelada
	        				  estado = 3;//Avança para o próximo estado
	        				  dutyCycle_TIM2_CH1 = 0;//Desliga o compressor
	        				  TIM2->CCR1 = dutyCycle_TIM2_CH1;
	        			  }else{//Senão faz a ação de controle
	        				  dutyCycle_TIM2_CH1 = (100-((T1*100)/capsulas[Bn1].temperatura_agua))*(MAX_DUTY_CYCLE_VALUE_TIM2/100);//Equação para o controle proporcional da resitencia
	        				  TIM2->CCR1 = dutyCycle_TIM2_CH1;
	        			  }
	                  }

	                  if(strcmp(capsulas[Bn1].tipo_agua, "gelada") == 0){//Se a temperatura da água estiver maior ou igual a temperatura especificada na capsula
	        			  escrever_texto_display(0xC0, "  Gelando agua  ");
	        			  if(T2 <= capsulas[Bn1].temperatura_agua){
	        			  	  HAL_GPIO_WritePin(GPIOB, Y2, GPIO_PIN_SET);//Abre valvula de água quente
	        				  estado = 3;//Avança para o próximo estado
	        				  dutyCycle_TIM2_CH2 = 0;//Desliga a resitência
	        				  TIM2->CCR2 = dutyCycle_TIM2_CH2;
	        			  }else{//Senão faz a ação de controle
	        				  dutyCycle_TIM2_CH2 = ((T2*100)/50)*(MAX_DUTY_CYCLE_VALUE_TIM2/100);//Equação para o controle proporcional do compressor
	        				  TIM2->CCR2 = dutyCycle_TIM2_CH2;
	        			  }
	                  }

	                  if(strcmp(capsulas[Bn1].tipo_agua, "natural") == 0){//Se o tipo de água for natural só abre a valvula
	                	  escrever_texto_display(0xC0, "  Agua natural  ");
	                  	  HAL_GPIO_WritePin(GPIOB, Y3, GPIO_PIN_SET);//Abre valvula de água natural
	                	  estado = 3;//Avança para o próximo estado
	                  }

	                  break;

	              case 3://Ativa a bomba e o CO2

        			  escrever_texto_display(0xC0, "Preparando drink");

	                  if(capsulas[Bn1].com_gas == yes){

	                	  HAL_GPIO_WritePin(GPIOB, Y4, GPIO_PIN_SET);//Abre valvula de gás
	                	  for(i=0; i <= 3000; i++){//Liga gradualmente a bomba
							  dutyCycle_TIM1_CH4 = i;
							  TIM1->CCR4 = dutyCycle_TIM1_CH4;
							  HAL_Delay(0.06);
	                	  }

	                	  HAL_Delay(((capsulas[Bn1].tempo_gas)*1000)-200);
	                	  HAL_GPIO_WritePin(GPIOB, Y4, GPIO_PIN_RESET);//Fecha valvula de gas
	                	  HAL_Delay(((capsulas[Bn1].tempo_agua-capsulas[Bn1].tempo_gas)*1000)-250);
	                	  for(i=3000; i >= 0; i--){//Desliga gradualmete o bomba
							  dutyCycle_TIM1_CH4 = i;
							  TIM1->CCR4 = dutyCycle_TIM1_CH4;
							  HAL_Delay(0.08);
						  }


	                  }else{
	                	  for(i=0; i <= 3000; i++){//Liga gradualmente a bomba
							  dutyCycle_TIM1_CH4 = i;
							  TIM1->CCR4 = dutyCycle_TIM1_CH4;
							  HAL_Delay(0.06);
	                	  }
	                	  HAL_Delay(((capsulas[Bn1].tempo_agua)*1000)-550);
	                	  for(i=3000; i >= 0; i--){//Desliga gradualmete o bomba
							  dutyCycle_TIM1_CH4 = i;
							  TIM1->CCR4 = dutyCycle_TIM1_CH4;
							  HAL_Delay(0.08);
						  }
	                  }
	                  HAL_GPIO_WritePin(GPIOB, Y1, GPIO_PIN_RESET);//Fecha valvula de água quente
	                  HAL_GPIO_WritePin(GPIOB, Y2, GPIO_PIN_RESET);//Fecha valvula de água gelada
	                  HAL_GPIO_WritePin(GPIOB, Y3, GPIO_PIN_RESET);//Fecha valvula de água natural
	                  estado = 4;//Avança para o próximo estado

	                  break;

	              case 4://Aviso para remoçâo de capsula usada
	            	  escrever_texto_display(0xC0, "Remover capsula ");
	            	  if(Bn1 == 0){//Se capsula removida
	            		  estado = 0;//Retorna para o estado inicial
	            	  }
	            	  break;

	              case 5://Seleção do tipo de água para a capsula de Agua

	            	  if(Bn1 == 0){//Se o usuário retar a capsula
	            		  estado = 0;//Retorna para o estado inicial
	            		  selecao = -1;
	            		  break;
	            	  }
	            	  //Incremento se os botões forem pressionados
	            	  if(state_btn_mais) selecao++;
					  if(state_btn_menos) selecao--;

					  //Se C presionado vai para o estado 1
					  if(state_btn_c) {
						  estado = 1;
						  selecao = 4;
						  break;
					  }

					  //Organiza a seleção de forma circular
					  if(selecao == -1) selecao = 3;
					  if(selecao == 4) selecao = 0;

					  //As opções
	        		  if(selecao == 0){
						  escrever_texto_display(0xC0, "   Agua quente  ");
						  strcpy(capsulas[Bn1].tipo_agua, "quente");
						  capsulas[Bn1].temperatura_agua = 80;
						  capsulas[Bn1].tempo_agua = 3;
					  }
					  if(selecao == 1){
						  escrever_texto_display(0xC0, "   Agua gelada  ");
						  strcpy(capsulas[Bn1].tipo_agua, "gelada");
						  capsulas[Bn1].temperatura_agua = 10;
						  capsulas[Bn1].tempo_agua = 3;
					  }
					  if(selecao == 3){
						  escrever_texto_display(0xC0, "   Agua natural  ");
						strcpy(capsulas[Bn1].tipo_agua, "natural");
						  capsulas[Bn1].tempo_agua = 3;
					  }

					  //Se S pressionado vai para o estado 1
					  if(state_btn_s) {
						  estado = 1;
					  }

	            	  break;

	          }





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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 12;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_7;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_8;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_9;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_10;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_11;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Rank = ADC_REGULAR_RANK_12;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 3000-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 3000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7201;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 3334;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 7201;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 10001;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * Enable DMA controller clock
  * Configure DMA for memory to memory transfers
  *   hdma_memtomem_dma1_channel2
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* Configure DMA request hdma_memtomem_dma1_channel2 on DMA1_Channel2 */
  hdma_memtomem_dma1_channel2.Instance = DMA1_Channel2;
  hdma_memtomem_dma1_channel2.Init.Direction = DMA_MEMORY_TO_MEMORY;
  hdma_memtomem_dma1_channel2.Init.PeriphInc = DMA_PINC_ENABLE;
  hdma_memtomem_dma1_channel2.Init.MemInc = DMA_MINC_ENABLE;
  hdma_memtomem_dma1_channel2.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_memtomem_dma1_channel2.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_memtomem_dma1_channel2.Init.Mode = DMA_NORMAL;
  hdma_memtomem_dma1_channel2.Init.Priority = DMA_PRIORITY_LOW;
  if (HAL_DMA_Init(&hdma_memtomem_dma1_channel2) != HAL_OK)
  {
    Error_Handler( );
  }

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3
                           PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA9 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB5 PB6 PB7
                           PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
//Função para as inperrupções dos timers
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  //Verifica qual time gerou a interrupção
  if (htim == &htim3)
  {

	  /////Leitura botões//////
	  state_btn_c = HAL_GPIO_ReadPin(GPIOB, btn_c);
	  state_btn_mais = HAL_GPIO_ReadPin(GPIOB, btn_mais);
	  state_btn_menos = HAL_GPIO_ReadPin(GPIOB, btn_menos);
	  state_btn_s = HAL_GPIO_ReadPin(GPIOB, btn_s);


	  //////Led para mostar que o sistemas esta funcionando////
	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
  }
  if (htim == &htim4){
	  //Contado para o funcionamento do relógio
	  segundos++;
	  if(segundos == 60){
		  segundos = 0;
		  minutos ++;
	  }
	  if(minutos == 60){
		  horas++;
		  minutos = 0;
	  }
	  if(horas == 24){
		  horas = 0;
	  }
  }
}

//Função do DMA
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc){
	adcDataReady = 1;
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
