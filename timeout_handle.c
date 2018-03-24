#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "timeout_handle.h"
#include "app_handle.h"

/*
 *超时判断
 */
enum {
	TIMEOUT_OFF = 0,
	TIMEOUT_ON
} timeoutFlag_t;

UINT8 packageTimeout = 3;
UINT8 timeoutTimes = 3;
static UINT8 timeoutFlag = TIMEOUT_OFF; 

typedef eMtErr(*p_pack)();

typedef struct
{
    BOOL     timeoutFlag;              //重发标志，判断是否重发
    UINT8    times;                    //重发次数
    time_t   time;                     //发送开始的时间，单位s
    UINT16      cmdId;                    //重发的包cmd类型
    p_pack   ptimeoutFunc;             //需要重发的打包函数

}sTimeoutHandle_t;


// 超时查询表
#define TIMEOUT_NUM          3
sTimeoutHandle_t  g_timeout_handle[TIMEOUT_NUM] = 
{   
	{FALSE,        0,        0,        CMD_AFN_2_F1_LOG_IN,          pack_afn02f01},
	{FALSE,        0,        0,        CMD_AFN_2_F2_LOG_OUT,         pack_afn02f2},
	{FALSE,        0,        0,        CMD_AFN_2_F3_HEART_BEAT,      pack_afn02f3}
	// {FALSE,        0,        0,        CMD_AFN_A_F1_TML_UP_CFG,      pack_afn0af1_s2m},
	// {FALSE,        0,        0,        CMD_AFN_A_F3_MST_IP_PORT,     pack_afn0af3_s2m},
	// {FALSE,        0,        0,        CMD_AFN_A_F13_LED_UP_CFG,     pack_afn0af13_s2m},
	// {FALSE,        0,        0,        CMD_AFN_A_F15_HAVE_DIFF_EVENT,pack_afn0af15_s2m},
	// {FALSE,        0,        0,        CMD_AFN_C_F1_ANALOG_DATA,     pack_afn0cf01_s2m_analog},
	// {FALSE,        0,        0,        CMD_AFN_C_F2_TML_CLOCK,       pack_afn0cf02_s2m_auto},
	// {FALSE,        0,        0,        CMD_AFN_11_F4,                pack_afn11hf4_s2m}
};


void app_timeout_reset(UINT16 cmdId)
{
	int i = 0;
	for(i = 0; i < TIMEOUT_NUM; i++)
	{
		/*将发送的包注册到重发检查表中*/
		if(g_timeout_handle[i].cmdId == cmdId)
		{
			printf("cmdId:%02X重发重置\n",cmdId);
			g_timeout_handle[i].timeoutFlag = FALSE;
			g_timeout_handle[i].times = 0;
			g_timeout_handle[i].time = 0;
		}
	}
}

void app_timeout_handle(void)
{
	int i = 0;
	eMtErr eRet = MT_OK;
	time_t now;
	p_pack   pApp;

	time(&now);
	printf("app_timeout_handle time:%08d\n",now);
	for(i = 0; i < TIMEOUT_NUM; i++)
	{
		pApp = g_timeout_handle[i].ptimeoutFunc;
		if(g_timeout_handle[i].timeoutFlag == TRUE)
		{
			if((now - g_timeout_handle[i].time) >= packageTimeout)
			{
				g_timeout_handle[i].time = now;
				if(g_timeout_handle[i].times < timeoutTimes)
				{
					printf("重发次数:%d\n", g_timeout_handle[i].times);
					g_timeout_handle[i].times++;	
					if(pApp!= NULL)
	            	{
	            		printf("超时：%02X\n", g_timeout_handle[i].cmdId);
	            	    eRet = pApp(TRUE);
	            	    if(eRet != MT_OK)
	            	    {
	            	        show_mt_error(eRet);
	            	    }
	            	}
	            }
	            else
	            {
	            	/*超时参数重置*/
	            	printf("超时参数重置\n");
	            	app_timeout_reset(g_timeout_handle[i].cmdId);
	            }
			}
		}
	}
}

/*
 *超时包注册
 */
void app_timeout_register(UINT16 cmdId)
{
	int i;
	time_t now;

	time(&now);
	for(i = 0; i < TIMEOUT_NUM; i++)
	{
		/*将发送的包注册到重发检查表中*/
		if(g_timeout_handle[i].cmdId == cmdId)
		{
			if(g_timeout_handle[i].timeoutFlag == FALSE){
				printf("cmdId:%02X注册重发,time:%08d\n",cmdId,now);
				g_timeout_handle[i].timeoutFlag = TRUE;
				g_timeout_handle[i].times = 0;
				g_timeout_handle[i].time = now;
			}
		}
	}
}

BOOL timeout_flag_get(UINT16 cmdId)
{
	int i = 0;

	for(i = 0; i < TIMEOUT_NUM; i++)
	{
		if(g_timeout_handle[i].cmdId == cmdId)
			return g_timeout_handle[i].timeoutFlag;
	}
}

#if 0
void timeout_flag_set(UINT8 flag)
{
	timeoutFlag = flag;
}

static UINT8 timeout_flag_get(UINT8 flag)
{
	return timeoutFlag;
}

BOOL app_ack_timeout(void)
{
	UINT8 flag;
	static UINT8 tick = 0;
	
	flag = timeout_flag_get();
	if(flag == TIMEOUT_ON)
	{
		tick++;
	}
	else 
	{
		tick = 0;
	}

	if(tick >= ackTimeout)
	{
		return FALSE;
	}

	return TRUE;
}
#endif












