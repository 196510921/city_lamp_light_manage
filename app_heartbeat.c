#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "app_heartbeat.h"
#include "app_handle.h"

/*心跳周期*/
UINT8 heartbeatF = 10;

/*设置heartbeat周期*/
void set_heartbeat(UINT8 d)
{
	heartbeatF = d;	
}

void app_heartbeat(void)
{
	time_t now;
	time(&now);

	if(0 == (now % heartbeatF))
	{
		printf("app_heartbeat\n");
		pack_afn02f3();
	}
}




