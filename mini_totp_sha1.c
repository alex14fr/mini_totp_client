/*
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

unsigned char b32dec_c(unsigned char c) {
	if(c>='a' && c<='z') c=c+'A'-'a';
	if(c>='A' && c<='Z') return(c-'A');
	else if(c>='2' && c<='7') return(c-'2'+26);
	else if(c=='=') return(0);
	else { puts("base32dec: invalid char"); exit(1); }
}

unsigned char b32enc_c(unsigned char v) {
	if(v<=25) return(v+'A');
	else if(v<=31) return(v+'2'-26);
	else return(0);
}

void b32dec_8(unsigned char *in, unsigned char *out) {
	unsigned long long int t=0;
	for(int i=0;i<8;i++) 
		t=(t<<5) | b32dec_c(in[i]);
	for(int i=4;i>=0;i--) {
		out[i]=t & 0xff;
		t=t>>8;
	}
}

int b32dec(unsigned char *in, int inlen, unsigned char *out) {
	int i, j=0;
	if(inlen & 0x7) {
		puts("base32dec: input length must be multiple of 8\n");
		return(-1);
	}
	for(i=0;i<inlen;i+=8) {
		b32dec_8(in+i, out+j);
		j+=5;
	}
	return(j);
}

extern void hmac_sha1(uint8_t *key, uint32_t keylen, uint8_t *data, uint32_t datalen, uint8_t *out);

int main(int argc, char **argv) {
	if(argc<2) {
		printf("usage: %s <base32-encoded secret> [<interval=30>] [<digits=6>] [<UNIX timestamp=now>]\n", argv[0]);
		return(1);
	}
	unsigned char secret[20];
	if(strlen(argv[1])>32) {
		printf("base32-encoded secret must be 32 chars max in length\n");
		return(1);
	}
	int declen=b32dec((unsigned char *)argv[1],strlen(argv[1]),secret);
	int interval=(argc<3 ? 30 : atoi(argv[2]));
	int digits=(argc<4 ? 6 : atoi(argv[3]));
	if(digits<6 || digits>8) {
		printf("digits must be between 6 and 8\n");
		return(1);
	}
	int digitMod[]={1000000,10000000,100000000};
	unsigned long long int cntr;
	cntr=(argc<5 ? time(NULL) : atoi(argv[4]));
	cntr=cntr/interval;
	unsigned char cntrb[8];
	for(int i=7;i>=0;i--)
		cntrb[i]=(cntr>>(8*(7-i)))&0xff;
//	if(argc<5) 
		printf("counter: %lld\nexpires in : %lld secs\n",cntr,interval*(cntr+1)-time(NULL));
	unsigned char hmac_result[20];
	hmac_sha1(secret, declen, cntrb, 8, hmac_result);
	unsigned int offset   =  hmac_result[19] & 0xf ;
	unsigned int bin_code = (hmac_result[offset]  & 0x7f) << 24
		| (hmac_result[offset+1] & 0xff) << 16
		| (hmac_result[offset+2] & 0xff) <<  8
		| (hmac_result[offset+3] & 0xff) ;
	char fmt[10];
	snprintf(fmt,10,"%%0%dd\n",digits);
	printf(fmt, bin_code % digitMod[digits-6]);
	return(0);
}

