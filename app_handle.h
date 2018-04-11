#ifndef _APP_HANDLE_H_
#define _APP_HANDLE_H_
#include "qgdw_3761_api.h"

enum {
	APP_HANDLE_FAILED = 0,
	APP_HANDLE_SUCCESS
}app_handle_state_t;

/*日控制时段相关*/
#define TASK_TYPE_YEAR    		0x13
#define TASK_TYPE_MON     		0x14
#define TASK_TYPE_DAY     		0x15
#define TASK_TYPE_WEEK    		0x16
#define TASK_TYPE_HOUR    		0x17

#define TASK_TYPE_COMMON_MASK   0x0F
#define TASK_TYPE_DELETE_MASK   0x30

/*组删除、测量点删除、组内测量点删除*/
#define SQL_GROUP_DEL           0x50
#define SQL_GROUP_PN_DEL        0x51
#define SQL_PN_DEL              0x00

/*04f3组信息配置相关*/
#define AFN_04_F3_GROUP_CONFIG  0x02


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

/*app_common*/
int app_groupInfo_unpack(UINT16* d, char* groupInfo);
#endif //_APP_HANDLE_H_






