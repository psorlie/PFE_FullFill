#ifndef SRC_PROXY_H_
#define SRC_PROXY_H_

void Proxy_send_update(int id, int filling, char* name);

void Proxy_send_detail(int id, int filling, int battery, char* date, char* name);

void Proxy_send_network(char* sleep_time, char* wake_up_time, char* cycle_time, int cleaning_interval_day);

#endif /* SRC_PROXY_H_ */