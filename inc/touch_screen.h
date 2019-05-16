/*
 * touch_screen.h
 *
 *  Created on: 26.02.2018
 *      Author: gwarchol
 */

#ifndef TOUCH_SCREEN_H_
#define TOUCH_SCREEN_H_
#include "stm32f10x_spi.h"
#include "stm32f10x_it.h"
#include "stdbool.h"
#include "menu.h"


#define TOUCH_SCREEN_BUFFER_SIZE 6


#define Z_VALUE_INDEX 1
#define X_VALUE_INDEX 3

#define TOUCH_SCREEN_PUSH_SENS 2

#define Y_VALUE_INDEX 4


typedef enum {TS_NOP = 0, WAIT_TO_READ_Z_AXIS =1, ASK_TO_READ_Z_AXIS,
	WAIT_TO_READ_X_AXIS, ASK_TO_READ_X_AXIS,
	WAIT_TO_READ_Y_AXIS, ASK_TO_READ_Y_AXIS
} touch_screen_state_t;


typedef struct touch_data_s{
	volatile unsigned char z_axis;
	volatile unsigned char x_axis;
	volatile unsigned char y_axis;
} touch_data_t;

#define TOUCH_SCREEN_GET_Z_VALUE_COMMAND 0xB3
#define TOUCH_SCREEN_GET_X_VALUE_COMMAND 0x90
#define TOUCH_SCREEN_GET_Y_VALUE_COMMAND 0xD0
#define TOUCH_SCREEN_DUMMY_WORLD 0x00

#define SPI_2                    SPI2
#define SPI_2_CLK                RCC_APB1Periph_SPI2
#define SPI_2_GPIO               GPIOB
#define SPI_2_GPIO_CLK           RCC_APB2Periph_GPIOB
#define SPI_2_PIN_NSS            GPIO_Pin_12
#define SPI_2_PIN_SCK            GPIO_Pin_13
#define SPI_2_PIN_MISO           GPIO_Pin_14
#define SPI_2_PIN_MOSI           GPIO_Pin_15



void init_spi_2();
void init_spi_2_dma();
void init_touch_screen();
void control_touch_buttons();
void analize_data_from_touch_screen(bool on_off);

void touch_screen_send_command(unsigned short command);

void touch_screen_transmit_dma_data_set();
void touch_screen_receive_dma_data_set();

void touch_screen_cs_low(void);
void touch_screen_cs_high(void);

void run_command(uint8_t x_min_limit, uint8_t x_max_limit, uint8_t y_min_limit, uint8_t y_max_limit, void (*fun)(void));
void run_command_parameter(uint8_t x_min_limit, uint8_t x_max_limit, uint8_t y_min_limit, uint8_t y_max_limit, void (*fun)(menu_state_t), menu_state_t param);

void on_button_released();

int Odczytaj_Pomiar(void);
int Odczyt_X(void);
int Odczyt_Y(void);

touch_data_t * getTouchData();


volatile unsigned char get_touch_force();


#endif /* TOUCH_SCREEN_H_ */
