/*
 * rtc.c
 *
 *  Created on: 15.12.2017
 *      Author: gwarchol
 */

#include "rtc.h"
//#include "DFPPlayer.h"
#include "menu.h"
#include "flash/flash.h"


volatile unsigned long int total_rtc_ticks = 0;

volatile struct Time_s s_TimeStructVar;
struct AlarmTime_s s_AlarmStructVar;
struct Date_s s_DateStructVar;
struct AlarmDate_s s_AlarmDateStructVar;

static volatile bool clock_state_changed = false;
static volatile bool alarm_state = false;

static uint8_t prev_day = 1;



bool getAlarmState(){
	return alarm_state;
}



void rtc_init(void){




	NVIC_RTC_Configuration();

	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		RTC_Configuration();
	}else{
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
			printf("\r\n\n Power On Reset occurred....");
		}
		/* Check if the Pin Reset flag is set */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
			printf("\r\n\n External Reset occurred....");
		}

		printf("\r\n No need to configure RTC....");
		/* Wait for RTC registers synchronization */
		RTC_WaitForSynchro();

		/* Enable the RTC Second */
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();

	}

	RCC_ClearFlag();
}






void RTC_Configuration(void)
{

	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	/* Reset Backup Domain */
	BKP_DeInit();

	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{}


	s_DateStructVar.u8_Month=DEFAULT_MONTH ;
	s_DateStructVar.u8_Day=DEFAULT_DAY;
	s_DateStructVar.u16_Year=DEFAULT_YEAR;
	//  u16_SummerTimeCorrect = OCTOBER_FLAG_SET;
	//  BKP_WriteBackupRegister(BKP_DR7,u16_SummerTimeCorrect);
	BKP_WriteBackupRegister(BKP_DR2,s_DateStructVar.u8_Month);
	BKP_WriteBackupRegister(BKP_DR3,s_DateStructVar.u8_Day);
	BKP_WriteBackupRegister(BKP_DR4,s_DateStructVar.u16_Year);
	BKP_WriteBackupRegister(BKP_DR1, CONFIGURATION_DONE);

	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);

	RTC_ITConfig(RTC_IT_ALR, ENABLE);

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();


	check_for_days_elapsed();
	//  u8_ClockSource = BKP_ReadBackupRegister(BKP_DR6);
	//  u8_TamperNumber = BKP_ReadBackupRegister(BKP_DR5);
	s_DateStructVar.u8_Month = BKP_ReadBackupRegister(BKP_DR2);
	s_DateStructVar.u8_Day = BKP_ReadBackupRegister(BKP_DR3);
	s_DateStructVar.u16_Year = BKP_ReadBackupRegister(BKP_DR4);
	//  u16_SummerTimeCorrect = BKP_ReadBackupRegister(BKP_DR7);
	s_AlarmDateStructVar.u8_Month = BKP_ReadBackupRegister(BKP_DR8);
	s_AlarmDateStructVar.u8_Day = BKP_ReadBackupRegister(BKP_DR9);
	s_AlarmDateStructVar.u16_Year = BKP_ReadBackupRegister(BKP_DR10);


}

void NVIC_RTC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;			//RTC 1 SEK IRQ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	//  NVIC_InitStructure.NVIC_IRQChannel =  RTCAlarm_IRQChannel;			//ALARM IRQ
	NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;			//ALARM IRQ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	//
	//  //konfiguracja przerwań pochodzących od RTC Alarm
	//      EXTI_ClearITPendingBit(EXTI_Line17);
	//      EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	//      EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//      EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	//      EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	//      EXTI_Init(&amp;EXTI_InitStructure);
	//      //generowanie przerwania przez RTC Alarm
	//      RTC_ITConfig(RTC_IT_ALR, ENABLE);
	//      //odczekanie na zakończenie operacji
	//      RTC_WaitForLastTask();



	//  NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
	//   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//   NVIC_Init(&NVIC_InitStructure);





}





void analize_clock_clendar_state(void)
{

	if(clock_state_changed){

		check_next_day();

		clock_state_changed = false;
	}




}

volatile bool x = false;

void RTC_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		/* Clear the RTC Second interrupt */
		RTC_ClearITPendingBit(RTC_IT_SEC);
		/* Toggle LED1 */
		total_rtc_ticks = RTC_GetCounter();
		clock_state_changed = true;
		//update_date();
		switch(x){
		case true:
			//turnOnOffLed(true);
			x = false;
			break;
		case false:
			//turnOnOffLed(false);
			x = true;
			break;
		}
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	}


	if (RTC_GetITStatus(RTC_IT_ALR) != RESET)
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);	//wyczyszczenie flagi przerwania


		if(alarm_on_checked()){
			int rand_alarm = (total_rtc_ticks % 6) + 1 ;// 1-5
			printf("alarm tutaj %d\n", rand_alarm);
			//printf("ALARM Alarm przerwanie \n");
			RTC_WaitForLastTask();
			unsigned char current_day = day_of_week();

			if(working_day_checked()){
				if(current_day != 5 && current_day != 6){
					//play_alarm(rand_alarm, 4);
					turnOnOffLed(true);
				}
			}
			else {
				//play_alarm(rand_alarm, 4);
				turnOnOffLed(true);
			}

		}
		//odczekanie na zakończenie operacji na RTC
		RTC_WaitForLastTask();
	}
}


void RTCAlarm_IRQHandler(void){


	//
		 printf("ALARM !");



		  if (RTC_GetITStatus(RTC_IT_ALR) != RESET)
		  {

				 printf("ALARM 2!");
			  RTC_WaitForLastTask();
			          //wyczyszczenie flagi przerwania
			   RTC_ClearITPendingBit(RTC_IT_ALR);
			          //odczekanie na zakończenie operacji na RTC
			   RTC_WaitForLastTask();

		  }


}





int getTotalRtcTicks(){
	return total_rtc_ticks;
}
void read_alarm(unsigned char *data_buff){
	uint32_t alarmvar;
	alarmvar = RTC_GetAlarm();
	alarmvar = alarmvar % 86400;
	s_AlarmStructVar.u8_HourHigh = (uint8_t)(alarmvar / 3600 ) / 10;
	s_AlarmStructVar.u8_HourLow = (uint8_t)(alarmvar / 3600 ) % 10;
	s_AlarmStructVar.u8_MinHigh = (uint8_t)((alarmvar % 3600 ) / 60) / 10;
	s_AlarmStructVar.u8_MinLow = (uint8_t)((alarmvar % 3600 ) / 60) % 10;
	s_AlarmStructVar.u8_SecHigh = ((uint8_t)(alarmvar % 60 ) % 60) / 10;
	s_AlarmStructVar.u8_SecLow = ((uint8_t)(alarmvar % 60 ) % 60) % 10;
	sprintf(data_buff, "%d%d:%d%d:%d%d",
			s_AlarmStructVar.u8_HourHigh,
			s_AlarmStructVar.u8_HourLow,
			s_AlarmStructVar.u8_MinHigh,
			s_AlarmStructVar.u8_MinLow,
			s_AlarmStructVar.u8_SecHigh,
			s_AlarmStructVar.u8_SecLow);



}


void read_time(unsigned char *data_buff){
	uint32_t timevar;
	timevar = RTC_GetCounter();
	timevar = timevar % 86400;
	s_TimeStructVar.u8_HourHigh = (uint8_t)(timevar / 3600 ) / 10;
	s_TimeStructVar.u8_HourLow = (uint8_t)(timevar / 3600 ) % 10;
	s_TimeStructVar.u8_MinHigh = (uint8_t)((timevar % 3600 ) / 60) / 10;
	s_TimeStructVar.u8_MinLow = (uint8_t)((timevar % 3600 ) / 60) % 10;
	s_TimeStructVar.u8_SecHigh = ((uint8_t)(timevar % 60 ) % 60) / 10;
	s_TimeStructVar.u8_SecLow = ((uint8_t)(timevar % 60 ) % 60) % 10;
	sprintf(data_buff, "%d%d:%d%d:%d%d",
			s_TimeStructVar.u8_HourHigh,
			s_TimeStructVar.u8_HourLow,
			s_TimeStructVar.u8_MinHigh,
			s_TimeStructVar.u8_MinLow,
			s_TimeStructVar.u8_SecHigh,
			s_TimeStructVar.u8_SecLow);
}


void read_date(unsigned char *data_buff){
	s_DateStructVar.u8_Month = BKP_ReadBackupRegister(BKP_DR2);
	s_DateStructVar.u8_Day = BKP_ReadBackupRegister(BKP_DR3);
	s_DateStructVar.u16_Year = BKP_ReadBackupRegister(BKP_DR4);


	unsigned char temp_day[2];
	unsigned char temp_month[2];

	if(s_DateStructVar.u8_Day < 10){
		sprintf(temp_day, "0%d", s_DateStructVar.u8_Day);
	}else{
		sprintf(temp_day, "%d", s_DateStructVar.u8_Day);
	}

	if(s_DateStructVar.u8_Month < 10){
		sprintf(temp_month, "0%d", s_DateStructVar.u8_Month);
	}else{
		sprintf(temp_month, "%d", s_DateStructVar.u8_Month);
	}


	sprintf(data_buff, "%s:%s:%d", temp_day, temp_month, s_DateStructVar.u16_Year);

}

void addAlarmHour(){
	unsigned long int total_alarm_ticks_to_add = RTC_GetAlarm();
	total_alarm_ticks_to_add +=  3600;
	RTC_SetAlarm(total_alarm_ticks_to_add);
}
void removeAlarmHour(){
	unsigned long int total_alarm_ticks_to_add = RTC_GetAlarm();
	total_alarm_ticks_to_add -=  3600;
	RTC_SetAlarm(total_alarm_ticks_to_add);
}
void addAlarmMinute(){
	unsigned long int total_alarm_ticks_to_add = RTC_GetAlarm();
	total_alarm_ticks_to_add +=  60;
	RTC_SetAlarm(total_alarm_ticks_to_add);
}
void removeAlarmMinute(){
	unsigned long int total_alarm_ticks_to_add = RTC_GetAlarm();
	total_alarm_ticks_to_add -=  60;
	RTC_SetAlarm(total_alarm_ticks_to_add);
}
void addAlarmSec(){
	unsigned long int total_alarm_ticks_to_add = RTC_GetAlarm();
	total_alarm_ticks_to_add +=  1;
	RTC_SetAlarm(total_alarm_ticks_to_add);
}
void removeAlarmSec(){
	unsigned long int total_alarm_ticks_to_add = RTC_GetAlarm();
	total_alarm_ticks_to_add -=  1;
	RTC_SetAlarm(total_alarm_ticks_to_add);
}








void addHour() {

	RTC_WaitForLastTask();
	unsigned long int total_ticks_to_add = RTC_GetCounter();
	total_ticks_to_add +=  3600;
	RTC_SetCounter(total_ticks_to_add);
	RTC_WaitForLastTask();
}
void removeHour() {

	RTC_WaitForLastTask();
	unsigned long int total_ticks_to_add = RTC_GetCounter();
	total_ticks_to_add -=  3600;
	RTC_SetCounter(total_ticks_to_add);
	RTC_WaitForLastTask();
}
void addMin() {

	RTC_WaitForLastTask();
	unsigned long int total_ticks_to_add = RTC_GetCounter();
	total_ticks_to_add +=  60;
	RTC_SetCounter(total_ticks_to_add);
	RTC_WaitForLastTask();
}
void removeMin() {

	RTC_WaitForLastTask();
	unsigned long int total_ticks_to_add = RTC_GetCounter();
	total_ticks_to_add -=  60;
	RTC_SetCounter(total_ticks_to_add);
	RTC_WaitForLastTask();
}
void addSec() {

	RTC_WaitForLastTask();
	unsigned long int total_ticks_to_add = RTC_GetCounter();
	total_ticks_to_add +=  1;
	RTC_SetCounter(total_ticks_to_add);
	RTC_WaitForLastTask();
}
void removeSec() {
	RTC_WaitForLastTask();
	unsigned long int total_ticks_to_add = RTC_GetCounter();
	total_ticks_to_add -=  1;
	RTC_SetCounter(total_ticks_to_add);
	RTC_WaitForLastTask();

}






void save_time(unsigned char hours, unsigned char minutes, unsigned short seconds){




	unsigned long int total_ticks_to_add = (hours * 3600) + (minutes * 60) +seconds;

	RTC_WaitForLastTask();
	RTC_SetCounter(total_ticks_to_add);
	RTC_WaitForLastTask();


}


void check_next_day(void){
	uint32_t timevar = RTC_GetCounter();
	//if(total_rtc_ticks % 86399 == 0){
	if((timevar  % 86399) == 0){
		update_date();
	}
}


void addDay(){
	update_date();

	if(prev_day == 6 && day_of_week() == 7){

		uint8_t month = BKP_ReadBackupRegister(BKP_DR2);
		uint16_t year = BKP_ReadBackupRegister(BKP_DR4);
		uint8_t day = BKP_ReadBackupRegister(BKP_DR3);

		flash_set_date_storage(day, month, year);
	}
	prev_day = day_of_week();




}

bool read_date_was_modyfied(){
	FLASH_ReadSettings();
	if(settings.write_param == SETTEINGS_MODYFIED){
		return true;
	}else if(settings.write_param == SETTEINGS_NOT_MODYFIED) {
		return false;
	}else{
		return false;
	}
	return false;
}
void  write_date_was_modyfied(){
	settings.write_param = SETTEINGS_MODYFIED;

}


void update_last_date_on_start(){
	if(read_date_was_modyfied()){

	}



}
void addMonth(){
	int month = BKP_ReadBackupRegister(BKP_DR2);
	int i = month;
	if(month < 12){
		while(i  != (month+1)){
			update_date();
			i = BKP_ReadBackupRegister(BKP_DR2);
		}
	}else if(month == 12){
		i = BKP_ReadBackupRegister(BKP_DR2);
		while(i != 1){
			update_date();
			i = BKP_ReadBackupRegister(BKP_DR2);
		}
	}

}
void addYear(){
	int year = BKP_ReadBackupRegister(BKP_DR4);
	int i = year;
	while(i  != (year+1)){
		update_date();
		i = BKP_ReadBackupRegister(BKP_DR4);
	}
}


void removeDay(){
	prev_date();
}

void removeMonth(){
	int month = BKP_ReadBackupRegister(BKP_DR2);
	int i = month;
	if(month > 1){
		while(i  != (month - 1)){
			prev_date();
			i = BKP_ReadBackupRegister(BKP_DR2);
		}
	}else if(month == 1){
		i = BKP_ReadBackupRegister(BKP_DR2);
		while(i != 12){
			prev_date();
			i = BKP_ReadBackupRegister(BKP_DR2);
		}
	}
}
void removeYear(){
	int year = BKP_ReadBackupRegister(BKP_DR4);
	int i = year;
	while(i  != (year - (1))){
		prev_date();
		i = BKP_ReadBackupRegister(BKP_DR4);
	}
}


void prev_date(){

	s_DateStructVar.u8_Month=BKP_ReadBackupRegister(BKP_DR2);
	s_DateStructVar.u16_Year=BKP_ReadBackupRegister(BKP_DR4);
	s_DateStructVar.u8_Day=BKP_ReadBackupRegister(BKP_DR3);

	if(s_DateStructVar.u8_Month == 1 || s_DateStructVar.u8_Month == 3 || \
			s_DateStructVar.u8_Month == 5 || s_DateStructVar.u8_Month == 7 ||\
			s_DateStructVar.u8_Month == 8 || s_DateStructVar.u8_Month == 10 \
			|| s_DateStructVar.u8_Month == 12)
	{
		if(s_DateStructVar.u8_Day > 1)
		{
			s_DateStructVar.u8_Day--;
		}
		/* Date structure member: s_DateStructVar.u8_Day = 31 */
		else
		{
			if(s_DateStructVar.u8_Month != 1)
			{
				s_DateStructVar.u8_Month--;
				s_DateStructVar.u8_Day = 31;
			}
			/* Date structure member: s_DateStructVar.u8_Day = 31 & s_DateStructVar.u8_Month =12 */
			else
			{
				s_DateStructVar.u8_Month = 12;
				s_DateStructVar.u8_Day = 31;
				s_DateStructVar.u16_Year--;
			}
		}
	}
	else if(s_DateStructVar.u8_Month == 4 || s_DateStructVar.u8_Month == 6 \
			|| s_DateStructVar.u8_Month == 9 ||s_DateStructVar.u8_Month == 11)
	{
		if(s_DateStructVar.u8_Day >1)
		{
			s_DateStructVar.u8_Day--;
		}
		/* Date structure member: s_DateStructVar.u8_Day = 30 */
		else
		{
			s_DateStructVar.u8_Month--;
			s_DateStructVar.u8_Day = 30;
		}
	}
	else if(s_DateStructVar.u8_Month == 2)
	{
		if(s_DateStructVar.u8_Day > 1)
		{
			s_DateStructVar.u8_Day--;
		}
		else if(s_DateStructVar.u8_Day == 28)
		{
			/* Leap Year Correction */
			if(check_leap(s_DateStructVar.u16_Year))
			{
				s_DateStructVar.u8_Day--;
			}
			else
			{
				s_DateStructVar.u8_Month--;
				s_DateStructVar.u8_Day = 28;
			}
		}
		else if(s_DateStructVar.u8_Day == 1)
		{
			s_DateStructVar.u8_Month--;
			s_DateStructVar.u8_Day = 29;
		}
	}

	BKP_WriteBackupRegister(BKP_DR2,s_DateStructVar.u8_Month);
	BKP_WriteBackupRegister(BKP_DR3,s_DateStructVar.u8_Day);
	BKP_WriteBackupRegister(BKP_DR4,s_DateStructVar.u16_Year);



}

void update_date(void){

	s_DateStructVar.u8_Month=BKP_ReadBackupRegister(BKP_DR2);
	s_DateStructVar.u16_Year=BKP_ReadBackupRegister(BKP_DR4);
	s_DateStructVar.u8_Day=BKP_ReadBackupRegister(BKP_DR3);

	if(s_DateStructVar.u8_Month == 1 || s_DateStructVar.u8_Month == 3 || \
			s_DateStructVar.u8_Month == 5 || s_DateStructVar.u8_Month == 7 ||\
			s_DateStructVar.u8_Month == 8 || s_DateStructVar.u8_Month == 10 \
			|| s_DateStructVar.u8_Month == 12)
	{
		if(s_DateStructVar.u8_Day < 31)
		{
			s_DateStructVar.u8_Day++;
		}
		/* Date structure member: s_DateStructVar.u8_Day = 31 */
		else
		{
			if(s_DateStructVar.u8_Month != 12)
			{
				s_DateStructVar.u8_Month++;
				s_DateStructVar.u8_Day = 1;
			}
			/* Date structure member: s_DateStructVar.u8_Day = 31 & s_DateStructVar.u8_Month =12 */
			else
			{
				s_DateStructVar.u8_Month = 1;
				s_DateStructVar.u8_Day = 1;
				s_DateStructVar.u16_Year++;
			}
		}
	}
	else if(s_DateStructVar.u8_Month == 4 || s_DateStructVar.u8_Month == 6 \
			|| s_DateStructVar.u8_Month == 9 ||s_DateStructVar.u8_Month == 11)
	{
		if(s_DateStructVar.u8_Day < 30)
		{
			s_DateStructVar.u8_Day++;
		}
		/* Date structure member: s_DateStructVar.u8_Day = 30 */
		else
		{
			s_DateStructVar.u8_Month++;
			s_DateStructVar.u8_Day = 1;
		}
	}
	else if(s_DateStructVar.u8_Month == 2)
	{
		if(s_DateStructVar.u8_Day < 28)
		{
			s_DateStructVar.u8_Day++;
		}
		else if(s_DateStructVar.u8_Day == 28)
		{
			/* Leap Year Correction */
			if(check_leap(s_DateStructVar.u16_Year))
			{
				s_DateStructVar.u8_Day++;
			}
			else
			{
				s_DateStructVar.u8_Month++;
				s_DateStructVar.u8_Day = 1;
			}
		}
		else if(s_DateStructVar.u8_Day == 29)
		{
			s_DateStructVar.u8_Month++;
			s_DateStructVar.u8_Day = 1;
		}
	}

	BKP_WriteBackupRegister(BKP_DR2,s_DateStructVar.u8_Month);
	BKP_WriteBackupRegister(BKP_DR3,s_DateStructVar.u8_Day);
	BKP_WriteBackupRegister(BKP_DR4,s_DateStructVar.u16_Year);

}

u8 check_leap(u16 u16_Year)
{
	if((u16_Year%400)==0)
	{
		return LEAP;
	}
	else if((u16_Year%100)==0)
	{
		return NOT_LEAP;
	}
	else if((u16_Year%4)==0)
	{
		return LEAP;
	}
	else
	{
		return NOT_LEAP;
	}
}




void save_date(unsigned char day, unsigned char month, unsigned short year){


	//	  RightLeftIntExtOnOffConfig(DISABLE);
	//	  UpDownIntOnOffConfig(DISABLE);

	if((( month ==4 || month ==6 || month ==9 || month==11) && day ==31) \
			|| (month==2 && day ==31)|| (month==2 && day==30)|| \
			(month==2 && day ==29 && (check_leap(year)==0)))
	{

	}else{

		s_DateStructVar.u8_Day = day;
		s_DateStructVar.u8_Month =  month;
		s_DateStructVar.u16_Year = year;
		BKP_WriteBackupRegister(BKP_DR3,s_DateStructVar.u8_Day);
		BKP_WriteBackupRegister(BKP_DR2,s_DateStructVar.u8_Month);
		BKP_WriteBackupRegister(BKP_DR4,s_DateStructVar.u16_Year);

		s_AlarmDateStructVar.u8_Day = day;
		s_AlarmDateStructVar.u8_Month = month;
		s_AlarmDateStructVar.u16_Year = year;
		BKP_WriteBackupRegister(BKP_DR8,month);
		BKP_WriteBackupRegister(BKP_DR9,day);
		BKP_WriteBackupRegister(BKP_DR10,year);




	}


}


void set_alarm(u8 u8_Hour,u8 u8_Minute, u8 u8_Seconds)
{
	u32 u32_CounterValue;

	u32_CounterValue=((u8_Hour * 3600)+ (u8_Minute * 60)+u8_Seconds);

	if(u32_CounterValue == 0)
	{
		u32_CounterValue = SECONDS_IN_DAY;
	}

	RTC_WaitForLastTask();
	RTC_SetAlarm(u32_CounterValue);
	RTC_WaitForLastTask();
}


void check_for_days_elapsed(void)
{
	u8 u8_DaysElapsed;

	if((RTC_GetCounter() / SECONDS_IN_DAY) != 0)
	{
		for(u8_DaysElapsed = 0; u8_DaysElapsed < (RTC_GetCounter() / SECONDS_IN_DAY)\
		;u8_DaysElapsed++)
		{
			update_date();
		}

		RTC_SetCounter(RTC_GetCounter() % SECONDS_IN_DAY);
	}
}



unsigned char day_of_week()
{

	u16 u16_CurrentYear =  BKP_ReadBackupRegister(BKP_DR4);
	u8 u8_CurrentMonth = BKP_ReadBackupRegister(BKP_DR2);
	u8 u8_CurrentDay =  BKP_ReadBackupRegister(BKP_DR3);

	unsigned char  u16_Temp1,u16_Temp2,u16_Temp3,u16_Temp4,u16_CurrentWeekDay;

	if(u8_CurrentMonth < 3)
	{
		u8_CurrentMonth=u8_CurrentMonth + 12;
		u16_CurrentYear=u16_CurrentYear-1;
	}

	u16_Temp1=(6*(u8_CurrentMonth + 1))/10;
	u16_Temp2=u16_CurrentYear/4;
	u16_Temp3=u16_CurrentYear/100;
	u16_Temp4=u16_CurrentYear/400;
	u16_CurrentWeekDay=u8_CurrentDay + (2 * u8_CurrentMonth) + u16_Temp1 \
			+ u16_CurrentYear + u16_Temp2 - u16_Temp3 + u16_Temp4 +1;
	u16_CurrentWeekDay = u16_CurrentWeekDay % 7;

	return(u16_CurrentWeekDay);
}






