
PROGS = rdt_stopwait_receiver rdt_stopwait_sender

CC = cc		
CFLAGS = -g #-Wall
		
all: ${PROGS}
		
rdt_stopwait_receiver: rdt_stopwait_receiver.o rdt_pkt_util.c net_exp.h
	${CC} ${CFLAGS} -o $@ rdt_stopwait_receiver.o rdt_pkt_util.c
	
rdt_stopwait_sender: rdt_stopwait_sender.o rdt_pkt_util.c net_exp.h
	${CC} ${CFLAGS} -o $@ rdt_stopwait_sender.o rdt_pkt_util.c
	
.PHONY: clean	

clean:
	rm -f *.o *.exe ${PROGS} *.gz *.tar *.log

tar:
	tar czf exp_code_stopwait.tar.gz rdt_stopwait_receiver.c rdt_stopwait_sender.c \
	rdt_pkt_util.c net_exp.h README Makefile
