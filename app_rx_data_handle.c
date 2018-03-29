#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "app_handle.h"
#include "app_sql.h"


extern sqlite3 *DCS003_db;

static int UINT8_to_ascii(int d, char* buf);
/*AFN:0x01*******************************************************************************/
/*硬件初始化*/
void afn_1_f1_hard_init_handle(void)
{
	/*确认回复*/
	pack_afn00f01(AFN_01_RSET);
	printf("硬件初始化\n");
}
/*数据区初始化*/
void afn_1_f2_data_init_handle(void)
{
	/*确认回复*/
	pack_afn00f01(AFN_01_RSET);
	printf("数据区初始化\n");
}

/*参数及全体数据区初始化（即恢复至出厂配置）*/
void afn_1_f3_factory_reset_handle(void)
{
	/*确认回复*/
	pack_afn00f01(AFN_01_RSET);
	printf("参数及全体数据区初始化（即恢复至出厂配置）\n");
}

/*参数（除与系统主站通信有关的）及全体数据区初始化*/
void afn_1_f4_para_init_handle(void)
{
	/*确认回复*/
	pack_afn00f01(AFN_01_RSET);
	printf("参数（除与系统主站通信有关的）及全体数据区初始化\n");
}
/*AFN:0x04*******************************************************************************/
/*终端电能表控制参数配置*/
#if 0
typedef struct
{
    UINT8   Type;       //测量点类型
    UINT8   Nature;     //测量点性质
    UINT8   Flag;       //启停用标志
    UINT8   Port;       //接入端口号
    UINT8   PortType;   //接入端口号属性
    UINT8   address[6]; //通信地址
    UINT8   Group[4];   //分组码
    UINT8   Num;        //所属回路编号
    Position PosLong;   //经度
    Position PosLat;    //纬度        
} sMtAfn04F13;
typedef struct{
    UINT8   Flag;       //标识
    UINT8   Degree;     //度
    UINT8   Min;        //分
    UINT16      Sec;        //秒
    
}Position;
#endif
int afn_04_f13_ctrl_table(sMtAfn04F13* d)
{
	printf("afn_04_f13_ctrl_table\n");
    #if 0
	if(NULL == d){
		printf("error: d is NULL!");
		ret = APP_HANDLE_FAILED;
		goto Finish;
	}

    int i, ret = APP_HANDLE_SUCCESS;
    char* sql = NULL;

	printf("测量点%X,测量点类型：%X,测量点性质:%X,启停标志:%X,接入端口号：%X,\r \
		接入端口号性质：%X \n",get_current_usPn(), d->Type,d->Nature,d->Flag,d->Port,d->PortType);

	printf("通信地址:\n");
	for(i = 0; i < 6; i++)
		printf("%X,",d->address[i]);
	printf("\n");

	printf("分组码：\n");
    for(i = 0; i < 4; i++)
        printf("%X,",d->Group[i]);
    printf("\n");

    printf("所属分组号:%X\n",d->Num);

    printf("经度标识:%X,度：%X,分:%X,秒:%X\n",d->PosLong.Flag,d->PosLong.Degree,\
        d->PosLong.Min,d->PosLong.Sec);
    printf("经度标识:%X,度：%X,分:%X,秒:%X\n",d->PosLat.Flag,d->PosLat.Degree,\
        d->PosLat.Min,d->PosLat.Sec);

    {/*sqlite open*/
        rc = app_sql_open();
        if(rc != APP_HANDLE_SUCCESS){
            ret = APP_HANDLE_FAILED;
            goto Finish;
        }
         /*插入dal_cbt_dp*/
        {
            sql = "insert or replace into dal_cbt_dp(id,dp,dpType,dpNature,dpSign,port,portType 
            ,box,channelNumber,state,Poslong,Poslite)values(NULL,?,?,?,?,?,?,?,?,?,?,?)";

            if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK){
                printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                ret = APP_HANDLE_FAILED;
                goto Finish; 
            }

            sqlite3_bind_int(stmt, 1, get_current_usPn());    
            sqlite3_bind_int(stmt, 2, d->Type);
            sqlite3_bind_int(stmt, 3, d->Nature);
            sqlite3_bind_int(stmt, 4, d->Flag);
            sqlite3_bind_int(stmt, 5, d->Port);
            sqlite3_bind_int(stmt, 6, d->PortType);

            rc = sqlite3_step(stmt);   
            printf("step() return %s, number:%03d\n", rc == SQLITE_DONE ? "SQLITE_DONE": rc == SQLITE_ROW ? "SQLITE_ROW" : "SQLITE_ERROR",rc);
            if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE)){
                printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
            }

            if(stmt != NULL)
                sqlite3_finalize(stmt);
        }
        /*插入dal_cbt_controller*/
        {
            sql = "insert or replace into dal_cbt_controller(id,box,type)values(NULL,?,?)";
            if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK){
                printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                ret = APP_HANDLE_FAILED;
                goto Finish; 
            }

            rc = sqlite3_step(stmt);   
            printf("step() return %s, number:%03d\n", rc == SQLITE_DONE ? "SQLITE_DONE": rc == SQLITE_ROW ? "SQLITE_ROW" : "SQLITE_ERROR",rc);
            if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE)){
                printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
            }

            if(stmt != NULL)
                sqlite3_finalize(stmt);
        }
        /*插入dal_cbt_group*/
        {
            sql = "insert or replace into dal_cbt_group(id,groupInfo)values(?,?)";
            if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK){
                printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                ret = APP_HANDLE_FAILED;
                goto Finish; 
            }

            rc = sqlite3_step(stmt);   
            printf("step() return %s, number:%03d\n", rc == SQLITE_DONE ? "SQLITE_DONE": rc == SQLITE_ROW ? "SQLITE_ROW" : "SQLITE_ERROR",rc);
            if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE)){
                printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
            }

            if(stmt != NULL)
                sqlite3_finalize(stmt);
        }

    }

	Finish:
    if(stmt != NULL)
        sqlite3_finalize(stmt);
     app_sql_close();

	return ret;
    #endif
} 

/*AFN:0x11*******************************************************************************/
/*日控时段*/
int afn_11_f2_ctrl_table(sMt11f2_u* d)
{
	printf("afn_11_f2_ctrl_table\n");

	int i,j,k,rc,ret = APP_HANDLE_SUCCESS;
	char startdate[20];
	char ucAddress[20];
	char* state = NULL;
	char* sql =	"insert or replace into dal_cbt_clock(id,groupid,startdate,state,percent)values(NULL,?,?,?,?);";	
	sqlite3_stmt* stmt = NULL;

	if(NULL == d){
		printf("error: d is NULL!");
		ret = APP_HANDLE_FAILED;
		goto Finish;
	}

	rc = app_sql_open();
	if(rc != APP_HANDLE_SUCCESS){
		ret = APP_HANDLE_FAILED;
		goto Finish;
	}

	if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK){
            printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
            ret = APP_HANDLE_FAILED;
            goto Finish; 
    }

	printf("任务格式:%02X \
			任务类型:%X \
			任务长度:%x \
			年:%X 月:%X 	日:%X \
			持续时间:%X \
			时段数据块总数: %X\n",d->ucTaskFormat, d->ucTaskType, \
								 d->ucTaskLen, d->sT.ucYY, d->sT.ucMM, \
								 d->sT.ucDD, d->LastTime, d->ucTimeNum);
	{
		/*添加年月日*/
		//startdate[0] = d->sT.ucYY;
		UINT8_to_ascii(d->sT.ucYY, &startdate[0]);
		//startdate[1] = d->sT.ucMM;
		UINT8_to_ascii(d->sT.ucMM, &startdate[2]);
		//startdate[2] = d->sT.ucDD;
		UINT8_to_ascii(d->sT.ucDD, &startdate[4]);
	}
	for(i = 0; i < d->ucTimeNum; i++)
	{
		printf("时段%X开始时间,分:%X 时:%X  \
				开关状态:%X \
				亮度百分比:%X \
				",i,d->Time[i].Min, d->Time[i].Hour, d->Time[i].Status, d->Time[i].Light);
		{
			/*时*/
			UINT8_to_ascii(d->Time[i].Hour, &startdate[6]);
			//startdate[3] = d->Time[i].Hour;
			/*分*/
			UINT8_to_ascii(d->Time[i].Min, &startdate[8]);
			//startdate[5] = d->Time[i].Min;
		}

		{
			/*开关状态*/
			if(d->Time[i].Status == 0x00)
				state = "OFF";
			else
				state = "ON";
		}

		printf("控制参数块数:%X\n", d->ucDataNum);
		for(j = 0; j < d->ucDataNum; j++)
		{
			printf("操作类型:%X ",d->Data[j].ucOperate);
			for(k = 0; k < 8; k++)
			{
				printf("控制器地址:%X, ",d->Data[j].ucAddress[k]);
				UINT8_to_ascii(d->Data[j].ucAddress[k], &ucAddress[k*2]);
			}
			/*地址*/
			// {
			// 	memcpy(ucAddress, d->Data[j].ucAddress[0], 8);
			// }
			printf("\n");
			for(k = 0; k < 32; k++)
			{
				printf("控制器密钥:%X ", d->Data[j].ucKey[k]);
			}
			printf("\n");
			{
	            printf("ucAddress:%s\n",ucAddress);
	            sqlite3_bind_text(stmt, 1, ucAddress, 16, NULL);
	            printf("startdate:%s\n",startdate);
	            sqlite3_bind_text(stmt, 2, startdate, 10, NULL);
	            sqlite3_bind_text(stmt, 3, state, -1, NULL);
	            sqlite3_bind_int(stmt, 4, d->Time[i].Light);
	            rc = sqlite3_step(stmt);   
			    printf("step() return %s, number:%03d\n", rc == SQLITE_DONE ? "SQLITE_DONE": rc == SQLITE_ROW ? "SQLITE_ROW" : "SQLITE_ERROR",rc);
			    if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE)){
			        printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
			    }
			    sqlite3_reset(stmt);
			    sqlite3_clear_bindings(stmt);
			}
		}

	}

	Finish:
	if(stmt != NULL)
		sqlite3_finalize(stmt);
	 app_sql_close();

	return ret;
}

int afn_11_f3_ctrl_table(sMtCtr_11hf3* d)
{
    int i,j;

    printf("任务格式:%02X,任务类型:%X,任务长度:%X\n",d->ucTaskFormat,d->ucTaskType,d->ucTaskLen);
    
    for(i = 0; i < d->ucTaskLen; i++)
    {
        printf("年:%X月:%X日:%X时:%X分:%X\n",d->ucData[i].Time.ucYY, \
                                            d->ucData[i].Time.ucMM, \
                                            d->ucData[i].Time.ucDD, \
                                            d->ucData[i].Time.ucHH, \
                                            d->ucData[i].Time.ucmm);
        printf("status:%X,Light%X\n",d->ucData[i].Status,d->ucData[i].Light);
        if(i > 12)
            break;
    }
    printf("配置数量:%X\n",d->ucNum);
    for(i = 0; i < d->ucNum; i++)
    {
        printf("操作类型:%X\n",d->ucPara[i].ucOperate);
        printf("通信地址:");
        for(j = 0; j < 8; j++)
            printf("%X,",d->ucPara[i].ucAddress[j]);
    }
}


void app_show_app_sub_data(eMtDir eDir,eMtCmd emtCmd, uMtApp *puAppData)
{
    int     i   = 0;
    int     j   = 0;
    eMtErr eRet = MT_OK;
    eMtErr eRet2 = MT_OK;

    sMtCmdInfor sCmdInfo;
    eMtCmd   eCmd = CMD_AFN_F_UNKOWN;
    char *pStr = NULL;
    
    switch(emtCmd)
    {
        case CMD_AFN_0_F1_ALL_OK:
            //sMtCmdErr* psAllOk = (sMtCmdErr*)puAppData;
            printf("AFN_0_F1_ALL_OK,eFAN:%02X\n",puAppData->sSure);//psAllOk->eAFN);
            //app_timeout_reset(puAppData->sSure);

        break;
        case CMD_AFN_0_F3_ONE_BY_ONE:
        {
            sMtOnebyOne * psOnebyOne = (sMtOnebyOne*)puAppData;
            printf("afn = %d\n", psOnebyOne->eAFN);
            printf("Num = %d\n", psOnebyOne->ucNum);
            int i = 0;
            for(i = 0; i < psOnebyOne->ucNum; i++)
            {
                printf("%d:\n", i+1);
                printf("确认命令  = %04X ", psOnebyOne->sOne[i].eCmd);
                /*接收到确认，清除重发*/
                printf("/*接收到确认，清除重发*/\n");
                app_timeout_reset(psOnebyOne->sOne[i].eCmd);
                eRet = eMtGetCmdInfor(psOnebyOne->sOne[i].eCmd, MT_DIR_S2M, &sCmdInfo);
                
                if(MT_OK == eRet)
                {
                   pStr =  sCmdInfo.pName; 
                }
                else
                {
                    eRet2 = eMtGetCmdInfor(psOnebyOne->sOne[i].eCmd, MT_DIR_M2S, &sCmdInfo);
                    if(eRet2 == MT_OK)
                    {
                        pStr =  sCmdInfo.pName; 
                    }
                    else
                    {
                        pStr = "未知命令";
                    }
                }

                show_item_value(pStr);
                //printf("%s\n", pStr);

                printf("\n测量点号  = %d\n", psOnebyOne->sOne[i].usPn);
                printf("确认结果  = %s\n", psOnebyOne->sOne[i].bOk == TRUE ? "确认" : "否认");
            }
            
            
        }
        break;

        case CMD_AFN_1_F1_HARD_INIT:
        	afn_1_f1_hard_init_handle();
        break;
        case CMD_AFN_1_F2_DATA_INIT:
        	afn_1_f2_data_init_handle();
        break;
        case CMD_AFN_1_F3_FACTORY_RESET:
        	afn_1_f3_factory_reset_handle();
        break;
        case CMD_AFN_1_F4_PARA_INIT:
        	afn_1_f4_para_init_handle();
        break;
        /*
        case CMD_AFN_1_F1_HARD_INIT:
        	afn_1_f1_hard_init_handle();
        break;
		*/
        case CMD_AFN_4_F1_TML_UP_CFG:
        {
            sMtTmlUpCfg * psTmlUpCfg = (sMtTmlUpCfg*)puAppData;

            printf("ucRTS = %d\n", psTmlUpCfg->ucRTS);
            printf("ucRTM = %d\n", psTmlUpCfg->ucRTM);
            printf("ucReSendTimes = %d\n", psTmlUpCfg->ucReSendTimes);
            printf("ucHeartBeat = %d\n", psTmlUpCfg->ucHeartBeat);
            printf("usTmWaitTimeoutS = %d\n", psTmlUpCfg->usTmWaitTimeoutS);
            printf("bAutoReportAsk_1 = %s\n", psTmlUpCfg->bAutoReportAsk_1 == TRUE ? "true":"false");
            printf("bAutoReportAsk_2 = %s\n", psTmlUpCfg->bAutoReportAsk_2 == TRUE ? "true":"false");
            printf("bAutoReportAsk_3 = %s\n", psTmlUpCfg->bAutoReportAsk_3 == TRUE ? "true":"false");
        }
        break;


        case CMD_AFN_4_F10_TML_POWER_CFG:
        {
            sMtTmlPowerCfg * pData = (sMtTmlPowerCfg*)puAppData;
            char addr[13] = {0};
            char pwd[7] = {0};
           
        }
        case CMD_AFN_4_F13_TML_SIMULA_CFG:
        {
        	show_error("终端电流表/路灯控制器参数配置\n");
            if(puAppData == NULL)
            	printf("puAppData error\n");
            if(afn_04_f13_ctrl_table((sMtAfn04F13*)puAppData) == APP_HANDLE_SUCCESS)
            	pack_afn00f01(CMD_AFN_4_F13_TML_SIMULA_CFG);
            else
            	printf("终端电流表/路灯控制器参数配置失败\n");
        }

        break;

         case CMD_AFN_5_F31_CHECK_TIME:
        {
            sMtUserClock * pData = (sMtUserClock*)puAppData;
            /*全部确认*/
            pack_afn00f01(AFN_05_CTRL);
            printf("check time :\n");
            printf("ucYear : %d\n", pData->ucYear);
            printf("ucMonth : %d\n", pData->ucMonth);
            //printf("ucWeek : %d\n", pData->ucWeek);
            printf("ucDay : %d\n", pData->ucDay);
            printf("ucHour : %d\n", pData->ucHour);
            printf("ucMinute : %d\n", pData->ucMinute);
            printf("ucSecond : %d\n", pData->ucSecond);

            
        }
        break;

        case CMD_AFN_9_F1_TML_VERSION:
        {   
            char buf[12] = {0};
            sMtTmlVersion * pData = (sMtTmlVersion*)puAppData;

            app_req_handle(CMD_AFN_9_F1_TML_VERSION, puAppData);
            
        }
        break;

        case CMD_AFN_9_F6_SUPPORT_ASK1_CFG:
        {    
            sMtCmdInfor    sCmdInfo = {0};
            sMtSuptAsk1 * pData = (sMtSuptAsk1*)puAppData;
            printf("support cfg num = %d\n", pData->ucNum);
            for(i = 0; i < pData->ucNum; i++)
            {
                printf("%d: cmd = 0x%04X\t", i+1, pData->eCfgCmd[i]);
                eRet = eMtGetCmdInfor(pData->eCfgCmd[i], MT_DIR_M2S, &sCmdInfo);  
                if(MT_OK == eRet)
                {
                    printf("%s\n", sCmdInfo.pName);
                }
                else
                {
                   printf("错误的命令字\n");
                }
            }
        }

        case CMD_AFN_A_F10_TML_POWER_CFG:
        {
            sMtTmlPowerCfg_Q* pData = (sMtTmlPowerCfg_Q*)puAppData;

            printf("usN = %d\n", pData->usN);
            for(i = 0; i < pData->usN; i++)
            {
               printf("usID[%d] = %d\n",i+1,  pData->usID[i]);
              
            }
        }
        break;
        
        case CMD_AFN_C_F1_ANALOG_DATA:
        {
            printf("CMD_AFN_C_F1_ANALOG_DATA\n");
            app_req_handle(CMD_AFN_C_F1_ANALOG_DATA, puAppData);
        }
        break;

        break;

        case CMD_AFN_A_F1_TML_UP_CFG:
            printf("CMD_AFN_A_F1_TML_UP_CFG\n");
            app_req_handle(CMD_AFN_A_F1_TML_UP_CFG, puAppData);
        break;
        case CMD_AFN_A_F3_MST_IP_PORT:
            printf("CMD_AFN_A_F3_MST_IP_PORT\n");
            app_req_handle(CMD_AFN_A_F3_MST_IP_PORT, puAppData);
        break;
        case CMD_AFN_A_F13_LED_UP_CFG:
            printf("CMD_AFN_A_F13_LED_UP_CFG\n");
            app_req_handle(CMD_AFN_A_F13_LED_UP_CFG, puAppData);
        break;
        case CMD_AFN_C_F2_TML_CLOCK:
        {
            printf("CMD_AFN_C_F2_TML_CLOCK\n");
            app_req_handle(CMD_AFN_C_F2_TML_CLOCK, puAppData);
            sMt11f2_u * pData = (sMt11f2_u*)puAppData;
            printf("任务格式");
            
            
        }
        break;
        
        case CMD_AFN_C_F8_TML_EVNT_FLAG:
        {
            printf("CMD_AFN_C_F8_TML_EVNT_FLAG\n");
            app_req_handle(CMD_AFN_C_F8_TML_EVNT_FLAG, puAppData);
                
        }
        break;
            
        case CMD_AFN_11_F2:
            show_error("日控时段表\n");
            if(puAppData == NULL)
            	printf("puAppData error\n");
            if(afn_11_f2_ctrl_table((sMt11f2_u*)puAppData) == APP_HANDLE_SUCCESS)
            	pack_afn00f01(AFN_11_LED);
            else
            	printf("日控时段表配置失败\n");
        break;
	    case CMD_AFN_11_F3:
            show_error("转发主站遥控命令\n");
            if(puAppData == NULL)
                printf("puAppData error\n");
            if(afn_11_f3_ctrl_table((sMtCtr_11hf3*)puAppData) == APP_HANDLE_SUCCESS)
                pack_afn00f01(AFN_11_LED);
            else
                printf("日控时段表配置失败\n");
        break;
	    
        case CMD_AFN_11_F4:
        
            show_error("查询对路灯批量控制情况\n");
            app_req_handle(CMD_AFN_11_F4, puAppData);
        
	    break;

        default:
            {
                show_error("没有数据域,或未实现显示\n");
            }

            break;

    }
}

static int UINT8_to_ascii(int d, char* a)
{
	int j = 0;
	printf("%X\n", d);

	a[j] = (d / 10) + '0';
	printf("a[j]:%c\n",a[j]);
	j++;
	a[j] = d%10+'0';
	printf("a[j]:%c\n",a[j]);
	return j+1;
}

static UINT16 appUsPN = 0;
void set_current_usPn(UINT16 usPn)
{
    appUsPN = usPn;
}

UINT16 get_current_usPn(void)
{
    return appUsPN;
}

