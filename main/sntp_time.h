#ifndef SNTP_TIME_H_
#define SNTP_TIME_H_

void sntp_time_task_start(void);

char* sntp_time_get_time(void);
void sntp_time_obtain_time(void);

#endif