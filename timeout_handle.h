#ifndef _TIMEOUT_HANDLE_H_
#define _TIMEOUT_HANDLE_H_
#include "qgdw_3761_api.h"

BOOL timeout_flag_get(UINT16 cmdId);
void app_timeout_reset(UINT16 cmdId);
void app_timeout_handle(void);
void app_timeout_register(UINT16 cmdId);

/*设置超时时间*/
void app_timtout_set_value(UINT16 d);
/*重传次数设置*/
void app_retry_times_set(UINT8 d);


#endif //_TIMEOUT_HANDLE_H_

