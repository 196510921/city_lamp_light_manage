#define _GNU_SOURCE  1
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


enum conn_flag_t{
	TCP_CONNECTED = 0,
	TCP_DISCONNECTED
};

enum block_status_t{
	TCP_CLIENT_FAILED = 0,
	TCP_CLIENT_SUCCESS
};

#define PACK_MEM_SIZE  10240
#define OUT_BUF_LEN    10240

typedef char                INT8;       //c
typedef short               INT16;      //s
typedef int                 INT32;      //l
typedef unsigned char       UINT8;      //uc
typedef unsigned short      UINT16;     //us
typedef unsigned int        UINT32;  

extern UINT8 g_ucInBuf[OUT_BUF_LEN];

#if 0
#define SERVER_IP  "192.168.1.210"
#define SERV_PORT 14000
#else
	#if 0
	#define SERVER_IP  "101.132.91.12"
	#define SERV_PORT 14100
	#else
	#define SERVER_IP  "172.16.200.1"
	#define SERV_PORT 6666
#endif
#endif

static int conn_flag = TCP_DISCONNECTED;
static int client_sockfd = 0;
static UINT8 clientTcpRxBuf[2048] = {0};

static int socket_client_handle(int clientFd, int revent);
static void client_rx_cb(int clientFd);

static void set_connect_flag(int d)
{
	conn_flag = d;
}

static int get_connect_flag(void)
{
	return conn_flag;
}

int tcp_client_connect(void)
{
	struct sockaddr_in  servaddr;
	struct hostent *host;
	char* server_ip = SERVER_IP;
	int i;

   	client_sockfd = socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, server_ip, &servaddr.sin_addr);
	if(connect(client_sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1){
		perror("server connect fialed:\n");
		set_connect_flag(TCP_DISCONNECTED);
		return TCP_CLIENT_FAILED;
	}else{
	 	printf("server connect ok\n");
	 	set_connect_flag(TCP_CONNECTED);
	}

	return TCP_CLIENT_SUCCESS;
}


void socket_client_poll(void)
{
	while (1)
	{
		if(get_connect_flag() == TCP_DISCONNECTED){
			printf("reconnect...\n");
			sleep(5);
			/*reconnect*/ 
			if(tcp_client_connect() != TCP_CLIENT_SUCCESS)
				continue;
		}
		printf("socket_client_poll\n");
	
		int pollFdIdx;
		int ret;
		//socket client FD
		struct pollfd *pollFds = malloc(sizeof(struct pollfd));

		if(pollFds)
		{
			/*set client port in the poll file descriptors*/
			pollFds[0].fd = client_sockfd;
			pollFds[0].events = POLLIN | POLLRDHUP;

			printf("client waiting for poll()\n");

			poll(pollFds, 1, -1);
			//poll(pollFds, numClientFds, -1);
			printf("client poll out\n");
			/*client*/
			if ((pollFds[0].revents))
			{
				printf("Message from Socket Sever\n");
				socket_client_handle(pollFds[0].fd, pollFds[0].revents);
			}

			free(pollFds);
			printf("free client\n");
		}
		
	}

}

static int socket_client_handle(int clientFd, int revent)
{
	printf("pollClientSocket++, revent: %d\n",revent);

	//this is a client socket is it a input or shutdown event
	if (revent & POLLIN)
	{
		uint32_t pakcetCnt = 0;
		//its a Rx event
		printf("got Rx on fd %d, pakcetCnt=%d\n", clientFd, pakcetCnt++);

		client_rx_cb(clientFd);
	}

	if (revent & POLLRDHUP)
	{
		printf("POLLRDHUP\n");
		//its a shut down close the socket
		printf("Client fd:%d disconnected\n", clientFd);
		set_connect_flag(TCP_DISCONNECTED);
		return TCP_CLIENT_FAILED;
	}

	return TCP_CLIENT_SUCCESS;
}


void unpack(UINT8* pInBuf, UINT16 usLen);

static void client_rx_cb(int clientFd)
{
	int byteToRead = 0;
	int byteRead = 0;
	int rtn = 0;
	int i = 0;
	UINT8* pInBuf = NULL;


	printf("SRPC_RxCB++[%x]\n", clientFd);

	rtn = ioctl(clientFd, FIONREAD, &byteToRead);

	if (rtn != 0)
	{
		printf("SRPC_RxCB: Socket error\n");
	}
	printf("byteToRead:%d\n",byteToRead);

	byteRead = read(clientFd, clientTcpRxBuf, 2048);

	if(byteRead > 0){
		printf("client rx, len:%d, data: \n",byteRead);
		for( i = 0; i < byteRead; i++)
			printf("%02X ",clientTcpRxBuf[i]);
		printf("\n");

		printf("--------------------------------------unpack-------------------------------------------------\n\n\n\n\n\n");
		unpack(clientTcpRxBuf, byteRead);
	}

	return;
}


void socket_client_send(UINT8* data, UINT16* len)
{
	int rtn;

	rtn = write(client_sockfd, data, len);
	if(rtn < 0)
		printf("write error\n");
}




