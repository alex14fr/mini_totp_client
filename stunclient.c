/*
 *
 *
BSD 2-Clause License

Copyright (c) 2021, Alexandre Janon
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>

void dump_address(uint8_t *buf) {
	uint16_t port=buf[7]|(buf[6]<<8);
	printf("%d.%d.%d.%d:%d\n", buf[8], buf[9], buf[10], buf[11], port);
}

void dump_response(uint8_t *buf, uint8_t len) {
	//length of buf=20(header size)+len
	uint8_t i=0;
	if(buf[i]!=1 || buf[i+1]!=1) {
		printf("Unsupported message type=%x%x\n", buf[i], buf[i+1]);
	}
	printf("Binding response:\n");
	i=20;
	while(i<20+len) {
		if(buf[i]!=0) {
			printf("  Unsupported attribute type=%x%x\n", buf[i], buf[i+1]);
		} else {
			switch(buf[i+1]) {
				case 1:
					printf("  Mapped address:   ");
					goto p_addr;
				case 2:
					printf("  Response address: ");
					goto p_addr;
				case 4:
					printf("  Source address:   ");
					goto p_addr;
				case 5:
					printf("  Changed address:  ");
					p_addr: dump_address(buf+i);
					break;
				default:
					printf("  Unsupported attribute type=%x%x\n", buf[i], buf[i+1]);
					break;
			}
		} 
		i+=4+(buf[i+3]|(buf[i+2]<<8));
	}
}

#define STUN_CHANGE_IP 1<<2
#define STUN_CHANGE_PORT 1<<1

int stun_bindreq(uint8_t change_request, int sock, uint8_t *buf, char *len) {
	bzero(buf, 256);
	buf[1]=0x01;
	*len=0;
	if(change_request) {
		*len=8;
		buf[3]=8;
		buf[21]=3;	// type=CHANGE_REQUEST
		buf[23]=4;	// length=4
		buf[27]=change_request;	// value
	}
	if(write(sock,buf,20+*len)<=0) {
		perror("write");
		return(1);
	}
	if(read(sock,buf,256)<=0) {
		perror("read");
		return(1);
	}
	if(buf[2]) {
		printf("response size too large\n");
		return(1);
	}
	*len=buf[3]; //+256*buf[2];
}

int main(int argc, char **argv) {
	int sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	uint8_t len;
	uint8_t buf[256];
	struct hostent *host;
	host=gethostbyname((argc>1 ? argv[1] : "stun.l.google.com"));
	if(!host) {
		perror("gethostbyname");
		exit(1);
	}
	struct sockaddr_in saddr;
	bzero(&saddr, sizeof(struct sockaddr_in));
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons((argc>2 ? atoi(argv[2]) : 3478));
	saddr.sin_addr.s_addr=*(long *)(host->h_addr_list[0]);
	struct sockaddr_in my_addr;
	bzero(&my_addr, sizeof(struct sockaddr_in));
	my_addr.sin_family=AF_INET;
	my_addr.sin_port=htons((argc>3 ? atoi(argv[3]) : 22222));
	if(bind(sock,(struct sockaddr *)&my_addr,sizeof(struct sockaddr_in))) {
		perror("bind");
		exit(1);
	}
	if(connect(sock,(struct sockaddr *)&saddr,sizeof(struct sockaddr_in))!=0) {
		perror("connect");
		exit(1);
	}
	stun_bindreq(0,sock,buf,&len);
	if(getenv("DEBUG")) write(1,buf,20+len);
	dump_response(buf,len);

	printf("With CHANGE_IP:\n");
	stun_bindreq(STUN_CHANGE_IP,sock,buf,&len);
	dump_response(buf,len);

	printf("With CHANGE_PORT:\n");
	stun_bindreq(STUN_CHANGE_PORT,sock,buf,&len);
	dump_response(buf,len);

	printf("With CHANGE_IP|CHANGE_PORT:\n");
	stun_bindreq(STUN_CHANGE_PORT|STUN_CHANGE_IP,sock,buf,&len);
	dump_response(buf,len);
}
