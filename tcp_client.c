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

#define SERVER_IP  "172.16.200.1"
#define SERV_PORT 6666
// #define SERVER_IP  "192.168.9.100"
// #define SERV_PORT 10004

static int conn_flag = TCP_DISCONNECTED;
static int client_sockfd = 0;
static char clientTcpRxBuf[2048] = {0};

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
			//sleep(5);
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
		return TCP_CLIENT_FAILED;
	}

	return TCP_CLIENT_SUCCESS;
}


extern void unpack_analyse(int nstart, int argc, char *argv[]);
extern void test_unpack(int nstart, int argc, char *argv[]);

static void client_rx_cb(int clientFd)
{
	int byteToRead = 0;
	int byteRead = 0;
	int rtn = 0;
	int i = 0;


	printf("SRPC_RxCB++[%x]\n", clientFd);

	rtn = ioctl(clientFd, FIONREAD, &byteToRead);

	if (rtn != 0)
	{
		printf("SRPC_RxCB: Socket error\n");
	}
	printf("byteToRead:%d\n",byteToRead);

	byteRead = read(clientFd, clientTcpRxBuf, 2048);

	printf("client rx, len:%d, data: \n");
	for( i = 0; i < byteRead; i++)
		printf("%X ",clientTcpRxBuf[i]);

	printf("--------------------------------------unpack-------------------------------------------------\n\n\n\n\n\n");
	test_unpack(0, byteRead, clientTcpRxBuf);
	printf("--------------------------------------unpack_analyse-------------------------------------------------\n\n\n\n\n\n");
	unpack_analyse(0, byteRead, clientTcpRxBuf);

	return;
}

