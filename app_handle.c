#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "con3761adp.h"
#include "qgdw_3761_api.h"

#define PACK_MEM_SIZE  10240
#define OUT_BUF_LEN    10240

extern UINT8 g_ucPackMem[PACK_MEM_SIZE]; // 测试封装命令的参数的内存空间
extern UINT8 g_ucOutBuf[OUT_BUF_LEN];
extern void socket_client_send(UINT8* data, UINT16* len);


/*****************************************************************************
 函 数 名  : pack_0001
 功能描述  : 全部确认
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年4月22日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr pack_afn00f01(UINT8 afn)
{
   eCmErr eRet;
   UINT16 usBuflen = 0;
   sCmPacket *pscmPacket = (sCmPacket*)g_ucPackMem;


     /* 2 \u73af\u5883\u521d\u59cb\u5316 */
    sCmInit  sInit;
    //sInit.eRole = MT_ROLE_MASTER;
    sInit.eRole = MT_ROLE_CONTOR;
    sInit.ucPermitDelayMinutes = 255;
    eRet = ecm_3761_init(&sInit);
    if(eRet != MT_OK)
    {
       // printf("\u521d\u59cb\u5316\u5931\u8d25\n");
        return eRet;
    }
    char *str = "1234567890";
    bCmSetPw(str);


     /* 3 \u5c01\u88c5\u53c2\u6570 */
    memcpy(pscmPacket->sAddress.acRegionCode, "1100", 4);
    pscmPacket->sAddress.usTAddress = 1;
    pscmPacket->sAddress.bTeamAddr  = FALSE;
    pscmPacket->sAddress.ucMAddress = 1;
    pscmPacket->bReSend = FALSE;

    pscmPacket->ucCmdNum = 1;
    pscmPacket->sCmdData[0].eCmd  = CMD_AFN_0_F1_ALL_OK;
    pscmPacket->sCmdData[0].bApp  = TRUE;
    pscmPacket->sCmdData[0].usPN  = 0;

    pscmPacket->sCmdData[0].uAppData.sSure.eAFN = (eMtAFN)afn;

    eRet = ecm_3761_pack(pscmPacket, (UINT8*)g_ucOutBuf, &usBuflen);
    if(eRet != MT_OK)
    {
        printf("ecm_3761_pack error %d\n", eRet);
        return eRet;
    }

    /* 5 \u8f93\u51fa\u7ed3\u679c */
    printf_buffer_color((char*)g_ucOutBuf, usBuflen);
    socket_client_send((UINT8*)(g_ucOutBuf), usBuflen);

    return MT_OK;
}


/*****************************************************************************
 函 数 名  : pack_afn02f1
 功能描述  : 测试封装登录命令
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr pack_afn02f01(void)
{
   /* 1 定义变量 */ 
   eCmErr eRet;
   UINT16 usBuflen = 0;
   sCmPacket *pscmPacket = (sCmPacket*)g_ucPackMem;
   

    /* 2 环境初始化 */
    sCmInit  sInit;
    sInit.eRole = MT_ROLE_CONTOR;
    sInit.ucPermitDelayMinutes = 255;
    eRet = ecm_3761_init(&sInit);
    if(eRet != MT_OK)
    {
        printf("初始化失败\n");
        return eRet;
    }
    char *str = "1234567890";
    bCmSetPw(str);
  
    
    /* 3 封装参数 */
    memcpy(pscmPacket->sAddress.acRegionCode, "1100", 4);
    pscmPacket->sAddress.usTAddress = 1;
    pscmPacket->sAddress.bTeamAddr  = FALSE;
    pscmPacket->sAddress.ucMAddress = 1;
    //pscmPacket->bReSend = FALSE;
    /*获取重发标志*/
    pscmPacket->bReSend = timeout_flag_get(CMD_AFN_2_F1_LOG_IN);

    pscmPacket->ucCmdNum = 1;
    pscmPacket->sCmdData[0].eCmd  = CMD_LOG_IN;
    pscmPacket->sCmdData[0].bApp  = FALSE;
    pscmPacket->sCmdData[0].usPN  = 0;
    
    /* 4 调用函数 */
    eRet = ecm_3761_pack(pscmPacket, (UINT8*)g_ucOutBuf, &usBuflen);
    if(eRet != MT_OK)
    {
        printf("ecm_3761_pack error %d\n", eRet);
        return eRet;
    }
    
    /* 5 输出结果 */ 
    printf_buffer_color((char*)g_ucOutBuf, usBuflen);
    socket_client_send((UINT8*)(g_ucOutBuf), usBuflen);
    /*注册重发监测*/
    app_timeout_register(CMD_AFN_2_F1_LOG_IN);
    return MT_OK;

}

/*****************************************************************************
 函 数 名  : test_pack_afn02f1
 功能描述  : 登出命令
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr pack_afn02f2(void)
{
   /* 1 定义变量 */
   eCmErr eRet;
   UINT16 usBuflen = 0;
   sCmPacket *pscmPacket = (sCmPacket*)g_ucPackMem;


    /* 2 环境初始化 */
    sCmInit  sInit;
    //sInit.eRole = MT_ROLE_MASTER;
    sInit.eRole = MT_ROLE_CONTOR;
    sInit.ucPermitDelayMinutes = 255;
    eRet = ecm_3761_init(&sInit);
    if(eRet != MT_OK)
    {
        printf("初始化失败\n");
        return eRet;
    }
    char *str = "1234567890";
    bCmSetPw(str);


    /* 3 封装参数 */
    memcpy(pscmPacket->sAddress.acRegionCode, "1100", 4);
    pscmPacket->sAddress.usTAddress = 1;
    pscmPacket->sAddress.bTeamAddr  = FALSE;
    pscmPacket->sAddress.ucMAddress = 1;
    pscmPacket->bReSend = timeout_flag_get(CMD_AFN_2_F2_LOG_OUT);

    pscmPacket->ucCmdNum = 1;
    pscmPacket->sCmdData[0].eCmd  = CMD_LOG_OUT;
    pscmPacket->sCmdData[0].bApp  = FALSE;
    pscmPacket->sCmdData[0].usPN  = 0;

    /* 4 调用函数 */
    eRet = ecm_3761_pack(pscmPacket, (UINT8*)g_ucOutBuf, &usBuflen);
    if(eRet != MT_OK)
    {
        printf("ecm_3761_pack error %d\n", eRet);
        return eRet;
    }

    /* 5 输出结果 */
    printf_buffer_color((char*)g_ucOutBuf, usBuflen);
    socket_client_send((UINT8*)(g_ucOutBuf), usBuflen);
    /*注册重发监测*/
    app_timeout_register(CMD_AFN_2_F2_LOG_OUT);
    return MT_OK;

}


/*****************************************************************************
 函 数 名  : pack_afn0cf01_s2m_analog
 功能描述  : 查询功率
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr pack_afn0cf01_s2m_analog(void)
{
    /* 1 定义变量 */ 
   eCmErr eRet;
   UINT16 usBuflen = 0;
   sCmPacket *pscmPacket = (sCmPacket*)g_ucPackMem;
   

    /* 2 环境初始化 */
    sCmInit  sInit;
    //sInit.eRole = MT_ROLE_MASTER;
    sInit.eRole = MT_ROLE_CONTOR;
    sInit.ucPermitDelayMinutes = 255;
    eRet = ecm_3761_init(&sInit);
    if(eRet != MT_OK)
    {
        printf("初始化失败\n");
        return eRet;
    }
    char *str = "1234567890";
    bCmSetPw(str);
  
    
    /* 3 封装参数 */
    memcpy(pscmPacket->sAddress.acRegionCode, "1100", 4);
    pscmPacket->sAddress.usTAddress = 1;
    pscmPacket->sAddress.bTeamAddr  = FALSE;
    pscmPacket->sAddress.ucMAddress = 1;
    pscmPacket->bReSend = FALSE;//timeout_flag_get(CMD_AFN_C_F1_ANALOG_DATA);
    pscmPacket->bActive = FALSE;

    pscmPacket->ucCmdNum = 1;
    pscmPacket->sCmdData[0].eCmd  = CMD_ANALOG_DATA;
    pscmPacket->sCmdData[0].bApp  = TRUE;
    pscmPacket->sCmdData[0].usPN  = 12;

    // app
    /*抄读时间*/
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.sReadTime.ucYY = 18;
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.sReadTime.ucMM = 3;
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.sReadTime.ucDD = 21;
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.sReadTime.ucHH = 16;
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.sReadTime.ucmm = 12;
    /*模拟量路数*/
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.anaglogNum = 2;
    /*1路当前电压*/
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.data[0].fU = 123.4;
    /*1路当前电流*/
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.data[0].fI = -123.456;
    /*1路当前有功功率*/
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.data[0].fP = -12.345;
    /*1路当前无功功率*/
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.data[0].fQ = -12.345;
    /*1路当前功率因数*/
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.data[0].fPf = -123.4;
    /*1路当前光控值*/
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.data[0].fLc = -123.4;

    /*1路当前电压*/
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.data[1].fU = 123.4;
    /*1路当前电流*/
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.data[1].fI = 123.456;
    /*1路当前有功功率*/
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.data[1].fP = 12.345;
    /*1路当前无功功率*/
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.data[1].fQ = 12.345;
    /*1路当前功率因数*/
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.data[1].fPf = 123.4;
    /*1路当前光控值*/
    pscmPacket->sCmdData[0].uAppData.sTmAnalog.data[1].fLc = 123.4;

    
    /* 4 调用函数 */
    eRet = ecm_3761_pack(pscmPacket, (UINT8*)g_ucOutBuf, &usBuflen);
    if(eRet != MT_OK)
    {
        printf("ecm_3761_pack error %d\n", eRet);
        return eRet;
    }
    
    /* 5 输出结果 */ 
    printf_buffer_color((char*)g_ucOutBuf, usBuflen);
    socket_client_send((UINT8*)(g_ucOutBuf), usBuflen);
    /*注册重发监测*/
    //app_timeout_register(CMD_AFN_C_F1_ANALOG_DATA);

    return MT_OK;
}


/*****************************************************************************
 函 数 名  : pack_afn0cf02_s2m_analog
 功能描述  : 查询时钟
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/

eMtErr pack_afn0cf02_s2m_auto(void)
{
   /* 1 定义变量 */ 
   eCmErr eRet;
   UINT16 usBuflen = 0;
   sCmPacket *pscmPacket = (sCmPacket*)g_ucPackMem;
   

    /* 2 环境初始化 */
    sCmInit  sInit;
    //sInit.eRole = MT_ROLE_MASTER;
    sInit.eRole = MT_ROLE_CONTOR;
    sInit.ucPermitDelayMinutes = 255;
    eRet = ecm_3761_init(&sInit);
    if(eRet != MT_OK)
    {
        printf("初始化失败\n");
        return eRet;
    }
    char *str = "1234567890";
    bCmSetPw(str);
  
    
    /* 3 封装参数 */
    memcpy(pscmPacket->sAddress.acRegionCode, "1100", 4);
    pscmPacket->sAddress.usTAddress = 1;
    pscmPacket->sAddress.bTeamAddr  = FALSE;
    pscmPacket->sAddress.ucMAddress = 1;
    pscmPacket->bReSend = FALSE;//timeout_flag_get(CMD_AFN_C_F2_TML_CLOCK);
    pscmPacket->bActive = FALSE;

    pscmPacket->ucCmdNum = 1;
    pscmPacket->sCmdData[0].eCmd  = CMD_TML_CLOCK;
    pscmPacket->sCmdData[0].bApp  = TRUE;
    pscmPacket->sCmdData[0].usPN  = 0;

    // app
    pscmPacket->sCmdData[0].uAppData.sTmlClock.ucYear = 18;
    pscmPacket->sCmdData[0].uAppData.sTmlClock.ucMonth = 3;
    //pscmPacket->sCmdData[0].uAppData.sTmlClock.ucWeek = 3;
    pscmPacket->sCmdData[0].uAppData.sTmlClock.ucDay = 21;
    pscmPacket->sCmdData[0].uAppData.sTmlClock.ucHour = 13;
    pscmPacket->sCmdData[0].uAppData.sTmlClock.ucMinute = 13;
    pscmPacket->sCmdData[0].uAppData.sTmlClock.ucSecond = 13;
   
    
    /* 4 调用函数 */
    eRet = ecm_3761_pack(pscmPacket, (UINT8*)g_ucOutBuf, &usBuflen);
    if(eRet != MT_OK)
    {
        printf("ecm_3761_pack error %d\n", eRet);
        return eRet;
    }
    
    /* 5 输出结果 */ 
    printf_buffer_color((char*)g_ucOutBuf, usBuflen);
    socket_client_send((UINT8*)(g_ucOutBuf), usBuflen);
    /*注册重发监测*/
    //app_timeout_register(CMD_AFN_C_F2_TML_CLOCK);

    return MT_OK;
}


/*****************************************************************************
 函 数 名  : pack_afn02f3
 功能描述  : 封装心跳命令
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr pack_afn02f3(void)
{
   /* 1 定义变量 */
   eCmErr eRet;
   UINT16 usBuflen = 0;
   sCmPacket *pscmPacket = (sCmPacket*)g_ucPackMem;


    /* 2 环境初始化 */
    sCmInit  sInit;
    sInit.eRole = MT_ROLE_CONTOR;
    sInit.ucPermitDelayMinutes = 255;
    eRet = ecm_3761_init(&sInit);
    if(eRet != MT_OK)
    {
        printf("初始化失败\n");
        return eRet;
    }
    char *str = "1234567890";
    bCmSetPw(str);


    /* 3 封装参数 */
    memcpy(pscmPacket->sAddress.acRegionCode, "1100", 4);
    pscmPacket->sAddress.usTAddress = 1;
    pscmPacket->sAddress.bTeamAddr  = FALSE;
    pscmPacket->sAddress.ucMAddress = 1;
    pscmPacket->bReSend = timeout_flag_get(CMD_AFN_2_F3_HEART_BEAT);

    pscmPacket->ucCmdNum = 1;
    pscmPacket->sCmdData[0].eCmd  = CMD_HEART_BEAT;
    pscmPacket->sCmdData[0].bApp  = FALSE;
    pscmPacket->sCmdData[0].usPN  = 0;

    /* 4 调用函数 */
    eRet = ecm_3761_pack(pscmPacket, (UINT8*)g_ucOutBuf, &usBuflen);
    if(eRet != MT_OK)
    {
        printf("ecm_3761_pack error %d\n", eRet);
        return eRet;
    }

    /* 5 输出结果 */
    printf_buffer_color((char*)g_ucOutBuf, usBuflen);
    socket_client_send((UINT8*)(g_ucOutBuf), usBuflen);
    /*注册重发监测*/
    app_timeout_register(CMD_AFN_2_F3_HEART_BEAT);
    return MT_OK;

}


/*****************************************************************************
 函 数 名  : test_pack_afn0af1_s2m
 功能描述  : 查询终端上行参数设置
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr pack_afn0af1_s2m(void)
{
   /* 1 定义变量 */
   eCmErr eRet;
   UINT16 usBuflen = 0;
   sCmPacket *pscmPacket = (sCmPacket*)g_ucPackMem;


    /* 2 环境初始化 */
    sCmInit  sInit;
    //sInit.eRole = MT_ROLE_MASTER;
    sInit.eRole = MT_ROLE_CONTOR;
    sInit.ucPermitDelayMinutes = 255;
    eRet = ecm_3761_init(&sInit);
    if(eRet != MT_OK)
    {
        printf("初始化失败\n");
        return eRet;
    }
    char *str = "1234567890ABCDEF";
    bCmSetPw(str);


    /* 3 封装参数 */
    memcpy(pscmPacket->sAddress.acRegionCode, "1100", 4);
    pscmPacket->sAddress.usTAddress = 1;
    pscmPacket->sAddress.bTeamAddr  = FALSE;
    pscmPacket->sAddress.ucMAddress = 1;
    /*获取重发标志*/
    pscmPacket->bReSend = FALSE;//timeout_flag_get(CMD_AFN_A_F1_TML_UP_CFG);

    pscmPacket->ucCmdNum = 1;
    pscmPacket->sCmdData[0].eCmd  = CMD_CFG_TML_UP_Q;
    pscmPacket->sCmdData[0].bApp  = TRUE;
    pscmPacket->sCmdData[0].usPN  = 0;

    // app
    pscmPacket->sCmdData[0].uAppData.sTmlUpCfg.bAutoReportAsk_1 = TRUE;
    pscmPacket->sCmdData[0].uAppData.sTmlUpCfg.bAutoReportAsk_2 = TRUE;
    pscmPacket->sCmdData[0].uAppData.sTmlUpCfg.bAutoReportAsk_3 = TRUE;
    pscmPacket->sCmdData[0].uAppData.sTmlUpCfg.ucRTS = 1;
    pscmPacket->sCmdData[0].uAppData.sTmlUpCfg.ucRTM = 1;
    pscmPacket->sCmdData[0].uAppData.sTmlUpCfg.ucReSendTimes = 3;
    pscmPacket->sCmdData[0].uAppData.sTmlUpCfg.ucHeartBeat = 1;
    pscmPacket->sCmdData[0].uAppData.sTmlUpCfg.usTmWaitTimeoutS = 1;


    /* 4 调用函数 */
    eRet = ecm_3761_pack(pscmPacket, (UINT8*)g_ucOutBuf, &usBuflen);
    if(eRet != MT_OK)
    {
        printf("ecm_3761_pack error %d\n", eRet);
        return eRet;
    }

    /* 5 输出结果 */
    printf_buffer_color((char*)g_ucOutBuf, usBuflen);
    socket_client_send((UINT8*)(g_ucOutBuf), usBuflen);
    /*注册重发监测*/
    //app_timeout_register(CMD_AFN_A_F1_TML_UP_CFG);

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : pack_afn0af3_s2m
 功能描述  : 主站IP
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr pack_afn0af3_s2m(void)
{
   /* 1 定义变量 */
   eCmErr eRet;
   UINT16 usBuflen = 0;
   sCmPacket *pscmPacket = (sCmPacket*)g_ucPackMem;


    /* 2 环境初始化 */
    sCmInit  sInit;
    //sInit.eRole = MT_ROLE_MASTER;
    sInit.eRole = MT_ROLE_CONTOR;
    sInit.ucPermitDelayMinutes = 255;
    eRet = ecm_3761_init(&sInit);
    if(eRet != MT_OK)
    {
        printf("初始化失败\n");
        return eRet;
    }
    char *str = "1234567890ABCDEF";
    bCmSetPw(str);


    /* 3 封装参数 */
    memcpy(pscmPacket->sAddress.acRegionCode, "1100", 4);
    pscmPacket->sAddress.usTAddress = 1;
    pscmPacket->sAddress.bTeamAddr  = FALSE;
    pscmPacket->sAddress.ucMAddress = 1;
    pscmPacket->bReSend = FALSE;//timeout_flag_get(CMD_AFN_A_F3_MST_IP_PORT);

    pscmPacket->ucCmdNum = 1;
    pscmPacket->sCmdData[0].eCmd  = CMD_IP_QUEST_0AF3;
    pscmPacket->sCmdData[0].bApp  = TRUE;
    pscmPacket->sCmdData[0].usPN  = 0;

    // app
    pscmPacket->sCmdData[0].uAppData.sTmIpPort.ulMainVersion = 0x01;
    pscmPacket->sCmdData[0].uAppData.sTmIpPort.ulMainIP = 0x11223344;
    pscmPacket->sCmdData[0].uAppData.sTmIpPort.usMainPort = 0x0010;
    pscmPacket->sCmdData[0].uAppData.sTmIpPort.ulBackVersion = 0x01;
    pscmPacket->sCmdData[0].uAppData.sTmIpPort.ulBackIP = 0x11223344;
    pscmPacket->sCmdData[0].uAppData.sTmIpPort.usBackPort = 0x0010;
    memcpy(pscmPacket->sCmdData[0].uAppData.sTmIpPort.ucascAPN,"12345678912345678",16);
    memcpy(pscmPacket->sCmdData[0].uAppData.sTmIpPort.virUserName,"99999999998888888888777777777766",32);
    memcpy(pscmPacket->sCmdData[0].uAppData.sTmIpPort.virUserPw,"55555555554444444444333333333322",32);


    /* 4 调用函数 */
    eRet = ecm_3761_pack(pscmPacket, (UINT8*)g_ucOutBuf, &usBuflen);
    if(eRet != MT_OK)
    {
        printf("ecm_3761_pack error %d\n", eRet);
        return eRet;
    }

    /* 5 输出结果 */
    printf_buffer_color((char*)g_ucOutBuf, usBuflen);
    socket_client_send((UINT8*)(g_ucOutBuf), usBuflen);
    /*注册重发监测*/
    //app_timeout_register(CMD_AFN_A_F3_MST_IP_PORT);

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : pack_afn0af13_s2m
 功能描述  : 终端电能表配置参数
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr pack_afn0af13_s2m(void)
{
   /* 1 定义变量 */
   eCmErr eRet;
   UINT16 usBuflen = 0;
   sCmPacket *pscmPacket = (sCmPacket*)g_ucPackMem;


    /* 2 环境初始化 */
    sCmInit  sInit;
    //sInit.eRole = MT_ROLE_MASTER;
    sInit.eRole = MT_ROLE_CONTOR;
    sInit.ucPermitDelayMinutes = 255;
    eRet = ecm_3761_init(&sInit);
    if(eRet != MT_OK)
    {
        printf("初始化失败\n");
        return eRet;
    }
    char *str = "1234567890ABCDEF";
    bCmSetPw(str);


    /* 3 封装参数 */
    memcpy(pscmPacket->sAddress.acRegionCode, "1100", 4);
    pscmPacket->sAddress.usTAddress = 1;
    pscmPacket->sAddress.bTeamAddr  = FALSE;
    pscmPacket->sAddress.ucMAddress = 1;
    pscmPacket->bReSend = FALSE;//timeout_flag_get(CMD_AFN_A_F13_LED_UP_CFG);

    pscmPacket->ucCmdNum = 1;
    pscmPacket->sCmdData[0].eCmd  = CMD_LED_0AF13;
    pscmPacket->sCmdData[0].bApp  = TRUE;
    pscmPacket->sCmdData[0].usPN  = 0;

    // app

    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.Type = 0x02;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.Nature = 0x00;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.Flag = 0x00;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.Port = 48;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.PortType = 0x5f;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.address[0] = 0x11;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.address[1] = 0x22;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.address[2] = 0x33;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.address[3] = 0x44;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.address[4] = 0x55;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.address[5] = 0x66;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.Group = 0x11223344;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.Num = 0x11;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.PosLong.Flag = 0x11;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.PosLong.Degree = 0x11;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.PosLong.Min = 0x33;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.PosLong.Sec = 0x1122;

    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.PosLat.Flag = 0x11;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.PosLat.Degree = 0x11;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.PosLat.Min = 0x33;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.PosLat.Sec = 0x1122;


    /* 4 调用函数 */
    eRet = ecm_3761_pack(pscmPacket, (UINT8*)g_ucOutBuf, &usBuflen);
    if(eRet != MT_OK)
    {
        printf("ecm_3761_pack error %d\n", eRet);
        return eRet;
    }

    /* 5 输出结果 */
    printf_buffer_color((char*)g_ucOutBuf, usBuflen);
    socket_client_send((UINT8*)(g_ucOutBuf), usBuflen);
    /*注册重发监测*/
    //app_timeout_register(CMD_AFN_A_F13_LED_UP_CFG);

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : pack_afn0af15_s2m
 功能描述  : 继电器输出控制方案
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr pack_afn0af15_s2m(void)
{
   /* 1 定义变量 */
   eCmErr eRet;
   UINT16 usBuflen = 0;
   sCmPacket *pscmPacket = (sCmPacket*)g_ucPackMem;


    /* 2 环境初始化 */
    sCmInit  sInit;
    //sInit.eRole = MT_ROLE_MASTER;
    sInit.eRole = MT_ROLE_CONTOR;
    sInit.ucPermitDelayMinutes = 255;
    eRet = ecm_3761_init(&sInit);
    if(eRet != MT_OK)
    {
        printf("初始化失败\n");
        return eRet;
    }
    char *str = "1234567890ABCDEF";
    bCmSetPw(str);


    /* 3 封装参数 */
    memcpy(pscmPacket->sAddress.acRegionCode, "1100", 4);
    pscmPacket->sAddress.usTAddress = 1;
    pscmPacket->sAddress.bTeamAddr  = FALSE;
    pscmPacket->sAddress.ucMAddress = 1;
    pscmPacket->bReSend = FALSE;//timeout_flag_get(CMD_AFN_A_F15_HAVE_DIFF_EVENT);

    pscmPacket->ucCmdNum = 1;
    pscmPacket->sCmdData[0].eCmd  = CMD_RELAY_0AF15;
    pscmPacket->sCmdData[0].bApp  = TRUE;
    pscmPacket->sCmdData[0].usPN  = 0;

    // app
    pscmPacket->sCmdData[0].uAppData.stmCtrlWaycfg.BeginTime.ucYY = 13;
    pscmPacket->sCmdData[0].uAppData.stmCtrlWaycfg.BeginTime.ucMM = 8;
    pscmPacket->sCmdData[0].uAppData.stmCtrlWaycfg.BeginTime.ucDD = 23;
    pscmPacket->sCmdData[0].uAppData.stmCtrlWaycfg.LastTime = 13;
    pscmPacket->sCmdData[0].uAppData.stmCtrlWaycfg.WayFlag = 8;
    pscmPacket->sCmdData[0].uAppData.stmCtrlWaycfg.ucM= 2;
    pscmPacket->sCmdData[0].uAppData.stmCtrlWaycfg.sTime[0].sBegin.ucHH = 2;
    pscmPacket->sCmdData[0].uAppData.stmCtrlWaycfg.sTime[0].sBegin.ucmm = 3;
    pscmPacket->sCmdData[0].uAppData.stmCtrlWaycfg.sTime[0].sEnd.ucHH = 4;
    pscmPacket->sCmdData[0].uAppData.stmCtrlWaycfg.sTime[0].sEnd.ucmm = 5;
    pscmPacket->sCmdData[0].uAppData.stmCtrlWaycfg.sTime[1].sBegin.ucHH = 2;
    pscmPacket->sCmdData[0].uAppData.stmCtrlWaycfg.sTime[1].sBegin.ucmm = 3;
    pscmPacket->sCmdData[0].uAppData.stmCtrlWaycfg.sTime[1].sEnd.ucHH = 4;
    pscmPacket->sCmdData[0].uAppData.stmCtrlWaycfg.sTime[1].sEnd.ucmm = 5;


    /* 4 调用函数 */
    eRet = ecm_3761_pack(pscmPacket, (UINT8*)g_ucOutBuf, &usBuflen);
    if(eRet != MT_OK)
    {
        printf("ecm_3761_pack error %d\n", eRet);
        return eRet;
    }

    /* 5 输出结果 */
    printf_buffer_color((char*)g_ucOutBuf, usBuflen);
    socket_client_send((UINT8*)(g_ucOutBuf), usBuflen);
    /*注册重发监测*/
    //app_timeout_register(CMD_AFN_A_F15_HAVE_DIFF_EVENT);

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : pack_afn11hf4_s2m
 功能描述  : 查询对路灯控制器批量执行情况
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr pack_afn11hf4_s2m(void)
{
   eCmErr eRet;
   UINT16 usBuflen = 0;
   sCmPacket *pscmPacket = (sCmPacket*)g_ucPackMem;


     /* 2 \u73af\u5883\u521d\u59cb\u5316 */
    sCmInit  sInit;
    //sInit.eRole = MT_ROLE_MASTER;
    sInit.eRole = MT_ROLE_CONTOR;
    sInit.ucPermitDelayMinutes = 255;
    eRet = ecm_3761_init(&sInit);
    if(eRet != MT_OK)
    {
       // printf("\u521d\u59cb\u5316\u5931\u8d25\n");
        return eRet;
    }
    char *str = "1234567890";
    bCmSetPw(str);


     /* 3 \u5c01\u88c5\u53c2\u6570 */
    memcpy(pscmPacket->sAddress.acRegionCode, "1100", 4);
    pscmPacket->sAddress.usTAddress = 1;
    pscmPacket->sAddress.bTeamAddr  = FALSE;
    pscmPacket->sAddress.ucMAddress = 1;
    pscmPacket->bReSend = FALSE;//timeout_flag_get(CMD_AFN_11_F4);;

    pscmPacket->ucCmdNum = 1;
    pscmPacket->sCmdData[0].eCmd  = CMD_LED_PARA;
    pscmPacket->sCmdData[0].bApp  = TRUE;
    pscmPacket->sCmdData[0].usPN  = 0;
    pscmPacket->sCmdData[0].uAppData.std_11f4.ucDataUnit = 0x1111;
    pscmPacket->sCmdData[0].uAppData.std_11f4.ucTaskType= 0x22;
    pscmPacket->sCmdData[0].uAppData.std_11f4.ucCtrlNum= 0x3333;
    pscmPacket->sCmdData[0].uAppData.std_11f4.ucExeNum= 0x4444;
    memcpy(pscmPacket->sCmdData[0].uAppData.std_11f4.addr[0].address,"99999999",8);

   // pscmdPacket->sCmdData[0].uAppData.std_11f4.


     /* 4 \u8c03\u7528\u51fd\u6570 */
    eRet = ecm_3761_pack(pscmPacket, (UINT8*)g_ucOutBuf, &usBuflen);
    if(eRet != MT_OK)
    {
        printf("ecm_3761_pack error %d\n", eRet);
        return eRet;
    }

    /* 5 \u8f93\u51fa\u7ed3\u679c */
    printf_buffer_color((char*)g_ucOutBuf, usBuflen);
    socket_client_send((UINT8*)(g_ucOutBuf), usBuflen);
    /*注册重发监测*/
    //app_timeout_register(CMD_AFN_11_F4);

    return MT_OK;
}

void app_req_handle(sMtData* pSub)
{
    printf("app_req_handle, pSub->eCmd:%02X",pSub->eCmd);
    switch(pSub->eCmd)
    {
        case CMD_AFN_C_F1_ANALOG_DATA:
            pack_afn0cf01_s2m_analog();
        break;
        case CMD_AFN_C_F2_TML_CLOCK:
            pack_afn0cf02_s2m_auto();
        break;
        case CMD_AFN_A_F1_TML_UP_CFG:
            pack_afn0af1_s2m();
        break;
        case CMD_AFN_A_F3_MST_IP_PORT:
            pack_afn0af3_s2m();
        break;
        case CMD_AFN_A_F13_LED_UP_CFG:
            pack_afn0af13_s2m();
        break;
        case CMD_AFN_A_F15_HAVE_DIFF_EVENT:
            pack_afn0af15_s2m();
        break;
        case CMD_AFN_11_F4:
            pack_afn11hf4_s2m();
        break;
        
    }
}

void app_test_step(int step, void* data)
{
	switch(step)
	{
		case 1:
			pack_afn02f01();
		break;

	}
}



