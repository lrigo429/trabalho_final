/*
 * display.h
 *
 *  Created on: Oct 28, 2020
 *      Author: Álisson Rigo
 */
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

void variaveis_display(uint16_t e, uint16_t rs, uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7);
void inicializar_display();
void tempo_display();
void limpar_display();
void comandos_display(char instrucao);
void escrever_texto_display(char posicao, char *texto);
void dec2bin(int16_t num, int8_t *bin);
void escrever_numero_display(char posicao, int32_t variavel);
void caracteres_especiais(char posicao, char *caracter);
void escreve_char(uint8_t caracter);

#endif /* INC_DISPLAY_H_ */
