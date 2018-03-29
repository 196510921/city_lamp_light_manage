#ifndef _APP_HANDLE_H_
#define _APP_HANDLE_H_
#include "qgdw_3761_api.h"

enum {
	APP_HANDLE_FAILED = 0,
	APP_HANDLE_SUCCESS
}app_handle_state_t;


eMtErr pack_afn00f01(UINT8 afn);
eMtErr pack_afn02f01(void);
eMtErr pack_afn02f2(void);
eMtErr pack_afn0cf01_s2m_analog(void);
eMtErr pack_afn0cf02_s2m_auto(void);
eMtErr pack_afn02f3(void);

eMtErr pack_afn0cf02_s2m_auto(void);
eMtErr pack_afn0cf08_s2m();
eMtErr pack_afn0af1_s2m(void);
eMtErr pack_afn0af3_s2m(void);
eMtErr pack_afn0af13_s2m(void);
eMtErr pack_afn0af15_s2m(void);
eMtErr pack_afn11hf4_s2m(void);
void app_req_handle(UINT16 eCmd, void* d);
void app_show_app_sub_data(eMtDir eDir,eMtCmd emtCmd, uMtApp *puAppData);

/*子功能*/
eMtErr app_login(void);
eMtErr app_logout(void);
void set_current_usPn(UINT16 usPn);
UINT16 get_current_usPn(void);
int afn_11_f2_ctrl_table(sMt11f2_u* d);
#endif //_APP_HANDLE_H_






