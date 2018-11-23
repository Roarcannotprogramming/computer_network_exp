#include "net_exp.h"

/*
	将数据封装成RDT数据包：即在数据前加上RDT数据包头部
*/
int pack_rdt_pkt( char *data_buf, char *rdt_pkt, int data_len, int seq_num, int flag )
{
	char *ptr = rdt_pkt;
	uint32_t ctrl_net_order = htonl( flag );
	uint32_t seq_net_order = htonl( seq_num );

	memcpy( ptr, &ctrl_net_order, sizeof(uint32_t) );
	ptr += sizeof(uint32_t);
	memcpy( ptr, &seq_net_order, sizeof(uint32_t) );
	ptr += sizeof(uint32_t);
	
	if( data_len > 0 && data_buf != NULL )
		memcpy( ptr, data_buf, data_len );
	return (RDT_HEADER_LEN+data_len);
}

/*
	将RDT数据包解封装
*/
int unpack_rdt_pkt( char *data_buf, char *rdt_pkt, int pkt_len, int *seq_num, int *flag )
{
	char *ptr = rdt_pkt;
	uint32_t ctrl_net_order, seq_net_order;
	int data_len;
	
	memcpy( &ctrl_net_order, ptr, sizeof(uint32_t) );
	ptr += sizeof(uint32_t);
	*flag = ntohl( ctrl_net_order );
	
	memcpy( &seq_net_order, ptr, sizeof(uint32_t) );
	ptr += sizeof(uint32_t);
	*seq_num = ntohl( seq_net_order );
	
	data_len = pkt_len - RDT_HEADER_LEN;
	if( data_buf != NULL && data_len > 0 )
		memcpy( data_buf, ptr, data_len );
	return data_len;
}

void udt_sendto( int sock_fd, char *pkt, int pkt_len, int flags, struct sockaddr *recv_addr, int addr_len )
{
	int seed =  rand() % 100;
	if( seed >= RDT_PKT_LOSS_RATE )
		sendto( sock_fd, pkt, pkt_len, flags, recv_addr, addr_len );
	else //pkt lost
		printf( "emulate packet[next] lost!\n" );
}

int time_out( struct timeval time_1, struct timeval time_2 ) //time_1 > time_2
{
	int elapse_time = (time_1.tv_sec - time_2.tv_sec)*1000000 + (time_1.tv_usec - time_2.tv_usec);
	if( elapse_time > RDT_TIME_OUT )
	{	
		printf( "[main thread] pkt timeout, elapsed_time : %ld\n", elapse_time );
		return 1;
	}
	return 0;
}

