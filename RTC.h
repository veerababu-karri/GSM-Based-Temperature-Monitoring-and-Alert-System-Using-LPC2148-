#ifndef RTC_H
#define RTC_H

void rtc_init(void);


void rtc_display(void);


int validate(unsigned char f, unsigned int v);


void rtc_write(unsigned char f, unsigned int v);


void rtc_edit(char key);


void rtc_edit_display(void);
void RTC_GetDateTime(char *buf);


extern volatile unsigned char edit_mode ;


extern unsigned char field ;


extern char digit_buf[5];


extern unsigned char digit_cnt;

#endif

