/*
 * rtc.h
 *
 *  Created on: 15.12.2017
 *      Author: gwarchol
 */

#ifndef RTC_H_
#define RTC_H_

#include <device_config.h>
#include "stm32f10x.h"
#include "stdbool.h"


#define BATTERY_REMOVED 98
#define BATTERY_RESTORED 99
#define SECONDS_IN_DAY 86399
#define CONFIGURATION_DONE 0xAAAA
#define CONFIGURATION_RESET 0x0000
#define OCTOBER_FLAG_SET 0x4000
#define MARCH_FLAG_SET 0x8000
#define DEFAULT_DAY 21
#define DEFAULT_MONTH 3
#define DEFAULT_YEAR 2018
#define DEFAULT_HOURS 12
#define DEFAULT_MINUTES 0
#define DEFAULT_SECONDS 0
#define LEAP 1
#define NOT_LEAP 0

#define ALARM_REPEATER 2



void RTC_Configuration(void);
void NVIC_RTC_Configuration(void);
int getTotalRtcTicks();

void rtc_init(void);
void analize_clock_clendar_state(void);
void read_alarm(unsigned char *data_buff);
void read_time(unsigned char *data_buff);
void save_time(unsigned char hours, unsigned char minutes, unsigned short seconds);
void read_date(unsigned char *data_buff);
void save_date(unsigned char day, unsigned char month, unsigned short year);
void update_date(void);

void check_next_day(void);

u8 check_leap(u16 u16_Year);


void set_alarm(u8 u8_Hour,u8 u8_Minute, u8 u8_Seconds);

void check_for_days_elapsed(void);
unsigned char day_of_week();
void update_last_date_on_start();


void addHour();
void removeHour() ;
void addMin();
void removeMin();
void addSec();
void removeSec();
void addAlarmHour();
void removeAlarmHour();
void addAlarmMinute();
void removeAlarmMinute();
void addAlarmSec();
void removeAlarmSec();

void prev_date();
void removeDay();
void removeMonth();
void removeYear();





struct Time_s
{
  u8 u8_SecLow;
  u8 u8_SecHigh;
  u8 u8_MinLow;
  u8 u8_MinHigh;
  u8 u8_HourLow;
  u8 u8_HourHigh;
};


struct AlarmTime_s
{
  u8 u8_SecLow;
  u8 u8_SecHigh;
  u8 u8_MinLow;
  u8 u8_MinHigh;
  u8 u8_HourLow;
  u8 u8_HourHigh;
};

struct Date_s
{
  u8 u8_Month;
  u8 u8_Day;
  u16 u16_Year;
};

struct AlarmDate_s
{
  u8 u8_Month;
  u8 u8_Day;
  u16 u16_Year;
};
#endif /* RTC_H_ */
