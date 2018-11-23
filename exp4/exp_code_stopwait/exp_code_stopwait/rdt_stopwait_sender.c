#include "net_exp.h"

void usage( char **argv )
{
	printf( "wrong argument!\n" );
	printf( "usage: %s send_file_name. \n",  argv[0] );
}


/*
	停等协议发送端函数
	输入参数：
		send_file_name: 待发送的文件名，将此文件中的数据封装成一个个的数据包进行发送
		sock_fd：发送数据的socket (同时从该socket发送数据包和接收数据包ACK)
		recv_addr_ptr: 接收端的地址
*/
int deliver_file( char *send_file_name, int sock_fd,  struct sockaddr_in *recv_addr_ptr )
{
	char recv_pkt_buf[RDT_PKT_LEN];
	char rdt_pkt[RDT_PKT_LEN];
	int seq_num = RDT_BEGIN_SEQ;
	int flag;
	int rdt_pkt_len;
	int total_send_byte = 0;
	
	struct sockaddr_in reply_addr;
	int reply_addr_len;
	int reply_ack_seq;
	int reply_ack_flag;

	char send_window[RDT_DATA_LEN]; //发送端窗口大小为1
	
	FILE *fp;
	int i, j, read_len, pkt_len;
	int counter = 1;

	if( (fp = fopen( send_file_name, "r" )) == NULL )
	{
		printf( "open file : %s failed.\n",  send_file_name );
		return 1;
	}
	
	while(1)
	{
		if( feof( fp ) )//如果已经读到发送文件的结尾，则设置数据包类型为RDT_CTRL_END
		{
			flag = RDT_CTRL_END;
			read_len = 0;
			rdt_pkt_len = pack_rdt_pkt( NULL, rdt_pkt, 0, seq_num, flag );
		}
		else //设置数据包类型为RDT_CTRL_DATA
		{
			flag = RDT_CTRL_DATA;	
			read_len = fread( send_window, sizeof(char), RDT_DATA_LEN, fp );
			rdt_pkt_len = pack_rdt_pkt( send_window, rdt_pkt, read_len, seq_num, flag );
		}
		
		while(1) //开始发送
		{
			fd_set fds; //文件描述符集合
			struct timeval timeout;
			int sock_state;
			
			printf( "send count #%d, rdt_pkt #%d: %d bytes.\n", counter++, seq_num, rdt_pkt_len );

			udt_sendto( sock_fd, rdt_pkt, rdt_pkt_len, 0,
					(struct sockaddr *)recv_addr_ptr, sizeof(*recv_addr_ptr) );

			timeout.tv_sec = 0;
			timeout.tv_usec = RDT_TIME_OUT ;
			FD_ZERO( &fds ); //初始化文件描述符
			FD_SET( sock_fd, &fds ); //将sock_fd加入到文件描述符集合中
			
			//一直等待到文件描述符集合中某个文件有可读数据，或者到达超时时限
			sock_state = select( sock_fd + 1, &fds, NULL, NULL, &timeout );

			if( sock_state == -1 ) //socket 错误
			{
				printf( "select failed.\n" );
				return 1;
			}
			else if( sock_state == 0 ) //数据包超时
			{
				printf( "packet #%d time out, resend....\n",  seq_num );
				continue; //重新发送
			}
			else //文件描述符集合中某个文件有可读数据
			{
				if( FD_ISSET( sock_fd, &fds ) ) //sock_fd有数据到达
				{
					memset( &reply_addr, 0, sizeof(reply_addr) );
					reply_addr_len = sizeof( reply_addr );
					pkt_len = recvfrom( sock_fd, recv_pkt_buf, RDT_PKT_LEN, 0, 
						(struct sockaddr *)&reply_addr, &reply_addr_len );
							
					unpack_rdt_pkt( NULL, recv_pkt_buf, pkt_len, &reply_ack_seq, &reply_ack_flag );
					
					//发送成功，开始发送下一个数据包
					if( reply_ack_seq == seq_num && reply_ack_flag == RDT_CTRL_ACK )
					{
						printf( "receive ACK for rdt_pkt #%d\n", seq_num);
						break; //跳出当前循环
					}	
					if( reply_ack_flag == RDT_CTRL_NACK  ) //接收到NACK数据包
						continue; //重新发送
				}
			}
		}
		
		seq_num++;
		total_send_byte += read_len;
		if( flag == RDT_CTRL_END ) //如果所有数据包都发送完，则结束发送
			break; //跳出当前循环
	}
	printf( "\n\nsend file %s finished\ntotal send %5d bytes.\n", send_file_name, total_send_byte );
	fclose( fp );
	return 0;
}

int main( int argc, char **argv )
{
	struct sockaddr_in recv_addr, send_addr;
	int sock_fd;

	if( argc != 2 )
	{
		usage( argv );
		exit(0);
	}
	
	srand ( time(NULL) ); //初始化随机数种子
	
	if( ( sock_fd = socket( AF_INET, SOCK_DGRAM, 0 ) ) == -1 )
	{
		printf( "error! information: %s\n", strerror(errno) );
		exit(1);	
	}
	memset( &send_addr, 0, sizeof(send_addr) );
	send_addr.sin_family = AF_INET;
	send_addr.sin_addr.s_addr = htonl( INADDR_ANY );
	send_addr.sin_port = htons( RDT_SEND_PORT );
	
	if( ( sock_fd = socket( AF_INET, SOCK_DGRAM, 0 ) ) == -1 )
	{
		printf( "error! information: %s\n", strerror(errno) );
		exit(1);	
	}
	
	if( bind( sock_fd, (struct sockaddr *)&send_addr, sizeof(send_addr) ) == -1 )
	{
		close( sock_fd );
		printf( "error! information: %s\n", strerror(errno) );
		exit(1);	
	}
			
	memset( &recv_addr, 0, sizeof(recv_addr) );
	recv_addr.sin_family = AF_INET;
	recv_addr.sin_addr.s_addr = inet_addr( RDT_SERVER_ADDRESS );
	recv_addr.sin_port = htons( RDT_RECV_PORT );

	if( deliver_file( argv[1], sock_fd, &recv_addr ) != 0 )
	{
		printf( "deliver file %s failed.\n", argv[1] );
		close( sock_fd );
		exit(1);
	}
	
	close( sock_fd );
	return 0;
}

