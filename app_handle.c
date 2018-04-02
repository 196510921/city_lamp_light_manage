#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "con3761adp.h"
#include "qgdw_3761_api.h"

#define PACK_MEM_SIZE  10240
#define OUT_BUF_LEN    10240

// extern UINT8 g_ucPackMem[PACK_MEM_SIZE]; // 测试封装命令的参数的内存空间
// extern UINT8 g_ucOutBuf[OUT_BUF_LEN];
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
   UINT8 g_ucPackMem[PACK_MEM_SIZE];
   UINT8 g_ucOutBuf[OUT_BUF_LEN];
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
    pscmPacket->bActive = FALSE;

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
 功能描述  : 登录命令
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
   UINT8 g_ucPackMem[PACK_MEM_SIZE];
   UINT8 g_ucOutBuf[OUT_BUF_LEN];
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
 函 数 名  : test_pack_afn02f2
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
   UINT8 g_ucPackMem[PACK_MEM_SIZE];
   UINT8 g_ucOutBuf[OUT_BUF_LEN];
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
   UINT8 g_ucPackMem[PACK_MEM_SIZE];
   UINT8 g_ucOutBuf[OUT_BUF_LEN];
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

/*终端时间标志状态*/
eMtErr pack_afn0cf08_s2m()
{
     eCmErr eRet;
    UINT16 usBuflen = 0;
    UINT8 g_ucPackMem[PACK_MEM_SIZE];
    UINT8 g_ucOutBuf[OUT_BUF_LEN];
    smtPack *pscmPacket = (smtPack*)g_ucPackMem;
    INT32 i;
  
    /* 2 环境初始化 */
    sCmInit  sInit;
    //sInit.eRole = MT_ROLE_CONTOR;
    sInit.eRole = MT_ROLE_CONTOR;
    sInit.ucPermitDelayMinutes = 255;
    eRet = ecm_3761_init(&sInit);
    if(eRet != MT_OK)
    {
        printf("初始化失败\n");
        return eRet;
    }
    
    /* 3 封装参数 */
    memcpy(pscmPacket->sAddress.acRegionCode, "1100", 4);
    pscmPacket->sAddress.usTAddress = 1;
    pscmPacket->sAddress.bTeamAddr  = FALSE;
    pscmPacket->sAddress.ucMAddress = 1;

    pscmPacket->eAFN = AFN_0C_ASK1;
    pscmPacket->eDir = MT_DIR_S2M;
    pscmPacket->ePRM = MT_PRM_PASIVE;
    
    pscmPacket->ePos = MT_POS_SIGLE;
    pscmPacket->ucSeq = 0;
    pscmPacket->bAcdFcb = TRUE;


    pscmPacket->usDataNum = 1;
    pscmPacket->sData[0].eCmd  = CMD_AFN_C_F8_TML_EVNT_FLAG;
    pscmPacket->sData[0].bApp  = TRUE;
    pscmPacket->sData[0].usPN  = 20;

    for(i = 0;i < MT_AFN0CF08_ERC_MAX;i++)
    {
        pscmPacket->sData[0].uApp.sTmlErcSta.bErc[i] = FALSE;
    }
    pscmPacket->sData[0].uApp.sTmlErcSta.bErc[5]  = TRUE;
    pscmPacket->sData[0].uApp.sTmlErcSta.bErc[4] = TRUE;
    pscmPacket->sData[0].uApp.sTmlErcSta.bErc[3] = TRUE;
    pscmPacket->sData[0].uApp.sTmlErcSta.bErc[2] = TRUE;
    pscmPacket->sData[0].uApp.sTmlErcSta.bErc[1] = TRUE;
    pscmPacket->sData[0].uApp.sTmlErcSta.bErc[0]  = TRUE;
    
    pscmPacket->sData[0].uApp.sTmlErcSta.bErc[37]  = TRUE;
    pscmPacket->sData[0].uApp.sTmlErcSta.bErc[36] = TRUE;
    pscmPacket->sData[0].uApp.sTmlErcSta.bErc[35] = TRUE;
    pscmPacket->sData[0].uApp.sTmlErcSta.bErc[34] = TRUE;
    pscmPacket->sData[0].uApp.sTmlErcSta.bErc[33] = TRUE;
    pscmPacket->sData[0].uApp.sTmlErcSta.bErc[32]  = TRUE;

    /* 4 调用函数 */
    eRet = emtPack(pscmPacket, &usBuflen, (UINT8*)g_ucOutBuf);
    if(eRet != MT_OK)
    {
        printf("ecm_3761_pack error %d\n", eRet);
        return eRet;
    }
    
    /* 5 输出结果 */ 
    printf_buffer_color((char*)g_ucOutBuf, usBuflen);
    socket_client_send((UINT8*)(g_ucOutBuf), usBuflen);
    /*注册重发监测*/
    app_timeout_register(CMD_AFN_C_F8_TML_EVNT_FLAG);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : pack_afn09f1_s2m
 功能描述  : 终端版本信息
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
eMtErr pack_afn09f1_s2m()
{
   /* 1 定义变量 */ 
   eCmErr eRet;
   UINT16 usBuflen = 0;
   UINT8 g_ucPackMem[PACK_MEM_SIZE];
   UINT8 g_ucOutBuf[OUT_BUF_LEN];
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
    pscmPacket->bReSend = FALSE;

    pscmPacket->ucCmdNum = 1;
    pscmPacket->sCmdData[0].eCmd  = CMD_AFN_TML_VERSION;
    //pscmPacket->sCmdData[0].eCmd  = CMD_AFN_9_F1_TML_VERSION;
    pscmPacket->sCmdData[0].bApp  = TRUE;
    pscmPacket->sCmdData[0].usPN  = 0;

    // app
    memcpy(pscmPacket->sCmdData[0].uAppData.sTmlVersion.ucFactoryID, "fact", 4);
    memcpy(pscmPacket->sCmdData[0].uAppData.sTmlVersion.ucDeviceID, "devciced", 8);
    memcpy(pscmPacket->sCmdData[0].uAppData.sTmlVersion.ucSoftwareID, "soft_dalitek", 8);

    pscmPacket->sCmdData[0].uAppData.sTmlVersion.sDateSoftware.ucYY = 13;
    pscmPacket->sCmdData[0].uAppData.sTmlVersion.sDateSoftware.ucMM = 8;
    pscmPacket->sCmdData[0].uAppData.sTmlVersion.sDateSoftware.ucDD = 23;

    memcpy(pscmPacket->sCmdData[0].uAppData.sTmlVersion.ucInfoCodeCap, "123456789987654321", 16);
    //memcpy(pscmPacket->sCmdData[0].uAppData.sTmlVersion.ucProtolID, "prot", 4);
   
    memcpy(pscmPacket->sCmdData[0].uAppData.sTmlVersion.ucHardWareID, "hardware", 8);
    pscmPacket->sCmdData[0].uAppData.sTmlVersion.sDateHardware.ucYY = 13;
    pscmPacket->sCmdData[0].uAppData.sTmlVersion.sDateHardware.ucMM = 8;
    pscmPacket->sCmdData[0].uAppData.sTmlVersion.sDateHardware.ucDD = 23;

    memcpy(pscmPacket->sCmdData[0].uAppData.sTmlVersion.ucRemoteID, "hardware", 8);
    pscmPacket->sCmdData[0].uAppData.sTmlVersion.sRemote.ucYY = 13;
    pscmPacket->sCmdData[0].uAppData.sTmlVersion.sRemote.ucMM = 8;
    pscmPacket->sCmdData[0].uAppData.sTmlVersion.sRemote.ucDD = 23;

    memcpy(pscmPacket->sCmdData[0].uAppData.sTmlVersion.ucLocalID, "hardware", 8);
    pscmPacket->sCmdData[0].uAppData.sTmlVersion.sLocal.ucYY = 13;
    pscmPacket->sCmdData[0].uAppData.sTmlVersion.sLocal.ucMM = 8;
    pscmPacket->sCmdData[0].uAppData.sTmlVersion.sLocal.ucDD = 23;

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
   UINT8 g_ucPackMem[PACK_MEM_SIZE];
   UINT8 g_ucOutBuf[OUT_BUF_LEN];
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
    pscmPacket->bReSend = FALSE;

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
   UINT8 g_ucPackMem[PACK_MEM_SIZE];
   UINT8 g_ucOutBuf[OUT_BUF_LEN];
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
    pscmPacket->bReSend = FALSE;

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
   UINT8 g_ucPackMem[PACK_MEM_SIZE];
   UINT8 g_ucOutBuf[OUT_BUF_LEN];
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
    pscmPacket->bReSend = FALSE;

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
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.Group[0] = 0x11;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.Group[1] = 0x22;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.Group[2] = 0x33;
    pscmPacket->sCmdData[0].uAppData.sTmTerminalCfg.Group[3]= 0x44;
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
   UINT8 g_ucPackMem[PACK_MEM_SIZE];
   UINT8 g_ucOutBuf[OUT_BUF_LEN];
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
    pscmPacket->bReSend = FALSE;

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
eMtErr pack_afn0cf01_s2m_analog()
{
   /* 1 定义变量 */ 
   eCmErr eRet;
   int    i = 0;
   UINT16 usPN = 0;
   UINT16 usBuflen = 0;
   UINT8 g_ucPackMem[PACK_MEM_SIZE];
   UINT8 g_ucOutBuf[OUT_BUF_LEN];
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

    usPN = get_current_usPn();
    /* 3 封装参数 */
    memcpy(pscmPacket->sAddress.acRegionCode, "1100", 4);
    pscmPacket->sAddress.usTAddress = 1;
    pscmPacket->sAddress.bTeamAddr  = FALSE;
    pscmPacket->sAddress.ucMAddress = 1;
    pscmPacket->bReSend = FALSE;
    pscmPacket->bActive = FALSE;

    pscmPacket->ucCmdNum = 1;
    pscmPacket->sCmdData[0].eCmd  = CMD_ANALOG_DATA;
    pscmPacket->sCmdData[0].bApp  = TRUE;
    pscmPacket->sCmdData[0].usPN  = usPN;
    //pscmPacket->sCmdData[0].usPN  = 100;

    /*Pn/P0共有部分*/
    {
        /*抄读时间*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.sReadTime.ucYY = 18;
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.sReadTime.ucMM = 3;
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.sReadTime.ucDD = 21;
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.sReadTime.ucHH = 16;
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.sReadTime.ucmm = 12;
        /*模拟量路数*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.anaglogNum = 4;
        /*1路当前电压*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.fU[0] = 111.2;
        /*1路当前电流*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.fI[0] = 222.222;
        /*1路当前有功功率*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.fP[0] = 33.333;
        /*1路当前无功功率*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.fQ[0] = 44.444;
        /*1路当前功率因数*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.fPf[0] = 555.5;
        /*1路当前光控值*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.fLc[0] = 666.6;
        /*当前视在功率*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.fS[0] = 33.333;
        /*有功能量寄存器*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.EP[0] = 232.323;
                /*正向有功能量寄存器*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.PosEP[0] = 34.3;
                /*反向有功能量寄存器*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.NegEP[0] = 45.4;
                /*无功能量寄存器*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.EQ[0] = 565.6;
         /*正向无功能量寄存器*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.PosEQ[0] = 676.7;
        /*反向无功能量寄存器*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.NegEQ[0] = 676.6;
        /*视在能量寄存器*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.ES[0] = 585.8;
    
    }
    /*pn = 0部分*/
    if(usPN == 0){
          //开关灯状态
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.state = 0xff;
        /*累计开灯时间*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.Time = 444.4;
        /*漏电流*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.Lc[0] = 1234;
    }
    else
    /*测量点部分配置*/    
    {
        /*1路*/
        /*终端停上电/*/
        pscmPacket->sCmdData[0].uAppData.sTmAnalog.TPower[0] = 0xFF;
        
        for(i = 1; i < 4; i++){
            /*终端停上电*/
            pscmPacket->sCmdData[0].uAppData.sTmAnalog.TPower[i] = 0xFF;
            /*当前电压*/
            pscmPacket->sCmdData[0].uAppData.sTmAnalog.fU[i] = 111.1;
            /*2路当前电流*/
            pscmPacket->sCmdData[0].uAppData.sTmAnalog.fI[i] = 222.222;
            /*2路当前有功功率*/
            pscmPacket->sCmdData[0].uAppData.sTmAnalog.fP[i] = 33.333;
            /*2路当前无功功率*/
            pscmPacket->sCmdData[0].uAppData.sTmAnalog.fQ[i] = 44.444;
            /*2路当前功率因数*/
            pscmPacket->sCmdData[0].uAppData.sTmAnalog.fPf[i] = 555.5;
            /*2路当前光控值*/
            pscmPacket->sCmdData[0].uAppData.sTmAnalog.fLc[i] = 666.6;
            /*当前视在功率*/
            pscmPacket->sCmdData[0].uAppData.sTmAnalog.fS[i] = 33.333;
            /*有功能量寄存器*/
            pscmPacket->sCmdData[0].uAppData.sTmAnalog.EP[i] = 232.323;
            /*正向有功能量寄存器*/
            pscmPacket->sCmdData[0].uAppData.sTmAnalog.PosEP[i] = 34.3;
            /*反向有功能量寄存器*/
            pscmPacket->sCmdData[0].uAppData.sTmAnalog.NegEP[i] = 45.4;
            /*无功能量寄存器*/
            pscmPacket->sCmdData[0].uAppData.sTmAnalog.EQ[i] = 565.6;
            /*正向无功能量寄存器*/
            pscmPacket->sCmdData[0].uAppData.sTmAnalog.PosEQ[i] = 676.7;
            /*反向无功能量寄存器*/
            pscmPacket->sCmdData[0].uAppData.sTmAnalog.NegEQ[i] = 676.6;
            /*视在能量寄存器*/
            pscmPacket->sCmdData[0].uAppData.sTmAnalog.ES[i] = 585.8;
        }

    }

    /* 4 调用函数 */
    eRet = ecm_3761_pack(pscmPacket, (UINT8*)g_ucOutBuf, &usBuflen);
    //eRet = ecm_3761_pack(pscmPacket, (UINT8*)packdata->data, &packdata->length);
    if(eRet != MT_OK)
    {
        printf("ecm_3761_pack error %d\n", eRet);
        return eRet;
    }
    
    /* 5 输出结果 */ 
   printf_buffer_color((char*)g_ucOutBuf, usBuflen);
   socket_client_send((UINT8*)(g_ucOutBuf), usBuflen);

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
   UINT8 g_ucPackMem[PACK_MEM_SIZE];
   UINT8 g_ucOutBuf[OUT_BUF_LEN];
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
  
    //pscmPacket = &packdata->pscmPacket;

    
#if 1
    /* 3 封装参数 */
    memcpy(pscmPacket->sAddress.acRegionCode, "1100", 4);
    pscmPacket->sAddress.usTAddress = 1;
    pscmPacket->sAddress.bTeamAddr  = FALSE;
    pscmPacket->sAddress.ucMAddress = 1;
    pscmPacket->bReSend = FALSE;
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
   
#endif
    /* 4 调用函数 */
    eRet = ecm_3761_pack(pscmPacket, (UINT8*)g_ucOutBuf, &usBuflen);
    //eRet = ecm_3761_pack(pscmPacket, (UINT8*)packdata->data, &packdata->length);
    if(eRet != MT_OK)
    {
        printf("ecm_3761_pack error %d\n", eRet);
        return eRet;
    }
    
    /* 5 输出结果 */ 
    printf_buffer_color((char*)g_ucOutBuf, usBuflen);
    socket_client_send((UINT8*)(g_ucOutBuf), usBuflen);


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
   UINT8 g_ucPackMem[PACK_MEM_SIZE];
   UINT8 g_ucOutBuf[OUT_BUF_LEN];
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

    #if 0
     /* 3 \u5c01\u88c5\u53c2\u6570 */
    memcpy(pscmPacket->sAddress.acRegionCode, "1100", 4);
    pscmPacket->sAddress.usTAddress = 1;
    pscmPacket->sAddress.bTeamAddr  = FALSE;
    pscmPacket->sAddress.ucMAddress = 1;
    pscmPacket->bReSend = FALSE;

    pscmPacket->ucCmdNum = 1;
    pscmPacket->sCmdData[0].eCmd  = CMD_LED_PARA;
    pscmPacket->sCmdData[0].bApp  = TRUE;
    pscmPacket->sCmdData[0].usPN  = 0;
    pscmPacket->sCmdData[0].uAppData.std_11f4.ucDataUnit = 0x1111;
    pscmPacket->sCmdData[0].uAppData.std_11f4.ucTaskType= 0x22;
    pscmPacket->sCmdData[0].uAppData.std_11f4.ucCtrlNum= 0x3333;
    pscmPacket->sCmdData[0].uAppData.std_11f4.ucExeNum= 0x4444;
    memcpy(pscmPacket->sCmdData[0].uAppData.std_11f4.addr[0].address,"99999999",8);
#endif

    eRet = ecm_3761_pack(pscmPacket, (UINT8*)g_ucOutBuf, &usBuflen);
    if(eRet != MT_OK)
    {
        printf("ecm_3761_pack error %d\n", eRet);
        return eRet;
    }

    socket_client_send((UINT8*)(g_ucOutBuf), usBuflen);
    /*注册重发监测*/
    //app_timeout_register(CMD_AFN_11_F4);

    return MT_OK;
}

void app_req_handle(UINT16 eCmd, void* d)
{
    printf("app_req_handle, pSub->eCmd:%02X",eCmd);
    switch(eCmd)
    {
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
        case CMD_AFN_9_F1_TML_VERSION:
            pack_afn09f1_s2m();
        break;
        case CMD_AFN_C_F1_ANALOG_DATA:
            pack_afn0cf01_s2m_analog(d);
        break;
        case CMD_AFN_C_F2_TML_CLOCK:
            pack_afn0cf02_s2m_auto();
        break;
        case CMD_AFN_C_F8_TML_EVNT_FLAG:
            pack_afn0cf08_s2m();
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
            //pack_afn11hf4_s2m();
        
        break;
        
        default:
            printf("not mactch ecmd\n");
        break;
    }
}

/*登录*/
eMtErr app_login(void)
{
    return pack_afn02f01();
}
/*登出*/
eMtErr app_logout(void)
{
    return pack_afn02f2();
}




