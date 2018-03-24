#ifndef _TIMEOUT_HANDLE_H_
#define _TIMEOUT_HANDLE_H_
#include "qgdw_3761_api.h"

BOOL timeout_flag_get(UINT16 cmdId);
void app_timeout_reset(UINT16 cmdId);
void app_timeout_handle(void);
void app_timeout_register(UINT16 cmdId);



#endif //_TIMEOUT_HANDLE_H_

