#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sqlite3.h"
#include "app_handle.h"
#include "app_sql.h"


extern sqlite3 *DCS003_db;

static int UINT8_to_ascii(int d, char* buf);
static void app_groupInfo_pack(UINT16 d, char* groupInfo);
static int afn_11_f3_group_ctrl(UINT8* groupid, UINT8 state);
static int app_groupInfo_pn_delete(UINT16 d, char* groupInfo);
static int afn_11_f2_delete_config(char* startdate, int type);
static int afn_04_f13_delete_pn(UINT16 pn);
static int afn_04_f13_delete_group(char* group);
static int afn_04_f13_delete_group_pn(UINT16 usPn, char* group);
static int afn_04f3_group_config_handle(UINT8* d, char* group);
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
/*终端上行通信端口通信参数设置*/
int afn_04_f1_ctrl_table(sMtTmlUpCfg* d)
{   
    printf("afn_04_f1_ctrl_table\n");
    char sql[300]      = {0};
    int  ret           = APP_HANDLE_SUCCESS;
    int  rc            = 0;
    int  reSendTimes   = 0;
    int  outtime       = 0;
    int  heartBeatTime = 0;    
    sqlite3_stmt* stmt = NULL;  

    printf("ucRTS = %d\n", d->ucRTS);
    printf("ucRTM = %d\n", d->ucRTM);
    printf("ucReSendTimes = %d\n", d->ucReSendTimes);
    printf("ucHeartBeat = %d\n", d->ucHeartBeat);
    printf("usTmWaitTimeoutS = %d\n", d->usTmWaitTimeoutS);
    printf("bAutoReportAsk_1 = %s\n", d->bAutoReportAsk_1 == TRUE ? "true":"false");
    printf("bAutoReportAsk_2 = %s\n", d->bAutoReportAsk_2 == TRUE ? "true":"false");
    printf("bAutoReportAsk_3 = %s\n", d->bAutoReportAsk_3 == TRUE ? "true":"false");

    {
        reSendTimes   = d->ucReSendTimes;
        outtime       = d->usTmWaitTimeoutS;
        heartBeatTime = d->ucHeartBeat;
    }
    /*更新数据库dal_cbt_ecbox*/
    {
        rc = app_sql_open();
        if(rc != APP_HANDLE_SUCCESS){
            ret = APP_HANDLE_FAILED;
            goto Finish;
        }

        sprintf(sql,"update dal_cbt_ecbox set reSendTimes = ?,outtime = ?,heartBeatTime = ?");
        printf("sql:%s\n",sql);

        if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
        {
            printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
            ret = APP_HANDLE_FAILED;
            goto Finish; 
        }

        sqlite3_bind_int(stmt,1,reSendTimes);
        sqlite3_bind_int(stmt,2,outtime);
        sqlite3_bind_int(stmt,3,heartBeatTime);
        rc = sqlite3_step(stmt);   
        if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE)){
            printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
        }

    }

    Finish:
    app_sql_close();
    if(stmt != NULL)
        sqlite3_finalize(stmt);
    return ret;
}

/**/
int afn_04_f3_ctrl_table(sMtMasterIpPort* d)
{
    printf("afn_04_f3_ctrl_table\n");

    char  sql[300]      = {0};
    char  group[5]      = {0};
    int   ret           = APP_HANDLE_SUCCESS;
    int   rc            = 0;
    int   i             = 0;
    UINT8 apnType       = 0;
    UINT8 Pn[64]        = {0};
    sqlite3_stmt* stmt  = NULL;  

    printf("主ip版本 = %d\n", d->ulMainVersion);
    printf("主用IP地址 = %03d,%03d,%03d,%03d\n", *((UINT8*)&d->ulMainIP),\
                                                *((UINT8*)&d->ulMainIP+1),\
                                                *((UINT8*)&d->ulMainIP+2),\
                                                *((UINT8*)&d->ulMainIP+3));
    printf("主端口地址 = %d\n", d->usMainPort);
    printf("备用ip版本 = %d\n", d->ulBackVersion);
    printf("主用IP地址 = %03d,%03d,%03d,%03d\n", *((UINT8*)&d->ulBackIP),\
                                                *((UINT8*)&d->ulBackIP+1),\
                                                *((UINT8*)&d->ulBackIP+2),\
                                                *((UINT8*)&d->ulBackIP+3));
    printf("备用端口地址 = %d\n", d->usBackPort);

    printf("APN类型:%x\n",d->ucascAPN[0]);
    printf("APN 组id:%x,%x,%x,%x\n",d->ucascAPN[1],d->ucascAPN[2],d->ucascAPN[3],d->ucascAPN[4]);

    {
        apnType = d->ucascAPN[0];
        /*数据库中组信息*/
        for(i = 0; i < 4; i++)
        {
            group[i] = d->ucascAPN[1 + i] + '0';
        }

    }

    rc = app_sql_open();
    if(rc != APP_HANDLE_SUCCESS){
        ret = APP_HANDLE_FAILED;
        goto Finish;
    }

    /*判断是否为配置组信息*/
    {
        if(apnType == AFN_04_F3_GROUP_CONFIG)
        {
            /*pn通道配置低32字节*/
            memcpy(Pn, d->virUserName, 32);
            /*pn通道配置高32字节*/
            if(d->virUserPw != 0x00)
            {
                memcpy(Pn+32, d->virUserPw, 32);
            }
            printf("04f3配置组信息\n");

            afn_04f3_group_config_handle(Pn, group);

            goto Finish;
        }
    }
    /*更新数据库dal_cbt_ecbox*/
    {
        sprintf(sql,"update dal_cbt_ecbox set ip = ?,port = ?,bakip = ?,bakport = ?;");
        printf("sql:%s\n",sql);

        if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
        {
            printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
            ret = APP_HANDLE_FAILED;
            goto Finish; 
        }

        sqlite3_bind_int(stmt,1,d->ulMainIP);
        sqlite3_bind_int(stmt,2,d->usMainPort);
        sqlite3_bind_int(stmt,3,d->ulBackIP);
        sqlite3_bind_int(stmt,4,d->usBackPort);
        rc = sqlite3_step(stmt);   
        if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE)){
            printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
        }
    }

    Finish:
    if(stmt != NULL)
        sqlite3_finalize(stmt);
    app_sql_close();
    return ret;
}

/*终端电能表控制参数配置*/
int afn_04_f13_ctrl_table(sMtAfn04F13* d)
{
	printf("afn_04_f13_ctrl_table\n");
    
    int i, rc,ret = APP_HANDLE_SUCCESS;
    char* sql = NULL;
    char sql_1[300] = {0};
    char groupInfoNew[512] = {0};
    char box[7] = {0};
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

    /*sqlite open*/
    rc = app_sql_open();
    if(rc != APP_HANDLE_SUCCESS){
        ret = APP_HANDLE_FAILED;
        goto Finish;
    }

    switch(d->Port)
    {
        case SQL_GROUP_DEL:
        /*判断是否为删除组*/
        {
            afn_04_f13_delete_group(Group);
            goto Finish;
        }
        break;
        case SQL_GROUP_PN_DEL:
        /*判断是否为删除组内测量点*/
        {
            afn_04_f13_delete_group_pn(usPn, Group);
            goto Finish;
        }
        break;
        case SQL_PN_DEL:
        /*判断是否为删除测量点*/
        {
            afn_04_f13_delete_pn(usPn);
            goto Finish;
        }
        break;

        default:
        break;
    }

    {  
         /*插入dal_cbt_dp*/
        {
            sql = "insert or replace into dal_cbt_dp(id,dp,dpType,dpNature,dpSign,port,portType \
            ,box,channelNumber,state,Poslong,Poslit)values(NULL,?,?,?,?,?,?,?,?,?,?,?)";
            printf("sql:%s\n",sql);
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
        

    }

	Finish:
    // if(stmt != NULL)
    //     sqlite3_finalize(stmt);
    app_sql_close();

	return ret;
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

    return APP_HANDLE_SUCCESS;
}

/*时段控制检查*/
int afn_11_f2_ctrl_period(void)
{
    printf("afn_11_f2_ctrl_period\n");
    static int preTimeMin = 0;
    UINT8 state = NULL;
    char* taskExecTime = NULL;
    char currTime[7] = {0};
    int rc;
    int i = 0;
    int ret = APP_HANDLE_SUCCESS;
    char* groupid = 0;
    char groupid_1[8] = {0};
    char sql[300] = {0};
    char sql_1[300] = {0};
    time_t now;
    struct tm* timenow;
    sqlite3_stmt* stmt = NULL;

    // 获取系统当前时间
    {
        time(&now);
        timenow = localtime(&now);
        
        printf("年:%03d 月:%03d 日:%03d 周:%03d 时:%03d 分:%03d\n",timenow->tm_year, \
            timenow->tm_mon,timenow->tm_mday,timenow->tm_wday,timenow->tm_hour,timenow->tm_min);

        /*检查的最小时间刻度为Min*/
        if(preTimeMin != timenow->tm_min)
        {
            preTimeMin = timenow->tm_min; 

            currTime[0] = (timenow->tm_year - 100) + 1;
            currTime[1] = (timenow->tm_mon + 1) + 1;
            currTime[2] = timenow->tm_mday + 1;
            if(timenow->tm_wday == 0)
                currTime[3] = 7;
            else
                currTime[3] = timenow->tm_wday;
            currTime[4] = timenow->tm_hour + 1;
            currTime[5] = timenow->tm_min + 1;
            printf("currTime:%x,%x,%x,%x,%x\n",currTime[0],currTime[1],currTime[2], \
                currTime[3],currTime[4],currTime[5]);
        }else{

            return ret;
        }
    }

    {
        rc = app_sql_open();
        if(rc != APP_HANDLE_SUCCESS)
        {
            ret = APP_HANDLE_FAILED;
            goto Finish;
        }

        /*特定任务年*/
        {
            {
                currTime[0] = (timenow->tm_year - 100) + 1;
                currTime[1] = (timenow->tm_mon + 1) + 1;
                currTime[2] = timenow->tm_mday + 1;
                currTime[3] = 1;
                currTime[4] = timenow->tm_hour + 1;
                currTime[5] = timenow->tm_min + 1;
            }
            sprintf(sql,"select groupid,state from dal_cbt_clock where tasktype = %d and startdate = \"%s\";"\
                ,TASK_TYPE_YEAR, currTime);
            printf("sql:%s\n",sql);

            if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
            {
                printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                ret = APP_HANDLE_FAILED;
                goto Finish; 
            }

            while(rc = sqlite3_step(stmt) == SQLITE_ROW) 
            {  
                groupid = sqlite3_column_text(stmt, 0);
                if(groupid == NULL)
                {
                    printf("groupid NULL");
                    continue;
                }
                /*组id转换*/
                for(i = 0; i < 4; i++)
                    groupid_1[i] = groupid[i+4] - '0';
                
                printf("groupid_1:%x,%x,%x,%x\n",groupid_1[0],groupid_1[1],groupid_1[2],groupid_1[3]);
                state = sqlite3_column_int(stmt, 1);
                printf("state:%x\n",state);
                /*根据组id查询box号与通道*/
                {
                    if(afn_11_f3_group_ctrl((UINT8*)groupid_1, state) != APP_HANDLE_SUCCESS)
                        printf("afn_11_f3_group_ctrl failed\n");
                }

                goto Finish;
            }
            if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE) && (rc != SQLITE_OK)){
                printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
            }
            if(stmt != NULL)
                sqlite3_finalize(stmt);
        }

        /*特定任务月*/
        {
            {
                currTime[0] = 1;
                currTime[1] = 1;
                currTime[2] = timenow->tm_mday + 1;
                currTime[3] = 1;
                currTime[4] = timenow->tm_hour + 1;
                currTime[5] = timenow->tm_min + 1;
            }
            sprintf(sql,"select groupid,state from dal_cbt_clock where tasktype = %d and startdate = \"00%s\";"\
                ,TASK_TYPE_MON, currTime);
            printf("sql:%s\n",sql);

            if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
            {
                printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                ret = APP_HANDLE_FAILED;
                goto Finish; 
            }

            while(rc = sqlite3_step(stmt) == SQLITE_ROW) 
            {  
                groupid = sqlite3_column_text(stmt, 0);
                if(groupid == NULL)
                {
                    printf("groupid NULL");
                    continue;
                }
                /*组id转换*/
                for(i = 0; i < 4; i++)
                    groupid_1[i] = groupid[i+4] - '0';
                
                printf("groupid_1:%x,%x,%x,%x\n",groupid_1[0],groupid_1[1],groupid_1[2],groupid_1[3]);
                state = sqlite3_column_int(stmt, 1);
                printf("state:%x\n",state);
                /*根据组id查询box号与通道*/
                {
                    if(afn_11_f3_group_ctrl((UINT8*)groupid_1, state) != APP_HANDLE_SUCCESS)
                        printf("afn_11_f3_group_ctrl failed\n");
                }

                goto Finish;
            }
            if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE) && (rc != SQLITE_OK)){
                printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
            }
            if(stmt != NULL)
                sqlite3_finalize(stmt);
        }

        /*特定任务周*/
        {
             {
                currTime[0] = 1;
                currTime[1] = 1;
                currTime[2] = 1;
                if(timenow->tm_wday == 0)
                    currTime[3] = 7;
                else
                    currTime[3] = timenow->tm_wday;
                currTime[4] = timenow->tm_hour + 1;
                currTime[5] = timenow->tm_min + 1;
            }
            
            printf("currTime:%x\n", currTime[3]);

            sprintf(sql,"select groupid,state,startdate from dal_cbt_clock where tasktype = %d;"\
                ,TASK_TYPE_WEEK);
            printf("sql:%s\n",sql);

            if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
            {
                printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                ret = APP_HANDLE_FAILED;
                goto Finish; 
            }

            while(rc = sqlite3_step(stmt) == SQLITE_ROW) 
            {  
                taskExecTime = sqlite3_column_text(stmt, 2);
                if(taskExecTime == NULL)
                {
                    printf("taskExecTime NULL");
                    continue;
                }
                printf("taskExecTime:%s\n",taskExecTime);
                /*判断是否属于该周*/
                if((taskExecTime[3] - 1) & ((1 << (currTime[3] - 1))&0xff))
                {
                    //if(strncmp(&taskExecTime[4], &currTime[4],2) == 0)
                    if((taskExecTime[4] == currTime[4]) && (taskExecTime[5] == currTime[5]))
                    {
                        printf("同一周,同一时段\n");
                        groupid = sqlite3_column_text(stmt, 0);
                        if(groupid == NULL)
                        {
                            printf("groupid NULL");
                            continue;
                        }
                        /*组id转换*/
                        for(i = 0; i < 4; i++)
                            groupid_1[i] = groupid[i+4] - '0';
                        
                        printf("groupid_1:%x,%x,%x,%x\n",groupid_1[0],groupid_1[1],groupid_1[2],groupid_1[3]);
                        state = sqlite3_column_int(stmt, 1);
                        printf("state:%x\n",state);
                        /*根据组id查询box号与通道*/
                        {
                            if(afn_11_f3_group_ctrl((UINT8*)groupid_1, state) != APP_HANDLE_SUCCESS)
                                printf("afn_11_f3_group_ctrl failed\n");
                        }
        
                        goto Finish;
                    }
                }

            }
            if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE) && (rc != SQLITE_OK))
            {
                printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
            }
            if(stmt != NULL)
                sqlite3_finalize(stmt);
        }

        /*特定任务日*/
        {
            {
                currTime[0] = 1;
                currTime[1] = 1;
                currTime[2] = 1;
                currTime[3] = 1;
                currTime[4] = timenow->tm_hour + 1;
                currTime[5] = timenow->tm_min + 1;
            }
            sprintf(sql,"select groupid,state from dal_cbt_clock where tasktype = %d and startdate = \"%s\";"\
                ,TASK_TYPE_DAY, currTime);
            printf("sql:%s\n",sql);

            if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
            {
                printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                ret = APP_HANDLE_FAILED;
                goto Finish; 
            }

            while(rc = sqlite3_step(stmt) == SQLITE_ROW) 
            {  
                groupid = sqlite3_column_text(stmt, 0);
                if(groupid == NULL)
                {
                    printf("groupid NULL");
                    continue;
                }
                /*组id转换*/
                for(i = 0; i < 4; i++)
                    groupid_1[i] = groupid[i+4] - '0';
                
                printf("groupid_1:%x,%x,%x,%x\n",groupid_1[0],groupid_1[1],groupid_1[2],groupid_1[3]);
                state = sqlite3_column_int(stmt, 1);
                printf("state:%x\n",state);
                /*根据组id查询box号与通道*/
                {
                    if(afn_11_f3_group_ctrl((UINT8*)groupid_1, state) != APP_HANDLE_SUCCESS)
                        printf("afn_11_f3_group_ctrl failed\n");
                }

                goto Finish;
            }
            if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE) && (rc != SQLITE_OK)){
                printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
            }
            if(stmt != NULL)
                sqlite3_finalize(stmt);
        }


        /*特定任务时*/
        {
            {
                currTime[0] = 1;
                currTime[1] = 1;
                currTime[2] = 1;
                currTime[3] = 1;
                currTime[4] = 1;
                currTime[5] = timenow->tm_min + 1;
            }
            sprintf(sql,"select groupid,state from dal_cbt_clock where tasktype = %d and startdate = \"%s\";"\
                ,TASK_TYPE_HOUR, currTime);
            printf("sql:%s\n",sql);

            if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
            {
                printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                ret = APP_HANDLE_FAILED;
                goto Finish; 
            }

            while(rc = sqlite3_step(stmt) == SQLITE_ROW) 
            {  
                groupid = sqlite3_column_text(stmt, 0);
                if(groupid == NULL)
                {
                    printf("groupid NULL");
                    continue;
                }
                /*组id转换*/
                for(i = 0; i < 4; i++)
                    groupid_1[i] = groupid[i+4] - '0';
                
                printf("groupid_1:%x,%x,%x,%x\n",groupid_1[0],groupid_1[1],groupid_1[2],groupid_1[3]);
                state = sqlite3_column_int(stmt, 1);
                printf("state:%x\n",state);
                /*根据组id查询box号与通道*/
                {
                    if(afn_11_f3_group_ctrl((UINT8*)groupid_1, state) != APP_HANDLE_SUCCESS)
                        printf("afn_11_f3_group_ctrl failed\n");
                }

                goto Finish;
            }
            if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE) && (rc != SQLITE_OK)){
                printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
            }
            if(stmt != NULL)
                sqlite3_finalize(stmt);
        }

        /*普通任务年*/
        {
            {
                currTime[0] = (timenow->tm_year - 100) + 1;
                currTime[1] = (timenow->tm_mon + 1) + 1;
                currTime[2] = timenow->tm_mday + 1;
                currTime[3] = 1;
                currTime[4] = timenow->tm_hour + 1;
                currTime[5] = timenow->tm_min + 1;
            }
            sprintf(sql,"select groupid,state from dal_cbt_clock where tasktype = %d and startdate = \"%s\";"\
                ,TASK_TYPE_YEAR & TASK_TYPE_COMMON_MASK, currTime);
            printf("sql:%s\n",sql);

            if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
            {
                printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                ret = APP_HANDLE_FAILED;
                goto Finish; 
            }

            while(rc = sqlite3_step(stmt) == SQLITE_ROW) 
            {  
                groupid = sqlite3_column_text(stmt, 0);
                if(groupid == NULL)
                {
                    printf("groupid NULL");
                    continue;
                }
                /*组id转换*/
                for(i = 0; i < 4; i++)
                    groupid_1[i] = groupid[i+4] - '0';
                
                printf("groupid_1:%x,%x,%x,%x\n",groupid_1[0],groupid_1[1],groupid_1[2],groupid_1[3]);
                state = sqlite3_column_int(stmt, 1);
                printf("state:%x\n",state);
                /*根据组id查询box号与通道*/
                {
                    if(afn_11_f3_group_ctrl((UINT8*)groupid_1, state) != APP_HANDLE_SUCCESS)
                        printf("afn_11_f3_group_ctrl failed\n");
                }

                goto Finish;
            }
            if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE) && (rc != SQLITE_OK)){
                printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
            }
            if(stmt != NULL)
                sqlite3_finalize(stmt);
        }

        /*普通任务月*/
        {
          {
                currTime[0] = 1;
                currTime[1] = 1;
                currTime[2] = timenow->tm_mday + 1;
                currTime[3] = 1;
                currTime[4] = timenow->tm_hour + 1;
                currTime[5] = timenow->tm_min + 1;
            }
            sprintf(sql,"select groupid,state from dal_cbt_clock where tasktype = %d and startdate = \"%s\";"\
                ,TASK_TYPE_MON & TASK_TYPE_COMMON_MASK, currTime);
            printf("sql:%s\n",sql);

            if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
            {
                printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                ret = APP_HANDLE_FAILED;
                goto Finish; 
            }

            while(rc = sqlite3_step(stmt) == SQLITE_ROW) 
            {  
                groupid = sqlite3_column_text(stmt, 0);
                if(groupid == NULL)
                {
                    printf("groupid NULL");
                    continue;
                }
                /*组id转换*/
                for(i = 0; i < 4; i++)
                    groupid_1[i] = groupid[i+4] - '0';
                
                printf("groupid_1:%x,%x,%x,%x\n",groupid_1[0],groupid_1[1],groupid_1[2],groupid_1[3]);
                state = sqlite3_column_int(stmt, 1);
                printf("state:%x\n",state);
                /*根据组id查询box号与通道*/
                {
                    if(afn_11_f3_group_ctrl((UINT8*)groupid_1, state) != APP_HANDLE_SUCCESS)
                        printf("afn_11_f3_group_ctrl failed\n");
                }

                goto Finish;
            }
            if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE) && (rc != SQLITE_OK)){
                printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
            }
            if(stmt != NULL)
                sqlite3_finalize(stmt);
        }

        /*普通任务周*/
        {
             {
                currTime[0] = 1;
                currTime[1] = 1;
                currTime[2] = 1;
                if(timenow->tm_wday == 0)
                    currTime[3] = 7;
                else
                    currTime[3] = timenow->tm_wday;
                currTime[4] = timenow->tm_hour + 1;
                currTime[5] = timenow->tm_min + 1;
            }
            printf("week:%d\n", currTime[3]);
            sprintf(sql,"select groupid,state,startdate from dal_cbt_clock where tasktype = %d;"\
                ,TASK_TYPE_WEEK & TASK_TYPE_COMMON_MASK);
            printf("sql:%s\n",sql);

            if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
            {
                printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                ret = APP_HANDLE_FAILED;
                goto Finish; 
            }

            while(rc = sqlite3_step(stmt) == SQLITE_ROW) 
            {  
                taskExecTime = sqlite3_column_text(stmt, 2);
                if(taskExecTime == NULL)
                {
                    printf("taskExecTime NULL");
                    continue;
                }
                printf("taskWeek:%02x,taskExecTime:%02x\n",*(UINT8*)&taskExecTime[3] - 1,*(UINT8*)&taskExecTime[4] - 1);
                /*判断是否属于该周*/
                if((taskExecTime[3] - 1) & ((1 << (currTime[3] - 1))&0xff))
                {
                    //if(strncmp(&taskExecTime[4], &currTime[4],2) == 0)
                    if((taskExecTime[4] == currTime[4]) && (taskExecTime[5] == currTime[5]))
                    {
                        printf("同一周,同一时段\n");
                        groupid = sqlite3_column_text(stmt, 0);
                        if(groupid == NULL)
                        {
                            printf("groupid NULL");
                            continue;
                        }
                        /*组id转换*/
                        for(i = 0; i < 4; i++)
                            groupid_1[i] = groupid[i+4] - '0';
                        
                        printf("groupid_1:%x,%x,%x,%x\n",groupid_1[0],groupid_1[1],groupid_1[2],groupid_1[3]);
                        state = sqlite3_column_int(stmt, 1);
                        printf("state:%x\n",state);
                        /*根据组id查询box号与通道*/
                        {
                            if(afn_11_f3_group_ctrl((UINT8*)groupid_1, state) != APP_HANDLE_SUCCESS)
                                printf("afn_11_f3_group_ctrl failed\n");
                        }
        
                        goto Finish;
                    }
                }

            }
            if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE) && (rc != SQLITE_OK))
            {
                printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
            }
            if(stmt != NULL)
                sqlite3_finalize(stmt);
        }

        /*普通任务日*/
        {
             {
                currTime[0] = 1;
                currTime[1] = 1;
                currTime[2] = 1;
                currTime[3] = 1;
                currTime[4] = timenow->tm_hour + 1;
                currTime[5] = timenow->tm_min + 1;
            }

            sprintf(sql,"select groupid,state from dal_cbt_clock where tasktype = %d and startdate = \"%s\";"\
                ,TASK_TYPE_DAY & TASK_TYPE_COMMON_MASK, currTime);
            printf("sql:%s\n",sql);

            if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
            {
                printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                ret = APP_HANDLE_FAILED;
                goto Finish; 
            }

            while(rc = sqlite3_step(stmt) == SQLITE_ROW) 
            {  
                groupid = sqlite3_column_text(stmt, 0);
                if(groupid == NULL)
                {
                    printf("groupid NULL");
                    continue;
                }
                /*组id转换*/
                for(i = 0; i < 4; i++)
                    groupid_1[i] = groupid[i+4] - '0';
                
                printf("groupid_1:%x,%x,%x,%x\n",groupid_1[0],groupid_1[1],groupid_1[2],groupid_1[3]);
                state = sqlite3_column_int(stmt, 1);
                printf("state:%x\n",state);
                /*根据组id查询box号与通道*/
                {
                    if(afn_11_f3_group_ctrl((UINT8*)groupid_1, state) != APP_HANDLE_SUCCESS)
                        printf("afn_11_f3_group_ctrl failed\n");
                }

                goto Finish;
            }
            if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE) && (rc != SQLITE_OK)){
                printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
            }
            if(stmt != NULL)
                sqlite3_finalize(stmt);
        }

        /*普通任务时*/
        {
            {
                currTime[0] = 1;
                currTime[1] = 1;
                currTime[2] = 1;
                currTime[3] = 1;
                currTime[4] = 1;
                currTime[5] = timenow->tm_min + 1;
            }
            sprintf(sql,"select groupid,state from dal_cbt_clock where tasktype = %d and startdate = \"%s\";"\
                ,TASK_TYPE_HOUR & TASK_TYPE_COMMON_MASK, currTime);
            printf("sql:%s\n",sql);

            if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
            {
                printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                ret = APP_HANDLE_FAILED;
                goto Finish; 
            }

            while(rc = sqlite3_step(stmt) == SQLITE_ROW) 
            {  
                groupid = sqlite3_column_text(stmt, 0);
                if(groupid == NULL)
                {
                    printf("groupid NULL");
                    continue;
                }
                /*组id转换*/
                for(i = 0; i < 4; i++)
                    groupid_1[i] = groupid[i+4] - '0';

                printf("groupid_1:%x,%x,%x,%x\n",groupid_1[0],groupid_1[1],groupid_1[2],groupid_1[3]);
                state = sqlite3_column_int(stmt, 1);
                printf("state:%x\n",state);
                /*根据组id查询box号与通道*/
                {
                    if(afn_11_f3_group_ctrl((UINT8*)groupid_1, state) != APP_HANDLE_SUCCESS)
                        printf("afn_11_f3_group_ctrl failed\n");
                }

                goto Finish;
            }
            if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE) && (rc != SQLITE_OK)){
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
}

/*日控时段*/
int afn_11_f2_ctrl_table(sMt11f2_u* d)
{
	printf("afn_11_f2_ctrl_table\n");

	int i,j,k,rc,ret = APP_HANDLE_SUCCESS;
	unsigned char startdate[20] = {0};
	unsigned char ucAddress[20] = {0};
	char* state = NULL;
	char* sql =	"insert or replace into dal_cbt_clock(id,groupid,startdate,state,percent,tasktype)values(NULL,?,?,?,?,?);";	
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

	printf("任务格式:%02X \
			任务类型:%X \
			任务长度:%x \
			年:%X 月:%X 	日:%X 周:%x\
			持续时间:%X \
			时段数据块总数: %X\n",d->ucTaskFormat, d->ucTaskType, \
								 d->ucTaskLen, d->sT.ucYY, d->sT.ucMM, \
								 d->sT.ucDD,d->sT.ucWW, d->LastTime, d->ucTimeNum);
	{
		/*添加年月日周*/
		startdate[0] = d->sT.ucYY + 1;
		startdate[1] = d->sT.ucMM + 1;
		startdate[2] = d->sT.ucDD + 1;
        startdate[3] = d->sT.ucWW + 1;
	}

    /*为删除配置时段类型则无需插入*/
    if(d->ucTaskType < TASK_TYPE_DELETE_MASK)
    {
        if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK){
                printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                ret = APP_HANDLE_FAILED;
                goto Finish; 
        }
    }

	for(i = 0; i < d->ucTimeNum; i++)
	{
		printf("时段%X开始时间,分:%X 时:%X  \
				开关状态:%X \
				亮度百分比:%X \
				",i,d->Time[i].Min, d->Time[i].Hour, d->Time[i].Status, d->Time[i].Light);
		{
			/*时*/
			startdate[4] = d->Time[i].Hour + 1;
			/*分*/
			startdate[5] = d->Time[i].Min + 1;
        }
        /*判断是否为删除时段类型*/
        {
            if(d->ucTaskType > TASK_TYPE_DELETE_MASK)
            {
                afn_11_f2_delete_config(startdate, d->ucTaskType);
                // if(sqlite3_exec(DCS003_db,"delete from dal_cbt_clock",0,0,&zErrMsg) != SQLITE_OK)
                //     printf("11_f2_delete_config failed\n");
                continue;
            }
        }

		{
            state = d->Time[i].Status;
		}

		printf("控制参数块数:%X\n", d->ucDataNum);
		for(j = 0; j < d->ucDataNum; j++)
		{
			printf("操作类型:%X ",d->Data[j].ucOperate);
			for(k = 0; k < 8; k++)
			{
				printf("控制器地址:%X, ",d->Data[j].ucAddress[k]);
                ucAddress[k] = d->Data[j].ucAddress[k] + '0';
			}
			printf("\n");
			for(k = 0; k < 32; k++)
			{
				printf("控制器密钥:%X ", d->Data[j].ucKey[k]);
			}
			printf("\n");
			{
	            sqlite3_bind_text(stmt, 1, ucAddress, -1, NULL);
                printf("startdate:%x,%x,%x,%x,%x,%x\n",startdate[0],startdate[1],startdate[2],startdate[3],\
                    startdate[4],startdate[5]);
	            sqlite3_bind_text(stmt, 2, startdate, -1, NULL);
	            //sqlite3_bind_text(stmt, 3, state, -1, NULL);
                sqlite3_bind_int(stmt, 3, state);
	            sqlite3_bind_int(stmt, 4, d->Time[i].Light);
                sqlite3_bind_int(stmt, 5, d->ucTaskType);
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
 
    int           i            = 0;
    int           rc           = 0;
    int           len          = 0;
    int           ret          = APP_HANDLE_SUCCESS;
    char          sql[300]     = {0};
    char*         groupInfo    = NULL;
    char          groupid_1[5] = {0};
    UINT16        pn[255]      = {0};
    UINT8*        box          = NULL;
    UINT8         channelNumber= 0;
    sqlite3_stmt* stmt         = NULL;
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
        if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE)){
            printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
        }

        groupInfo = sqlite3_column_text(stmt, 0);
        if(NULL == groupInfo){
            printf("groupInfo NULL\n");
            ret = APP_HANDLE_FAILED;
            //goto Finish;
        }else{
            /*将group表中的信息转换成pn*/
            len = app_groupInfo_unpack(pn, groupInfo);
        }
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

    return ret;
}

int afn_11_f3_ctrl_table(sMtCtr_11hf3* d)
{
    int i,j,rc,ret = APP_HANDLE_SUCCESS;

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

    rc = app_sql_open();
    if(rc != APP_HANDLE_SUCCESS)
    {
        ret = APP_HANDLE_FAILED;
        goto Finish;
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

    Finish:
    app_sql_close();

    return APP_HANDLE_SUCCESS;
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

            show_error("终端上行通信端口通信参数设置\n");
            if(puAppData == NULL)
                printf("puAppData error\n");
            if(afn_04_f1_ctrl_table((sMtTmlUpCfg*)puAppData) == APP_HANDLE_SUCCESS)
                pack_afn00f01(CMD_AFN_4_F1_TML_UP_CFG);
            else
                printf("终端上行通信端口通信参数设置\n");
        }
        break;
        case CMD_AFN_4_F3_MST_IP_PORT:
        {
            show_error("主站IP地址和端口\n");
            if(puAppData == NULL)
                printf("puAppData error\n");
            if(afn_04_f3_ctrl_table((sMtMasterIpPort*)puAppData) == APP_HANDLE_SUCCESS)
                pack_afn00f01(CMD_AFN_4_F3_MST_IP_PORT);
            else
                printf("主站IP地址和端口\n");
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

/*将pn加入到groupInfo中*/
static void app_groupInfo_pack(UINT16 d, char* groupInfo)
{
    int i, num;

    UINT16 pn[255] = {0};

    num = groupInfo[0];
    if(0 == num)
    {
        groupInfo[0] = 1;
        groupInfo[1] = 0;
        d = 1;
        memcpy(&groupInfo[2], (UINT8*)&d, 2);
        return;
    }

    printf("groupInfo长度:%d\n", num);
    /*取出已有的Pn*/
    {
        printf("pn:");
        for(i = 0; i < num ; i++)
        {
            if(*(UINT16*)&groupInfo[i*2 + 2] == d)
            {
                printf("Pn重复：%03d\n",d);
                return;
            }
            pn[i] = *(UINT16*)&groupInfo[i*2 + 2];
            printf(" %03d,",pn[i]);
        }
        printf("\n");
    }
    /*加入传入的新Pn*/
    {
        printf("new d:%03d\n",d);
        pn[i] = d;
        printf("new Pn[i]:%03d\n",pn[i]);
        num+=1;
        /*更新pn个数*/
        groupInfo[0] = (num & 0xff);
    }
    /*排序*/
    {
        qsort(pn, num, sizeof(pn[0]), cmp);
        printf("sort pn:");
        for(i = 0; i < num; i++)
        {
            printf(" %03d,",pn[i]);
        }
        printf("\n");
    }
    /*组装排序完成的groupInfo*/
    {
        for(i = 0; i < num; i++)
        {
            memcpy(&groupInfo[i*2 + 2], (UINT8*)&pn[i], 2);
            printf("groupInfoNew:%02X\n",groupInfo[i*2 + 2]);       
        }
    }

}

/*从groupInfo中取出pn*/
int app_groupInfo_unpack(UINT16* d, char* groupInfo)
{
    int i, num;

    UINT16 pn[255] = {0};

    num = groupInfo[0];
    printf("groupInfo长度:%d\n", num);
    
    /*取出已有的Pn*/
    {
        printf("get pn:");
        for(i = 0; i < num; i++)
        {
            pn[i] = *(UINT16*)&groupInfo[i*2+2];
            printf(" %03d,",pn[i]);
        }
        printf("\n");
    }

    memcpy((UINT8*)d, (UINT8*)pn, num*2);

    return num;
}

/*删除groupInfo中的Pn*/
static int app_groupInfo_pn_delete(UINT16 d, char* groupInfo)
{
    int i;
    int j;
    int num;
    int deleteFlag = 0;

    UINT16 pn[255] = {0};

    num = groupInfo[0];
    if(0 == num)
    {
        printf("no Pn to delete!\n");
        return;
    }

    printf("groupInfo长度:%d\n", num);
    /*取出已有的Pn*/
    {
        printf("pn:");
        for(i = 0,j=0; i < num ; i++)
        {
            if(*(UINT16*)&groupInfo[i*2 + 2] != d)
            {
                pn[j] = *(UINT16*)&groupInfo[i*2 + 2];
                j++;
                printf(" %03d,",pn[i]);
            }else{
                deleteFlag = 1;
            }
            
        }
        printf("\n");
    }
    /*重新计算groupInfo中的pn*/
    {
        num = j;
        /*更新pn个数*/
        groupInfo[0] = (num & 0xff);
    }

    /*组装排序完成的groupInfo*/
    {
        for(i = 0; i < num; i++)
        {
            memcpy(&groupInfo[i*2 + 2], (UINT8*)&pn[i], 2);
            printf("groupInfoNew:%02X\n",groupInfo[i*2 + 2]);       
        }
    }

    return deleteFlag;

}

/*删除测量点号*/
static int afn_04_f13_delete_pn(UINT16 pn)
{
    int           ret            = APP_HANDLE_SUCCESS;
    int           rc             = 0;
    char*         groupid        = NULL;
    char          sql[300]       = {0};
    char*         sql_1          = NULL;
    char*         groupInfo      = NULL;
    sqlite3_stmt* stmt           = NULL;
    sqlite3_stmt* stmt_1         = NULL;

    /*删除表dp中的测量点相关信息*/
    {
        sprintf(sql,"delete from dal_cbt_dp where dp = %04d;",pn);
        printf("sql:%s\n",sql);
        if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK){
            printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
            ret = APP_HANDLE_FAILED;
            goto Finish; 
        }

        rc = sqlite3_step(stmt);   
        if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE)){
            printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
        }

        if(stmt != NULL)
            sqlite3_finalize(stmt);
    }

    /*从表dal_cbt_group删除groupInfo中的Pn*/
    {
        sql_1 = "select groupInfo, id from dal_cbt_group";
        printf("sql:%s\n",sql_1);
        if(sqlite3_prepare_v2(DCS003_db, sql_1, strlen(sql_1), &stmt_1, NULL) != SQLITE_OK){
            printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
            ret = APP_HANDLE_FAILED;
            goto Finish; 
        }

        while(rc = sqlite3_step(stmt_1) == SQLITE_ROW) 
        {  
            groupInfo = sqlite3_column_text(stmt_1, 0);
            if(groupInfo == NULL)
            {
                printf("groupInfo NULL\n");
                continue;
            }
            groupid = sqlite3_column_text(stmt_1, 1);
            if(groupid == NULL)
            {
                printf("groupid NULL\n");
                continue;
            }

            if(app_groupInfo_pn_delete(pn, groupInfo) == 1)
            /*group表有内容要更新*/
            {
                sprintf(sql, "update dal_cbt_group set groupInfo = \"%s\" where id = \"%s\";",groupInfo, groupid);
                printf("sql:%s\n",sql);
                if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
                {
                    printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
                    ret = APP_HANDLE_FAILED;
                    goto Finish; 
                }
                rc = sqlite3_step(stmt);
                if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE)){
                    printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
                }

                if(stmt != NULL)
                    sqlite3_finalize(stmt);
            }
        }
        if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE) && (rc!= SQLITE_OK)){
            printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
        }

        if(stmt_1 != NULL)
            sqlite3_finalize(stmt_1);   
    }

    Finish:
    // if(stmt != NULL)
    //     sqlite3_finalize(stmt);      
    // if(stmt_1 != NULL)
    //     sqlite3_finalize(stmt_1);      
    return ret;
}

/*删除组*/
static int afn_04_f13_delete_group(char* group)
{
    int           ret            = APP_HANDLE_SUCCESS;
    int           rc             = 0;
    char          sql[300]       = {0};
    sqlite3_stmt* stmt           = NULL;

    /*删除表group中的组信息*/
    
    sprintf(sql,"delete from dal_cbt_group where id = \"%s\";",group);
    printf("sql:%s\n",sql);
    if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK){
        printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
        ret = APP_HANDLE_FAILED;
        goto Finish; 
    }

    rc = sqlite3_step(stmt);   
    if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE) && (rc!= SQLITE_OK)){
        printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
    }

    if(stmt != NULL)
        sqlite3_finalize(stmt);
    
    Finish:
     
    return ret;
}


/*删除组内测量点*/
static int afn_04_f13_delete_group_pn(UINT16 usPn, char* group)
{
    int           ret            = APP_HANDLE_SUCCESS;
    int           rc             = 0;
    char*         sql            = NULL;
    char*         groupInfo      = NULL;
    sqlite3_stmt* stmt           = NULL;
    sqlite3_stmt* stmt_1         = NULL;

    /*删除表group中的组信息*/
    
    sql = "select groupInfo from dal_cbt_group where group = ?;";
    printf("sql:%s\n",sql);
    if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK){
        printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
        ret = APP_HANDLE_FAILED;
        goto Finish; 
    }

    sqlite3_bind_text(stmt, 1, group, -1, NULL);
    rc = sqlite3_step(stmt);   
    if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE) && (rc!= SQLITE_OK)){
        printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
    }

    groupInfo = sqlite3_column_text(stmt,0);
    if(groupInfo == NULL)
    {
        printf("groupInfo NULL\n");
        ret = APP_HANDLE_FAILED;
        goto Finish;
    }

    app_groupInfo_pn_delete(usPn, groupInfo);
    {
        sql = "insert or replace into dal_cbt_group(id,groupInfo)values(?,?)";
        printf("sql:%s\n",sql);
        if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt_1, NULL) != SQLITE_OK){
            printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
            ret = APP_HANDLE_FAILED;
            goto Finish; 
        }
        /*id*/
        sqlite3_bind_text(stmt_1, 1, group, -1, NULL);
        /*groupInfo*/
        sqlite3_bind_text(stmt_1, 2, groupInfo, -1, NULL);

        rc = sqlite3_step(stmt_1);   
        if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE) && (rc != SQLITE_OK)){
            printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
        }

        if(stmt_1 != NULL)
            sqlite3_finalize(stmt_1);
    }
    if(stmt != NULL)
        sqlite3_finalize(stmt);
    
    Finish:
     
    return ret;
}

/*删除日控时段的配置*/
static int afn_11_f2_delete_config(char* startdate, int type)
{
    printf("afn_11_f2_delete_config\n");

    int rc             = 0;
    int ret            = APP_HANDLE_SUCCESS;
    char* sql          = NULL;
    sqlite3_stmt* stmt = NULL;
    /*删除任务*/
    {
        //sql = "delete from dal_cbt_clock where tasktype = ? and startdate = ?;";
        sql = "delete from dal_cbt_clock;";
        printf("sql:%s\n",sql);

        if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
        {
            printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
            ret = APP_HANDLE_FAILED;
            goto Finish; 
        }

        //sqlite3_bind_int(stmt, 1, type - TASK_TYPE_DELETE_MASK);
        //sqlite3_bind_text(stmt, 2, startdate, -1, NULL);

        rc = sqlite3_step(stmt);
        if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE) && (rc != SQLITE_OK))
        {
            printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
            ret = APP_HANDLE_FAILED;
        }
    }

    Finish:
    if(stmt != NULL)
        sqlite3_finalize(stmt);
    
    return ret;
}

/*04f3配置组信息处理*/
static int afn_04f3_group_config_handle(UINT8* d, char* group)
{
    printf("afn_04f3_group_config_handle\n");

    char*          sql     = NULL; 
    UINT16         pn[512] = {0};
    UINT16         num     = 0;
    int            ret     = APP_HANDLE_SUCCESS;
    int            rc      = 0;
    int            i       = 0;
    int            j       = 0;
    sqlite3_stmt*  stmt    = NULL;
    
    /*转换Pn*/
    {
        /*终端通道pn*/
        for(i = 0; i < 64; i++)
        {
            //判断该字节是否有有效pn
            if(d[i] == 0x00)
                continue;
            printf("d[i]:%x\n",d[i]);
            for(j = 0; j < 8; j++)
            {
                /*有效pn*/
                if((d[i] >> j) & 0x01)
                {
                    if(i < 62)
                    {
                        /*存入终端有效测量点号*/
                        if(((UINT16)((i * 8 + j)) + 1) != 496){  
                            pn[num + 1] = (UINT16)((i * 8 + j)) + 1;
                        }
                        else
                        {
                            pn[num + 1] = 2032;   
                        }

                        printf("终端有效pn:%03d\n",pn[num+1]);
                        num++;
                    }
                    else
                    {
                        /*存入集控器有效测量点号*/
                        pn[num + 1] = (UINT16)(((i - 62) * 8 + j) + 2033);
                        printf("集控器有效pn:%03d\n",pn[num+1]);   
                        num++;
                    }
                }
            }
        }
    }
    /*存入pn数量*/
    {
        pn[0] = num;
    }

    {
        sql = "insert or replace into dal_cbt_group(id,groupInfo)values(?,?)";
        printf("sql:%s\n",sql);
        if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK){
            printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
            ret = APP_HANDLE_FAILED;
            goto Finish; 
        }
        /*id*/
        sqlite3_bind_text(stmt, 1, group, -1, NULL);
        /*groupInfo*/
        sqlite3_bind_text(stmt, 2, (char*)pn, num*2 + 2, NULL);

        rc = sqlite3_step(stmt);   
        if((rc != SQLITE_ROW) && (rc!= SQLITE_DONE) && (rc != SQLITE_OK)){
            printf("step() return %s, number:%03d\n", "SQLITE_ERROR",rc);
        }

        if(stmt != NULL)
            sqlite3_finalize(stmt);
    }

    Finish:
    
    return ret;
}




