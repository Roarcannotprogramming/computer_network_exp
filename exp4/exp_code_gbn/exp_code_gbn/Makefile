
PROGS = rdt_gbn_receiver rdt_gbn_sender

CC = cc		
CFLAGS = -g #-Wall
		
all: ${PROGS}
		
rdt_gbn_receiver: rdt_gbn_receiver.o rdt_pkt_util.c net_exp.h
	${CC} ${CFLAGS} -lpthread -o $@ rdt_gbn_receiver.o rdt_pkt_util.c
	
rdt_gbn_sender: rdt_gbn_sender.o rdt_pkt_util.c net_exp.h
	${CC} ${CFLAGS} -lpthread -o $@ rdt_gbn_sender.o rdt_pkt_util.c
	
.PHONY: clean	

clean:
	rm -f *.o *.exe ${PROGS} exp_code_gbn.tar.gz

tar:
	tar czf exp_code_gbn.tar.gz rdt_gbn_receiver.c rdt_gbn_sender.c \
	rdt_pkt_util.c net_exp.h README Makefile

