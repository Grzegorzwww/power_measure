/*
 * calculations.c
 *
 *  Created on: 24.05.2019
 *      Author: gwarchol
 */


#include "calculations.h"

static int power = 0;
static float work = 0;

int calculate_power(int mass, int cadence){

	power = mass;
	return power; //TODO

}


float calculate_work(){
	float f_power = power;
	work += f_power / 1000;
	return work;

}

float fGetWork()
{
	return work;
}
float iGetPower()
{
	return power;
}
