#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sqlite3.h"
#include "app_handle.h"
#include "app_sql.h"

extern sqlite3 *DCS003_db;


/*时段控制检查*/
int afn_11_f2_ctrl_period(void)
{
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
    sqlite3_stmt* stmt = NULL;

    // 获取系统当前时间
    {
        time_t now;
        struct tm* timenow;
        time(&now);
        timenow = localtime(&now);
        
        printf("年:%03d 月:%03d 日:%03d 周:%03d 时:%03d 分:%03d\n",timenow->tm_year, \
            timenow->tm_mon,timenow->tm_mday,timenow->tm_wday,timenow->tm_hour,timenow->tm_min);

        /*检查的最小时间刻度为Min*/
        if(preTimeMin != timenow->tm_min)
        {
            preTimeMin = timenow->tm_min; 

            currTime[0] = (timenow->tm_year - 100) + '0';
            currTime[1] = (timenow->tm_mon + 1) + '0';
            currTime[2] = timenow->tm_mday + '0';
            currTime[3] = timenow->tm_wday + '0';
            currTime[4] = timenow->tm_hour + '0';
            currTime[5] = timenow->tm_min + '0';
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
        }

        /*特定任务月*/
        {
            
            sprintf(sql,"select groupid,state from dal_cbt_clock where tasktype = %d and startdate = \"0%s\";"\
                ,TASK_TYPE_MON, &currTime[1]);
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
        }

        /*特定任务日*/
        {
            sprintf(sql,"select groupid,state from dal_cbt_clock where tasktype = %d and startdate = \"00%s\";"\
                ,TASK_TYPE_DAY, &currTime[2]);
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
        }

        /*特定任务周*/
        {
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
                if((taskExecTime[3] - '0') & ((1 << (currTime[3] - '0'))&0xff))
                {
                    if(strncmp(&taskExecTime[4], &currTime[4],2) == 0)
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
        }

        /*特定任务时*/
        {
            sprintf(sql,"select groupid,state from dal_cbt_clock where tasktype = %d and startdate = \"0000%s\";"\
                ,TASK_TYPE_HOUR, &currTime[4]);
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
        }

        /*普通任务年*/
        {
            
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
        }

        /*普通任务月*/
        {
            
            sprintf(sql,"select groupid,state from dal_cbt_clock where tasktype = %d and startdate = \"0%s\";"\
                ,TASK_TYPE_MON & TASK_TYPE_COMMON_MASK, &currTime[1]);
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
        }

        /*普通任务日*/
        {
            sprintf(sql,"select groupid,state from dal_cbt_clock where tasktype = %d and startdate = \"00%s\";"\
                ,TASK_TYPE_DAY & TASK_TYPE_COMMON_MASK, &currTime[2]);
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
        }

        /*普通任务周*/
        {
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
                printf("taskExecTime:%s\n",taskExecTime);
                /*判断是否属于该周*/
                if((taskExecTime[3] - '0') & ((1 << (currTime[3] - '0'))&0xff))
                {
                    if(strncmp(&taskExecTime[4], &currTime[4],2) == 0)
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
        }

        /*普通任务时*/
        {
            sprintf(sql,"select groupid,state from dal_cbt_clock where tasktype = %d and startdate = \"0000%s\";"\
                ,TASK_TYPE_HOUR & TASK_TYPE_COMMON_MASK, &currTime[4]);
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
        }

    }

    Finish:
    if(stmt != NULL)
        sqlite3_finalize(stmt);
     app_sql_close();

    return ret;
}

/*AFN:0x11*******************************************************************************/
int afn_11_f3_group_ctrl(UINT8* groupid, UINT8 state)
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

    return ret;
}

/*将pn加入到groupInfo中*/
void app_groupInfo_pack(UINT16 d, char* groupInfo)
{
    int i, num;

    UINT16 pn[255] = {0};

    num = groupInfo[0];
    if(0 == num)
    {
        groupInfo[0] = 1 + '0';
        groupInfo[1] = '0';
        d = d + '0';
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
            if(*(UINT16*)&groupInfo[i*2 + 2] == (d+'0'))
            {
                printf("Pn重复：%03d\n",d+'0');
                return;
            }
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

/*从groupInfo中取出pn*/
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

/*删除groupInfo中的Pn*/
int app_groupInfo_pn_delete(UINT16 d, char* groupInfo)
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
    }else{
        num -= '0';
    }

    printf("groupInfo长度:%d,排序:%s\n", num, groupInfo);
    /*取出已有的Pn*/
    {
        printf("pn:");
        for(i = 0,j=0; i < num ; i++)
        {
            if(*(UINT16*)&groupInfo[i*2 + 2] != (d+'0'))
            {
                pn[j] = *(UINT16*)&groupInfo[i*2 + 2];
                j++;
                printf(" %03d,",pn[i]-'0');
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
        groupInfo[0] = (num & 0xff)+'0';
    }

    /*组装排序完成的groupInfo*/
    {
        for(i = 0; i < num; i++)
        {
            memcpy(&groupInfo[i*2 + 2], (UINT8*)&pn[i], 2);
            printf("groupInfoNew:%02X\n",groupInfo[i*2 + 2]-'0');       
        }
    }

    return deleteFlag;

}

/*删除测量点号*/
int afn_04_f13_delete_pn(UINT16 pn)
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
int afn_04_f13_delete_group(char* group)
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




/*删除日控时段的配置*/
int afn_11_f2_delete_config(char* startdate, int type)
{
    printf("afn_11_f2_delete_config\n");

    int rc             = 0;
    int ret            = APP_HANDLE_SUCCESS;
    char sql[300]      = {0};
    sqlite3_stmt* stmt = NULL;
    /*删除任务*/
    {
        sprintf(sql,"delete from dal_cbt_clock where tasktype = %d and startdate = \"%s\";"\
            ,type, startdate);//,type - TASK_TYPE_DELETE_MASK, startdate);
        printf("sql:%s\n",sql);

        if(sqlite3_prepare_v2(DCS003_db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK)
        {
            printf( "sqlite3_prepare_v2:error %s\n", sqlite3_errmsg(DCS003_db));   
            ret = APP_HANDLE_FAILED;
            goto Finish; 
        }

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

#if 1
void test_group(void)
{
    int i;
    // UINT16 data[10] = {101,102,103,105,104,109,110,107,106};
    // char data_1[10] = {0};

    // for(i = 0; i < 10; i++)
    //     app_groupInfo_pack(data[i],&data_1);

    // printf("delete...\n");
    // app_groupInfo_pn_delete(102,data_1);
    // printf("删除测量点\n");
     app_sql_open();    
    // afn_04_f13_delete_pn(110);
    
    char* group = "0006";
    afn_04_f13_delete_group(group);
    app_sql_close();
}
#endif