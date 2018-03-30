#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "app_handle.h"
#include "app_sql.h"


extern sqlite3 *DCS003_db;

static int UINT8_to_ascii(int d, char* buf);
static void app_groupInfo_pack(UINT16 d, char* groupInfo);
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
/*远程请求空中升级*/
void afn_1_f5_update_handle(void)
{
    /*确认回复*/
    pack_afn00f01(AFN_01_RSET);
    printf("远程请求空中升级\n");
}

/*AFN:0x04*******************************************************************************/
/*终端电能表控制参数配置*/
int afn_04_f13_ctrl_table(sMtAfn04F13* d)
{
	printf("afn_04_f13_ctrl_table\n");
    #if 1
    int i, rc,ret = APP_HANDLE_SUCCESS;
    char* sql = NULL;
    char sql_1[300] = {0};
    char groupInfoNew[512] = {0};
    char box[6] = {0};
    char Group[5] = {0};
    char* groupInfoOld = NULL;
    UINT16 usPn = 0;
    sqlite3_stmt* stmt = NULL;
    
    if(NULL == d){
        printf("error: d is NULL!");
        ret = APP_HANDLE_FAILED;
        goto Finish;
    }

    {
        /*获取当前usPn*/
        usPn = get_current_usPn();
    }

	printf("测量点%X,测量点类型：%X,测量点性质:%X,启停标志:%X,接入端口号：%X,\r \
		接入端口号性质：%X \n",usPn, d->Type,d->Nature,d->Flag,d->Port,d->PortType);

	printf("通信地址:\n");
	for(i = 0; i < 6; i++){
		printf("%X,",d->address[i]);
        box[i] = d->address[i] + '0';
    }
	printf("\n");

	printf("分组码：\n");
    for(i = 0; i < 4; i++){
        printf("%X,",d->Group[i]);
        Group[i] = d->Group[i] + '0';
    }
    printf("\n");

    printf("所属分组号:%X\n",d->Num);

    printf("经度标识:%X,度：%X,分:%X,秒:%X\n",d->PosLong.Flag,d->PosLong.Degree,\
        d->PosLong.Min,d->PosLong.Sec);
    printf("经度标识:%X,度：%X,分:%X,秒:%X\n",d->PosLat.Flag,d->PosLat.Degree,\
        d->PosLat.Min,d->PosLat.Sec);

    {   /*sqlite open*/
        rc = app_sql_open();
        if(rc != APP_HANDLE_SUCCESS){
            ret = APP_HANDLE_FAILED;
            goto Finish;
        }
         /*插入dal_cbt_dp*/
        {
            sql = "insert or replace into dal_cbt_dp(id,dp,dpType,dpNature,dpSign,port,portType \
            ,box,channelNumber,state,Poslong,Poslit)values(NULL,?,?,?,?,?,?,?,?,?,?,?)";

            if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK){
                printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                ret = APP_HANDLE_FAILED;
                goto Finish; 
            }
            /*dp*/
            sqlite3_bind_int(stmt, 1, usPn);    
            /*dpType*/
            sqlite3_bind_int(stmt, 2, d->Type);
            /*dpNature*/
            sqlite3_bind_int(stmt, 3, d->Nature);
            /*dpSign*/
            sqlite3_bind_int(stmt, 4, d->Flag);
            /*port*/
            sqlite3_bind_int(stmt, 5, d->Port);
            /*PortType*/
            sqlite3_bind_int(stmt, 6, d->PortType);
            /*box*/
            //sqlite3_bind_text(stmt, 7, (char*)d->address, 6, NULL);
            sqlite3_bind_text(stmt, 7, box, 6, NULL);
            /*channelNumber*/
            sqlite3_bind_int(stmt, 8, d->Num);
            /*state*/
            sqlite3_bind_int(stmt, 9, 0);
            /*Poslong*/
            sqlite3_bind_text(stmt, 10, (char*)&d->PosLong, 5, NULL);
            /*Poslit*/
            sqlite3_bind_text(stmt, 11, (char*)&d->PosLat, 5, NULL);

            rc = sqlite3_step(stmt);   
            printf("step() return %s, number:%03d\n", rc == SQLITE_DONE ? \
                "SQLITE_DONE": rc == SQLITE_ROW ? "SQLITE_ROW" : "SQLITE_ERROR",rc);
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

            /*box*/
            sqlite3_bind_text(stmt, 1, box, 6, NULL);
            /*type*/
            sqlite3_bind_int(stmt, 2, 0);

            rc = sqlite3_step(stmt);   
            printf("step() return %s, number:%03d\n", rc == SQLITE_DONE ? \
                "SQLITE_DONE": rc == SQLITE_ROW ? "SQLITE_ROW" : "SQLITE_ERROR",rc);
            if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE)){
                printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
            }

            if(stmt != NULL)
                sqlite3_finalize(stmt);
        }
        /*插入dal_cbt_group*/
        {
            /*获取已有的groupInfo*/
            {
                sprintf(sql_1,"select groupInfo from dal_cbt_group where id = \"%s\";",  Group);
                printf("sql_1:%s",sql_1);
                if(sqlite3_prepare_v2(DCS003_db, sql_1, strlen(sql_1), &stmt, NULL) != SQLITE_OK){
                    printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                    ret = APP_HANDLE_FAILED;
                    goto Finish; 
                }

                rc = sqlite3_step(stmt);   
                if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE)){
                    printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
                }

                groupInfoOld = sqlite3_column_text(stmt, 0);
                if(NULL == groupInfoOld){
                    // printf("groupInfoOld NULL\n");
                    // ret = APP_HANDLE_FAILED;
                    // goto Finish;
                }else{
                    printf("groupInfoOld:%x,%x\n",groupInfoOld[0],groupInfoOld[1]);
                    memcpy((UINT8*)groupInfoNew, (UINT8*)groupInfoOld, groupInfoOld[0]*2);
                }
                if(stmt != NULL)
                    sqlite3_finalize(stmt);

            }
            /*拼接新的groupInfo到表group*/
            {
                #if 1
                sql = "insert or replace into dal_cbt_group(id,groupInfo)values(?,?)";
                if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK){
                    printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                    ret = APP_HANDLE_FAILED;
                    goto Finish; 
                }
                /*id*/
                sqlite3_bind_text(stmt, 1, Group, 4, NULL);
                /*groupInfo*/
                printf("new usPn:%03d\n",usPn);
                app_groupInfo_pack(usPn, groupInfoNew);
                printf("groupInfoNew num:%d\n", groupInfoNew[0]-'0');
                for(i = 0; i < groupInfoNew[0]-'0'; i++)
                {
                    printf("%c,", groupInfoNew[i*2+2]);
                }
                printf("\n");
                sqlite3_bind_text(stmt, 2, (char*)groupInfoNew, (groupInfoNew[0]-'0')*2 + 2, NULL);
                //sqlite3_bind_text(stmt, 2, (char*)groupInfoNew, 4, NULL);
                //sqlite3_bind_text(stmt, 2, "1234", 4, NULL);

                rc = sqlite3_step(stmt);   
                printf("step() return %s, number:%03d\n", rc == SQLITE_DONE ? \
                    "SQLITE_DONE": rc == SQLITE_ROW ? "SQLITE_ROW" : "SQLITE_ERROR",rc);
                if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE)){
                    printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
                }

                if(stmt != NULL)
                    sqlite3_finalize(stmt);
                #endif
            }
        }

    }

	Finish:
    // if(stmt != NULL)
    //     sqlite3_finalize(stmt);
     app_sql_close();

	return ret;
    #endif
} 

int afn_05_f31_clock(sMtUserClock* d)
{
    /*全部确认*/
    printf("check time :\n");
    printf("ucYear : %d\n", d->ucYear);
    printf("ucMonth : %d\n", d->ucMonth);
    printf("ucDay : %d\n", d->ucDay);
    printf("ucHour : %d\n", d->ucHour);
    printf("ucMinute : %d\n", d->ucMinute);
    printf("ucSecond : %d\n", d->ucSecond);    

}


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
		startdate[0] = d->sT.ucYY;
		startdate[1] = d->sT.ucMM;
		startdate[2] = d->sT.ucDD;
		//UINT8_to_ascii(d->sT.ucYY, &startdate[0]);
        //UINT8_to_ascii(d->sT.ucMM, &startdate[2]);
        //UINT8_to_ascii(d->sT.ucDD, &startdate[4]);
	}
	for(i = 0; i < d->ucTimeNum; i++)
	{
		printf("时段%X开始时间,分:%X 时:%X  \
				开关状态:%X \
				亮度百分比:%X \
				",i,d->Time[i].Min, d->Time[i].Hour, d->Time[i].Status, d->Time[i].Light);
		{
			/*时*/
			startdate[3] = d->Time[i].Hour;
			/*分*/
			startdate[5] = d->Time[i].Min;
		    //UINT8_to_ascii(d->Time[i].Hour, &startdate[6]);
            //UINT8_to_ascii(d->Time[i].Min, &startdate[8]);
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
				//UINT8_to_ascii(d->Data[j].ucAddress[k], &ucAddress[k*2]);
			}
			/*地址*/
			{
				memcpy(ucAddress, d->Data[j].ucAddress[0], 8);
			}
			printf("\n");
			for(k = 0; k < 32; k++)
			{
				printf("控制器密钥:%X ", d->Data[j].ucKey[k]);
			}
			printf("\n");
			{
	            sqlite3_bind_text(stmt, 1, ucAddress, 16, NULL);
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

/*AFN:0x11*******************************************************************************/
static int afn_11_f3_group_ctrl(UINT8* groupid, UINT8 state)
{
    printf("afn_11_f3_group_ctrl\n");
 
    int i,rc,len,ret = APP_HANDLE_SUCCESS;
    char sql[300] = {0};
    char* groupInfo = NULL;
    char groupid_1[5] = {0};
    UINT16 pn[255] = {0};
    UINT8* box = NULL;
    UINT8 channelNumber;
    sqlite3_stmt* stmt = NULL;
    /*debug信息*/
    {
        printf("分组地址: \n");
        for(i = 0; i < 4; i++)
        {
            printf("%x,",groupid[i]);
            groupid_1[i] = groupid[i] + '0';
        }
        printf("\n");
    }

    rc = app_sql_open();
    if(rc != APP_HANDLE_SUCCESS)
    {
        ret = APP_HANDLE_FAILED;
        goto Finish;
    }

    /*从group表中获取组内的pn*/
    {
        sprintf(sql,"select groupInfo from dal_cbt_group where id = \"%s\";", groupid_1);
        printf("sql:%s\n",sql);
        if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK){
            printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
            ret = APP_HANDLE_FAILED;
            goto Finish; 
        }

        rc = sqlite3_step(stmt);   
        printf("step() return %s, number:%03d\n", rc == SQLITE_DONE ? \
            "SQLITE_DONE": rc == SQLITE_ROW ? "SQLITE_ROW" : "SQLITE_ERROR",rc);
        if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE)){
            printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
        }

        groupInfo = sqlite3_column_text(stmt, 0);
        if(NULL == groupInfo){
            printf("groupInfo NULL\n");
            ret = APP_HANDLE_FAILED;
            goto Finish;
        }
        /*将group表中的信息转换成pn*/
        len = app_groupInfo_unpack(pn, groupInfo);

        if(stmt != NULL)
            sqlite3_finalize(stmt);
        /*根据pn查询box号,通道号*/
        {
            for(i = 0; i < len; i++)
            {
                sprintf(sql,"select box,channelNumber from dal_cbt_dp where dp = %03d;", pn[i]);
                //sprintf(sql,"select box,channelNumber from dal_cbt_dp where dp = %x;", 0x288);
                printf("sql:%s\n",sql);
                if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK){
                    printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                    ret = APP_HANDLE_FAILED;
                    goto Finish; 
                }

                rc = sqlite3_step(stmt);   
                printf("step() return %s, number:%03d\n", rc == SQLITE_DONE ? \
                    "SQLITE_DONE": rc == SQLITE_ROW ? "SQLITE_ROW" : "SQLITE_ERROR",rc);
                if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE)){
                    printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
                }

                box = sqlite3_column_text(stmt, 0);
                if(NULL == box){
                    printf("box NULL\n");
                    ret = APP_HANDLE_FAILED;
                    //goto Finish;
                }else
                /*控制通道*/
                {
                    printf("box:%x,%x,%x,%x,%x,%x\n",box[0] - '0',box[1] - '0',box[2] - '0',box[3]-'0',box[4]-'0',box[5]-'0');
                    channelNumber = sqlite3_column_int(stmt, 1);
                    printf("channelNumber:%d\n",channelNumber);
                    printf("state:%x",state);
                }

                if(stmt != NULL)
                    sqlite3_finalize(stmt);
            }
        }

    }

    Finish:
    // if(stmt != NULL)
    //     sqlite3_finalize(stmt);
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

        if(d->ucPara[i].ucOperate == 0x01){
            afn_11_f3_group_ctrl(&d->ucPara[i].ucAddress[4], d->ucData[i].Status);
        }
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
        case CMD_AFN_1_F5_UPDATE:
            afn_1_f5_update_handle();
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
            printf("实时对时\n");
            if(puAppData == NULL)
                printf("puAppData error\n");
            if(afn_05_f31_clock((sMtUserClock*)puAppData) == APP_HANDLE_SUCCESS)
                pack_afn00f01(AFN_05_CTRL);
            else
                printf("日控时段表配置失败\n");
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

static int cmp( const void *a , const void *b )
{
      return *(UINT16 *)a - *(UINT16 *)b;
}

static void app_groupInfo_pack(UINT16 d, char* groupInfo)
{
    int i, num;

    UINT16 pn[255] = {0};

    num = groupInfo[0];
    if(0 == num)
    {
        groupInfo[0] = 1 + '0';
        groupInfo[1] = '0';
        d += '0';
        memcpy(&groupInfo[2], (UINT8*)&d, 2);
        return;
    }else{
        num -= '0';
    }

    printf("groupInfo长度:%d,排序:%s\n", num, groupInfo);
    /*取出已有的Pn*/
    {
        printf("pn:");
        for(i = 0; i < num ; i++)
        {
            pn[i] = *(UINT16*)&groupInfo[i*2 + 2];
            printf(" %03d,",pn[i]-'0');
        }
        printf("\n");
    }
    /*加入传入的新Pn*/
    {
        printf("new d:%03d\n",d);
        pn[i] = d + '0';
        printf("new Pn[i]:%03d\n",pn[i]);
        num+=1;
        /*更新pn个数*/
        groupInfo[0] = (num & 0xff)+'0';
    }
    /*排序*/
    {
        qsort(pn, num, sizeof(pn[0]), cmp);
        printf("sort pn:");
        for(i = 0; i < num; i++)
        {
            printf(" %03d,",pn[i] - '0');
        }
        printf("\n");
    }
    /*组装排序完成的groupInfo*/
    {
        for(i = 0; i < num; i++)
        {
            memcpy(&groupInfo[i*2 + 2], (UINT8*)&pn[i], 2);
            printf("groupInfoNew:%02X\n",groupInfo[i*2 + 2]-'0');       
        }
    }

}

int app_groupInfo_unpack(UINT16* d, char* groupInfo)
{
    int i, num;

    UINT16 pn[255] = {0};

    num = groupInfo[0];
    if(num >0)
        num-='0';
    printf("groupInfo长度:%d,排序:%s\n", num, groupInfo);
    
    /*取出已有的Pn*/
    {
        printf("get pn:");
        for(i = 0; i < num; i++)
        {
            pn[i] = *(UINT16*)&groupInfo[i*2+2] - '0';
            printf(" %03d,",pn[i]);
        }
        printf("\n");
    }

    memcpy((UINT8*)d, (UINT8*)pn, num*2);

    return num;
}



