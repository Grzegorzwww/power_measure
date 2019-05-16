/*
 * buffors.c
 *
 *  Created on: 13.12.2017
 *      Author: gwarchol
 */

#include "buffors.h"

bool addToBuffor(volatile data_buffor_t *_data_buffor, unsigned char data_to_add)
{
	if(_data_buffor->quantity < BUFFOR_SIZE)
	{
		_data_buffor->data[_data_buffor->quantity++] = data_to_add;
		return true;
	}
	else
	{
		return false;
	}
	return false;
}


bool getFromBuffor(volatile data_buffor_t *_data_buffor, unsigned char *data_to_return)
{
	int i = 0;
	if(_data_buffor->quantity != 0)
	{
		*data_to_return = _data_buffor->data[0];
		_data_buffor->quantity--;
		while(i < _data_buffor->quantity){
			_data_buffor->data[i] = _data_buffor->data[i+1];
			i++;
		}
		return true;
	}
	else {
		return false;
	}
}

int getBufforSize(volatile data_buffor_t *_data_buffor){
	return _data_buffor->quantity;
}


bool isBufforEmpty(volatile data_buffor_t *_data_buffor){
	if(_data_buffor->quantity == 0)
		return true;
	else return false;
}

void clearBuffor(volatile data_buffor_t *_data_buffor){
	_data_buffor->quantity = 0;
	_data_buffor->buffor_is_empty = true;
}



void initBuffor(volatile data_buffor_t *_data_buffor)
{
	_data_buffor->quantity = 0;
	_data_buffor->buffor_is_empty = true;
}
