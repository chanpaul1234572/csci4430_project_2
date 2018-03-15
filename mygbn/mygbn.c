#include "mygbn.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pthread.h>
#include "mygbn.h"

void mygbn_init_sender(struct mygbn_sender* mygbn_sender, char* ip, int port, int N, int timeout){

}

int mygbn_send(struct mygbn_sender* mygbn_sender, unsigned char* buf, int len){
  return 0;
}

void mygbn_close_sender(struct mygbn_sender* mygbn_sender){

}

void datagen(struct MYGBN_Packet* data, int seqNum, char* buf, int datalen){
  data -> protocol[0] = 'g';
  data -> protocol[1] = 'b';
  data -> protocol[2] = 'n';
  data -> type = 0xA0;
  data -> seqNum = seqNum;
  data -> length = datalen + 12;
  memcpy(data -> payload, buf, datalen);
}

void endgen(struct MYGBN_Packet* data, int seqNum){
  data -> protocol[0] = 'g';
  data -> protocol[1] = 'b';
  data -> protocol[2] = 'n';
  data -> type = 0xA1;
  data -> seqNum = seqNum;
  data -> length = 12;
  memset(data -> payload, '\0', 512);
}

void mygbn_init_receiver(struct mygbn_receiver* mygbn_receiver, int port){
  int one = 1;
	mygbn_receiver->sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (mygbn_receiver->sd < 0){
		perror("Error : Cannot create receiver socket\n");
		exit(-1);
	}
	if (setsockopt(mygbn_receiver -> sd, SOL_SOCKET, SO_REUSEADDR, (char*)&one, sizeof(one)) < 0){
		perror("Error: cannot set socket option\n");
		exit(-1);
	}
	mygbn_receiver -> server_addr.sin_family = AF_INET;
	mygbn_receiver -> server_addr.sin_port = htonl(port);
	mygbn_receiver -> server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	mygbn_receiver -> currentSeqNum = 0;
	mygbn_receiver -> expectedSeqNum = 1;

	if (bind(mygbn_receiver -> sd, (struct sockaddr*) &(mygbn_receiver -> server_addr)), 
		sizeof(mygbn_receiver -> server_addr) < 0) {
			perror("Bind failed");
			exit(1);
		}
}

int mygbn_recv(struct mygbn_receiver* mygbn_receiver, unsigned char* buf, int len){
  int fd = mygbn_receiver -> sd;
	char bufrec[524];
	struct MYGBN_Packet packet;
	int len = recvfrom(fd, bufrec, sizeof(struct MYGBN_Packet), 0, NULL, NULL);
	if len == -1{
		//error occured
		return -1;
	}
	memcpy(&packet, bufrecv, 524);
	if (packet.protocol[0] == 'g' && packet.protocol[1] == 'b' && packet.protocol[2] == 'n')
	{
		if(packet.type == 0xA0){
			if (packet.seqNum == mygbn_receiver -> expectedSeqNum){
				memcpy(buf, &packet.payload, packet.length - 12);
				mygbn_receiver -> currentSeqNum++;
				mygbn_receiver -> expectedSeqNum++;
				return packet.length;
			}
			struct MYGBN_Packet ackn;
			endgen(&ackn, mygbn_receiver -> currentSeqNum);
			int len = sendto(fd, (char*)&ackn, 12, 0, (struct sockaddr*)&(mygbn_receiver -> server_addr), sizeof(mygbn_receiver -> server_addr));
		}
		else if(packet.type == 0xA2){
			buf = '\0';
			mygbn_close_receiver(mygbn_receiver);
			return 0;
		}
	}
  return 0;
}

void mygbn_close_receiver(struct mygbn_receiver* mygbn_receiver) {

}
