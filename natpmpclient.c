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

int main(int argc, char **argv) {
	int sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	uint8_t len;
	uint8_t buf[256];
	bzero(buf,256);
	struct hostent *host;
	host=gethostbyname(argv[1]);
	if(!host) {
		perror("gethostbyname");
		exit(1);
	}
	struct sockaddr_in saddr;
	bzero(&saddr, sizeof(struct sockaddr_in));
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(5351);
	saddr.sin_addr.s_addr=*(long *)(host->h_addr_list[0]);
	struct sockaddr_in my_addr;
	bzero(&my_addr, sizeof(struct sockaddr_in));
	my_addr.sin_family=AF_INET;
	my_addr.sin_port=htons(22222);
	if(bind(sock,(struct sockaddr *)&my_addr,sizeof(struct sockaddr_in))) {
		perror("bind");
		exit(1);
	}
	if(connect(sock,(struct sockaddr *)&saddr,sizeof(struct sockaddr_in))!=0) {
		perror("connect");
		exit(1);
	}
	write(sock,buf,2);
	read(sock,buf,12);
	if(getenv("DEBUG")) write(1,buf,12);
}
