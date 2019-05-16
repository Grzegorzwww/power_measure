#ifndef _SCAN_TIMER_
#define _SCAN_TIMER_

#include "stdbool.h"

void init_timer();

typedef enum {TIMER_1s, TIMER_100ms, TIMER_10ms, TIMER_1ms} timer_channel_t;



bool getTimerChannelState(timer_channel_t channel);
void setTimerChannelState(timer_channel_t channel, bool state);


#endif
