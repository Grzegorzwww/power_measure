/*
 * buffors.h
 *
 *  Created on: 13.12.2017
 *      Author: gwarchol
 */

#ifndef BUFFORS_H_
#define BUFFORS_H_

#include "stdbool.h"




#define BUFFOR_SIZE 512



typedef struct data_buffor_s {
	volatile unsigned char data[127];
	volatile unsigned char read_index_counter ;
	volatile unsigned char write_index_counter;
	volatile bool buffor_is_empty;
	volatile unsigned char quantity;
} data_buffor_t;





void initBuffor(volatile data_buffor_t *_data_buffor);
bool addToBuffor( volatile data_buffor_t *_data_buffor, unsigned char data_to_add);
bool getFromBuffor(volatile data_buffor_t *_data_buffor, unsigned char *data_to_return);
int getBufforSize( volatile data_buffor_t *_data_buffor);


bool isBufforEmpty(volatile data_buffor_t *_data_buffor);
void clearBuffor(volatile data_buffor_t *_data_buffor);



#endif /* BUFFORS_H_ */
