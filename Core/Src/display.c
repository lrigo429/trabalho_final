/*
 * display.c
 *
 *  Created on: Oct 28, 2020
 *      Author: Álisson Rigo
 */

#include "display.h"

uint16_t D4;
uint16_t D5;
uint16_t D6;
uint16_t D7;

uint16_t E;
uint16_t RS;

/* --- Posicão da CRGRAM para salvar os caracteres especiais --- */

//endereço do carater na CGRAM á, DDRAM endreço 0
char posicao_c1 = 0x40;
//endereço do carater na CGRAM é, DDRAM endreço 1
char posicao_c2 = 0x48;
//endereço do carater na CGRAM ç, DDRAM endreço 2
char posicao_c3 = 0x50;
//endereço do carater na CGRAM ã, DDRAM endreço 3
char posicao_c4 = 0x58;
//endereço do carater na CGRAM õ, DDRAM endreço 4
char posicao_c5 = 0x60;

/* --- Caracteres especiais em binário para salvar na CGRAM --- */

//caracte á
char c1[8]={0b00000010,
			0b00000100,
			0b00001110,
			0b00000001,
			0b00001111,
			0b00010001,
			0b00001111,
			0b00000000};

//caracte é
char c2[8]={0b00000010,
			0b00000100,
			0b00001110,
			0b00010001,
			0b00011111,
			0b00010000,
			0b00001110,
			0b00000000};

//caracte ç
char c3[8]={0b00001110,
			0b00010001,
			0b00010000,
			0b00010000,
			0b00010101,
			0b00001110,
			0b00010000,
			0b00000000};

//caracte ã
char c4[8]={0b00001101,
			0b00010010,
			0b00001110,
			0b00000001,
			0b00001111,
			0b00010001,
			0b00001111,
			0b00000000};

//caracte õ
char c5[8]={0b00001101,
			0b00010010,
			0b00000000,
			0b00001110,
			0b00010001,
			0b00010001,
			0b00001110,
			0b00000000};


/**
 * @brief Pinos usados para conectar o diplay LCD ao microcontroladornas portas A usando 4 bits.
 *
 * Essa função passa para as variaveis E, RS, D4, D5, D6, D7 os pinos usados para conectar o diplay ao microcontrolador.
 * OBS. Usar as portas A do microcontrolador.
 *
 * @param[in] e : unsigned int de 16 bits, contendo a porta conectada oa pino E do display.
 * @param[in] rs : unsigned int de 16 bits, contendo a porta conectada oa pino RS do display.
 * @param[in] d4 : unsigned int de 16 bits, contendo a porta conectada oa pino D4 do display.
 * @param[in] d5 : unsigned int de 16 bits, contendo a porta conectada oa pino D5 do display.
 * @param[in] d6 : unsigned int de 16 bits, contendo a porta conectada oa pino D6 do display.
 * @param[in] d7 : unsigned int de 16 bits, contendo a porta conectada oa pino D7 do display.
 */

void variaveis_display(uint16_t e, uint16_t rs, uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7){
	E = e;
	RS = rs;

	D4 = d4;
	D5 = d5;
	D6 = d6;
	D7 = d7;
}

/**
 * @brief Inicializa o diplay LCD.
 *
 * Essa função inicializa o diplay de 16X2 com 4 bits para comunicação, sem cursor e escreve deslocando o cursor para direita.
 * E carrega os carcteres especiais para CRGRAM.
 */

void inicializar_display(){

	comandos_display(0x33); 
	comandos_display(0x32);
	comandos_display(0x28); //display 16X2 de 4 bits
	comandos_display(0x0C); //display sem cursos

	//caracteres_especiais(posicao_c1, c1);
	//caracteres_especiais(posicao_c2, c2);
	//caracteres_especiais(posicao_c3, c3);
	//caracteres_especiais(posicao_c4, c4);
	//caracteres_especiais(posicao_c5, c5);
  }

/**
 * @brief Tempo entre as mensagens enviadas para o diplay LCD.
 *
 * Essa função é usada para que entre cada mensagem ocorra um intervalo de tempo, como o fabricante recomenda.
 *
 */

void tempo_display(){
	HAL_GPIO_WritePin(GPIOA, E, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOA, E, GPIO_PIN_RESET);
}

/**
 * @brief Limpa o diplay LCD.
 *
 * Essa função é usada para limpara todos os caracteres impressos no display.
 *
 */

void limpar_display(){
	comandos_display(0x01);
}

/**
 * @brief Comados insternos do display.
 *
 * Essa função recebe uma instrução em hexadecimal e a envia para o display
 *
 * @param[in] instrucao : char de 8 bits, contendo a instrução para o display.
 */

void comandos_display(char instrucao){

	int8_t vet_bin[8];

	int16_t i = instrucao;//strtol(instrucao, NULL, 16);
	
	//envia para a função dec2bin um número de decimal e recebe um vetor com o número em binário
	dec2bin(i, &vet_bin);

	HAL_GPIO_WritePin(GPIOA, E, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, RS, GPIO_PIN_RESET);

	//manda para o display os 4 bits mais significativos do carácter
	HAL_GPIO_WritePin(GPIOA, D7, vet_bin[0]);
	HAL_GPIO_WritePin(GPIOA, D6, vet_bin[1]);
	HAL_GPIO_WritePin(GPIOA, D5, vet_bin[2]);
	HAL_GPIO_WritePin(GPIOA, D4, vet_bin[3]);
	tempo_display();

	//manda para o display os 4 bits menos significativos do carácter
	HAL_GPIO_WritePin(GPIOA, D7, vet_bin[4]);
	HAL_GPIO_WritePin(GPIOA, D6, vet_bin[5]);
	HAL_GPIO_WritePin(GPIOA, D5, vet_bin[6]);
	HAL_GPIO_WritePin(GPIOA, D4, vet_bin[7]);
	tempo_display();

	HAL_GPIO_WritePin(GPIOA, RS, GPIO_PIN_SET);

}


/**
 * @brief Escrve uma vetor de char no diplay.
 *
 * Essa função escreve um vetor de char no display.
 *
 * @param[in] posicao : char de 8 bits, contendo a possição de início da escrita no display.
 * @param[in] *texto : ponteiro para char, contendo o vetor de char para ser escrito no display.
 */

void escrever_texto_display(char posicao, char *texto){

	uint16_t i = 0;
	
	comandos_display(posicao);

	//escreve a vetor de char no display
	while(i < 17 & texto[i] != '\0'){

		//if para identificar carcteres especiais
		if(texto[i] == 195){
			//caracter é
			if(texto[i+1] == 169){
				texto[i+1] = 0;
			}

			//caracter é
			if(texto[i+1] == 161){
				texto[i+1] = 1;
			}

			//caracter ç
			if(texto[i+1] == 167){
				texto[i+1] = 2;
			}

			//caracter ã
			if(texto[i+1] == 163){
				texto[i+1] = 3;
			}

			//caracter õ
			if(texto[i+1] == 181){
				texto[i+1] = 4;
			}

			i++;
		}

		//chama a função para escrever os caracteres no display
		escreve_char(texto[i]);
		i++;
		}
	}


/**
 * @brief Escrve uma variavel (int de 32 bits) no diplay.
 *
 * Essa função escreve uma varival no display.
 *
 * @param[in] posicao : char de 8 bits, contendo a possição de início da escrita no display.
 * @param[in] variavel : inteiro de 32 bits, contendo o valor para ser escrito no display.
 */

void escrever_numero_display(char posicao, int32_t variavel){

	uint16_t i = 0;
	char texto[17];
	int8_t letra[8];

	//converte o valor de inteiro para um vetor de char
	itoa(variavel, texto, 10);

	comandos_display(posicao);

	//escreve no diplay o número
	while(i < 17 & texto[i] != '\0'){

		//chama a função para escrever os caracteres no display
		escreve_char(texto[i]);
		i++;
		}
	}

/**
 * @brief Converte um número décimal para binário.
 *
 * Essa função converte um valor em décimal para um vetor com esse número em binário.
 *
 * @param[in] num : inteiro de 16 bits, contendo o valor para ser convertido.
 * @param[out] *bin : ponteiro para um vetor inteiro de 8 bits com 8 posições, onde será gradado o valor em binário do número.
 */


void dec2bin(int16_t num, int8_t *bin){

	for (int16_t aux = 7; aux >= 0; aux--)
	   {
	      if (num % 2 == 0)
	         bin[aux] = 0;
	      else
	         bin[aux] = 1;
	      num = num / 2;
	   }
}

/**
 * @brief Escrve uma caracter especial na CGRAM.
 *
 * Essa função escreve os caracteres especias na CGRAM do display.
 *
 * @param[in] posicao : char de 8 bits, contendo a possição de início de gravação da CGRAM.
 * @param[in] caracter : ponteiro para char, contendo o desenho do caracter, um vetor de char de 8 posições.
 */

void caracteres_especiais(char posicao, char *caracter){

	comandos_display(posicao);


	for(int8_t i = 0; i < 8 ; i++){

		//chama a função para escrever as posições do desenho na CGRAM
		escreve_char(caracter[i]);
	}
}

/**
 * @brief Escrve um caracter no diplay.
 *
 * Essa função um caracter do display.
 *
 * @param[in] caracter : unsigned int de 8 bits, contendo o endereço do caracter na DDRAM do display.
 */

void escreve_char(uint8_t caracter){

	uint8_t vet_bin[8];

	//envia para a função dec2bin um número em decimal e recebe um vetor com o número em binário
	dec2bin(caracter, &vet_bin);

	HAL_GPIO_WritePin(GPIOA, RS, GPIO_PIN_SET);

	//manda para o display os 4 bits mais significativos do carácter
	HAL_GPIO_WritePin(GPIOA, D7, vet_bin[0]);
	HAL_GPIO_WritePin(GPIOA, D6, vet_bin[1]);
	HAL_GPIO_WritePin(GPIOA, D5, vet_bin[2]);
	HAL_GPIO_WritePin(GPIOA, D4, vet_bin[3]);
	tempo_display();

	//manda para o display os 4 bits menos significativos do carácter
	HAL_GPIO_WritePin(GPIOA, D7, vet_bin[4]);
	HAL_GPIO_WritePin(GPIOA, D6, vet_bin[5]);
	HAL_GPIO_WritePin(GPIOA, D5, vet_bin[6]);
	HAL_GPIO_WritePin(GPIOA, D4, vet_bin[7]);
	tempo_display();

	HAL_GPIO_WritePin(GPIOA, RS, GPIO_PIN_RESET);
}
