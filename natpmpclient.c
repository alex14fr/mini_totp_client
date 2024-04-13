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
//#include <netinet/in6.h>
#include <netdb.h>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
	int sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	uint8_t len;
	uint8_t buf[256];
	bzero(buf,256);

	struct sockaddr_in laddr;
	bzero(&laddr, sizeof(struct sockaddr_in));
	laddr.sin_family=AF_INET;
	laddr.sin_port=htons(5350);
	laddr.sin_addr.s_addr=INADDR_ANY;
	if(bind(sock,(struct sockaddr*)&laddr,sizeof(struct sockaddr_in))) {
		perror("bind");
		exit(1);
	}
	struct sockaddr_in saddr;
	bzero(&saddr, sizeof(struct sockaddr_in));
	inet_pton(AF_INET,argv[1],&(saddr.sin_addr));
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(5351);
	sendto(sock, buf, 2, 0, &saddr, sizeof(struct sockaddr_in));
	recvfrom(sock, buf, 12, 0, &laddr, sizeof(struct sockaddr_in));
	write(1,buf,12);
}
