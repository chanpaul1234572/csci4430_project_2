/*
 * mygbn.h
 */

#ifndef __mygbn_h__
#define __mygbn_h__
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
#define MAX_PAYLOAD_SIZE 512

struct MYGBN_Packet {
  unsigned char protocol[3];                  /* protocol string (3 bytes) "gbn" */
  unsigned char type;                         /* type (1 byte) */
  unsigned int seqNum;                        /* sequence number (4 bytes) */
  unsigned int length;                        /* length(header+payload) (4 bytes) */
  unsigned char payload[MAX_PAYLOAD_SIZE];    /* payload data */
}__attribute__((packed));

struct mygbn_sender {
  int sd; // GBN sender socket
  // ... other member variables
  int N;
  int timeout;
  int send_base;
  int nextSeqNum;
  int ackedSeqNum;
  struct sockaddr server_addr;
};

void mygbn_init_sender(struct mygbn_sender* mygbn_sender, char* ip, int port, int N, int timeout);
int mygbn_send(struct mygbn_sender* mygbn_sender, unsigned char* buf, int len);
void mygbn_close_sender(struct mygbn_sender* mygbn_sender);

struct mygbn_receiver {
  int sd; // GBN receiver socket
  // ... other member variables
  struct sockaddr_in server_addr;
  int currentSeqNum;
  int expectedSeqNum;
};
void datagen(struct MYGBN_Packet* data, int seqNum, char* buf, int datalen);
void endgen(struct MYGBN_Packet* data, int seqNum);
void mygbn_init_receiver(struct mygbn_receiver* mygbn_receiver, int port);
int mygbn_recv(struct mygbn_receiver* mygbn_receiver, unsigned char* buf, int len);
void mygbn_close_receiver(struct mygbn_receiver* mygbn_receiver);

#endif
