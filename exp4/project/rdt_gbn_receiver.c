#include "net_exp.h"

void usage(char **argv) {
    printf("wrong argument!\n");
    printf("usage: %s save_file_name\n", argv[0]);
}

/*
	Go-Back-N 协议接收端接受函数
	输入参数：
		save_file_name： 保存文件名
		sock_fd：接受数据的socket
*/
int receive_file(char *save_file_name, int sock_fd) {
    char reply_pkt_buf[RDT_PKT_LEN];
    int reply_pkt_len;

    char recv_buf[RDT_SENDWIN_LEN][RDT_DATA_LEN];
    int recv_len[RDT_SENDWIN_LEN];
//    int recv_stat[RDT_SENDWIN_LEN];
    int this = 0;

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
    int clear_flag = 0, end_flag = 1, done_flag = 1;

    if ((fp = fopen(save_file_name, "w")) == NULL) {
        printf("open file : %s failed.\n", save_file_name);
        return 1;
    }


    memset(&client_addr, 0, sizeof(client_addr));
    sin_len = sizeof(client_addr);

    exp_seq_num = RDT_BEGIN_SEQ;

    //TODO
    while (1) //接收RDT数据包，直到所有数据全部接收完毕
    {
        /*
            step 1. 接收RDT数据包 :	recvfrom()
            step 2. 解封装RDT数据包 : unpack_rdt_pkt()
            step 3. 检查此数据包是否为期待的数据包 : seq_num==exp_seq_num
            step 4. 封装一个新的RDT数据包(ACK包) : pack_rdt_pkt()
            step 5. 调用不可靠数据传输发送新的RDT数据包(ACK包): udt_sendto()
        */


        // Receive packet from client
        if ((pkt_len = (int) recvfrom(sock_fd, rdt_pkt, RDT_PKT_LEN, 0, (struct sockaddr *) &client_addr, sin_len)) <
            0) {
            return -1;
        }

        // Extract the packet
        if ((data_len = unpack_rdt_pkt(rdt_data, rdt_pkt, pkt_len, &seq_num, &flag)) < 0) {
            return -1;
        }

        // Test if the packet should be received
        memset(reply_pkt_buf, 0, RDT_PKT_LEN);
        if (seq_num < exp_seq_num || seq_num >= exp_seq_num + RDT_SENDWIN_LEN) {
            return -1;
        }

        // If the packet should be received
        if (seq_num >= exp_seq_num && seq_num < exp_seq_num + RDT_SENDWIN_LEN) {

            // Add received data to circle buff
            strcpy(recv_buf[(this + seq_num - exp_seq_num) % RDT_SENDWIN_LEN], rdt_data);
            recv_len[(this + seq_num - exp_seq_num) % RDT_SENDWIN_LEN] = data_len;

            // Pack packet
            if ((reply_pkt_len = pack_rdt_pkt(reply_pkt_buf, rdt_data, data_len, seq_num, RDT_CTRL_ACK)) < 0) {
                printf("pack reply packet error");
            }

            // Send packet to client
            udt_sendto(sock_fd, reply_pkt_buf, reply_pkt_len, 0, (struct sockaddr *) &client_addr, sin_len);

            // Protect the circle buff
            if (seq_num == exp_seq_num + 1) {
                for (; recv_len[this] >= 0; this = (this + 1) % RDT_SENDWIN_LEN) {
                    fputs(recv_buf[this], fp);
                    recv_len[this] = -1;
                    exp_seq_num++;
                }
            }
        }

        // Test if the last packet received
        if (flag == RDT_CTRL_END)
            end_flag = 0;  // Set end flag

        // Test if the circle buff is clear
        if (end_flag == 0) {
            clear_flag = 0;  //Set clear flag first
            for (int jj = 0; jj < RDT_SENDWIN_LEN; jj++) {
                if (recv_len[jj] >= 0)
                    clear_flag = 1;  // Not Clear, unset the clear flag
                if (jj == RDT_SENDWIN_LEN - 1 && clear_flag == 0) {
                    done_flag = 0;  // Done
                }
            }
        }

        // If done end the while loop
        if (done_flag == 0) {
            break;
        }


    }
    printf("\n\nreceive file succeed. write to file %s\ntotal recv %d byte\n",
           save_file_name, total_recv_byte);

    fflush(fp);
    fclose(fp);
    return 0;
}

int main(int argc, char **argv) {
    struct sockaddr_in recv_addr;
    int sin_len;
    int sock_fd;
    int pkt_len;

    srand(time(NULL));

    if (argc != 2) {
        usage(argv);
        exit(0);
    }

    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_addr.sin_port = htons(RDT_RECV_PORT);

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("error! information: %s\n", strerror(errno));
        exit(1);
    }

    if (bind(sock_fd, (struct sockaddr *) &recv_addr, sizeof(recv_addr)) == -1) {
        close(sock_fd);
        printf("error! information: %s\n", strerror(errno));
        exit(1);
    }

    if (receive_file(argv[1], sock_fd) != 0) {
        printf("receive file %s failed.\n", argv[1]);
        close(sock_fd);
        exit(1);
    }

    close(sock_fd);
    return 0;
}


