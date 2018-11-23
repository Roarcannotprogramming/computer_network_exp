#ifndef NETEXP_H
#define NETEXP_H

#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>


#define		RDT_SERVER_ADDRESS		"127.0.0.1"		//RDT服务器端IP
#define 	RDT_RECV_PORT			8003            //RDT接收端端口号
#define		RDT_SEND_PORT			8004            //RDT发送端端口号
#define		RDT_BEGIN_SEQ			1               //RDT数据包初始序列号，（假设数据包序列号不循环）

#define		RDT_PKT_LOSS_RATE		10				//不可靠数据传输层的丢包率
#define		RDT_TIME_OUT			5000            //数据包超时时限
#define		RDT_HEADER_LEN			(4 + 4)         //RDT头标长度
#define 	RDT_DATA_LEN			1000            //RDT中数据域长度
#define		RDT_PKT_LEN				( RDT_DATA_LEN + RDT_HEADER_LEN )	//RDT中数据包长度

//RDT包类型
#define		RDT_CTRL_BEGN			0	//初始包
#define		RDT_CTRL_DATA			1   //数据包
#define 	RDT_CTRL_ACK			2   //ACK包
#define		RDT_CTRL_NACK			3   //NACK包
#define		RDT_CTRL_END			4	//结束包


/*
	RDT packet format: |CTRL|SEQ|...DATA...|
	将数据封装成RDT数据包，头部只包含控制域和序列号域，其中控制域用来标识RDT数据包类型,序列号域是包含此数据包的序列号。
	函数返回RDT数据包长度
*/
int pack_rdt_pkt( char *data_buf, char *rdt_pkt, int data_len, int seq_num, int flag );

/*
	RDT packet format: |CTRL|SEQ|...DATA...|
	将数据包解封装。
	函数返回RDT包中的数据长度
*/
int unpack_rdt_pkt( char *data_buf, char *rdt_pkt, int pkt_len, int *seq_num, int *flag );

/*
	模拟不可靠数据传输，以一定的概率（RDT_PKT_LOSS_RATE）丢弃数据包，调用形式和recvfrom一致
*/
void udt_sendto( int sock_fd, char *pkt, int pkt_len, int flags, struct sockaddr *recv_addr, int addr_len );

#endif
