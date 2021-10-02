/*
 * A tiny and rough simple network time protocol client
 * for time synchronization when computing TOTP tokens.
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
	char buf[48];
	bzero(&buf,48);
	buf[0]=(1<<3)|3;
	struct hostent *host;
	host=gethostbyname((argc>1 ? argv[1]:"pool.ntp.org"));
	if(!host) {
		perror("gethostbyname");
		exit(1);
	}
	struct sockaddr_in saddr;
	bzero(&saddr, sizeof(struct sockaddr_in));
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(123);
	saddr.sin_addr.s_addr=*(long *)(host->h_addr_list[0]);
	if(connect(sock,(struct sockaddr *)&saddr,sizeof(struct sockaddr_in))!=0) {
		perror("connect");
		exit(1);
	}
	if(write(sock,buf,48)<=0) {
		perror("write");
		exit(1);
	}
	if(read(sock,buf,48)<=0) {
		perror("read");
		exit(1);
	}
	if(getenv("DEBUG")) write(1,buf,48);
	uint32_t tm=((buf[32]<<24) | (buf[33]<<16) | (buf[34]<<8) | buf[35]);
	tm=tm-2208988800L;
	printf("%u\n",tm);
	// (  date -u -d "1900-01-01 00:00" +%s == -2208988800 )
}
