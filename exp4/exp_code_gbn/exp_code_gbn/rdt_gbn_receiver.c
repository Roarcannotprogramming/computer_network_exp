#include "net_exp.h"

void usage( char **argv )
{
	printf( "wrong argument!\n" );
	printf( "usage: %s save_file_name\n",  argv[0] );
}

/*
	Go-Back-N 协议接收端接受函数
	输入参数：
		save_file_name： 保存文件名
		sock_fd：接受数据的socket
*/
int receive_file( char *save_file_name, int sock_fd )
{
	char reply_pkt_buf[RDT_PKT_LEN];
	int reply_pkt_len;
	
	char rdt_pkt[RDT_PKT_LEN];
	char rdt_data[RDT_DATA_LEN];
	uint32_t seq_net_order;
	int seq_num;
	int flag;
	int exp_seq_num;
	
	int total_recv_byte = 0;
	
	struct sockaddr_in client_addr;
	int i, j, sin_len, pkt_len, data_len;
	
	int counter = 1;
	FILE *fp;
	
	if( (fp = fopen( save_file_name, "w" )) == NULL )
	{
		printf( "open file : %s failed.\n",  save_file_name );
		return 1;
	}
	
	memset( &client_addr, 0, sizeof(client_addr) );
	sin_len = sizeof( client_addr );

	exp_seq_num = RDT_BEGIN_SEQ;
	
	//TODO
	while(1) //接收RDT数据包，直到所有数据全部接收完毕
	{			
		/*
			step 1. 接收RDT数据包 :	recvfrom()
			step 2. 解封装RDT数据包 : unpack_rdt_pkt()
			step 3. 检查此数据包是否为期待的数据包 : seq_num==exp_seq_num
			step 4. 封装一个新的RDT数据包(ACK包) : pack_rdt_pkt()
			step 5. 调用不可靠数据传输发送新的RDT数据包(ACK包): udt_sendto()
		*/													
	}
	printf( "\n\nreceive file succeed. write to file %s\ntotal recv %d byte\n", 
				save_file_name, total_recv_byte );
		
	fflush( fp );
	fclose( fp );
	return 0;
}

int main( int argc, char **argv )
{
	struct sockaddr_in recv_addr;
	int sin_len;
	int sock_fd;
	int pkt_len;
	
	srand ( time(NULL) );
	
	if( argc != 2 )
	{
		usage( argv );
		exit(0);
	}
	
	memset( &recv_addr, 0, sizeof(recv_addr) );
	recv_addr.sin_family = AF_INET;
	recv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
	recv_addr.sin_port = htons( RDT_RECV_PORT );
	
	if( ( sock_fd = socket( AF_INET, SOCK_DGRAM, 0 ) ) == -1 )
	{
		printf( "error! information: %s\n", strerror(errno) );
		exit(1);	
	}
	
	if( bind( sock_fd, (struct sockaddr *)&recv_addr, sizeof(recv_addr) ) == -1 )
	{
		close( sock_fd );
		printf( "error! information: %s\n", strerror(errno) );
		exit(1);	
	}
	
	if( receive_file( argv[1], sock_fd ) != 0 )
	{
		printf( "receive file %s failed.\n", argv[1] );
		close( sock_fd );
		exit(1);
	}
	
	close( sock_fd );
	return 0;
}


