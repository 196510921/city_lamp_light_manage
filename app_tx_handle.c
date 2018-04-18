#include "app_handle.h"


/*终端事件标志状态*/
int afn_0c_f8_event_report(int event)
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




