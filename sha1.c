#include <stdint.h>
#ifdef __TEST
#include <stdio.h>
#endif

uint32_t rotateLeft(uint32_t x, uint32_t n) {
  return ((x << n) | (x >> (32 - n)));
}

void sha1(uint8_t *data, uint32_t datalen, uint8_t *out) {
	uint32_t h[]={ 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };	
	unsigned int i, j, padlen, chunks, chnk;
	uint8_t work[128];
	if(datalen>120) return;
	for(i=0; i<datalen; i++) work[i]=data[i];
	work[i++]=0x80;
	padlen=(datalen+9)%64;
	if(padlen>0) padlen=64-padlen;
	chunks=1+(datalen+8)/64;
	for(; i<datalen+1+padlen; i++) work[i]=0;
	j=7;
	while(1) {
		work[i++]=((8*(uint64_t)datalen) & (0xff << (8*j))) >> (8*j); 
		if(j==0) break;
		j--;
	}
	for(chnk=0; chnk<chunks; chnk++) {
		uint32_t a=h[0], b=h[1], c=h[2], d=h[3], e=h[4], temp, w[80];
		for(i=0; i<16; i++)
			w[i]=work[chnk*64+i*4+3] | (work[chnk*64+i*4+2]<<8) | (work[chnk*64+i*4+1]<<16) | (work[chnk*64+i*4]<<24);
		for(i=16; i<80; i++)
			w[i]=rotateLeft(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);
		for(i=0; i<80; i++) {
			temp=rotateLeft(a, 5)+e+w[i];
			if(i<20) {
				temp+=((b & c) | ((~b) & d)) + 0x5a827999;
			} else if(i<40) {
				temp+=(b ^ c ^ d) + 0x6ed9eba1;
			} else if(i<60) {
				temp+=((b & c) | (b & d) | (c & d)) + 0x8f1bbcdc;
			} else {
				temp+=(b ^ c ^ d) + 0xca62c1d6;
			}
			e=d;
			d=c;
			c=rotateLeft(b, 30);
			b=a;
			a=temp;
		}
		h[0]+=a;
		h[1]+=b;
		h[2]+=c;
		h[3]+=d;
		h[4]+=e;
	}

	for(i=0; i<5; i++) 
		for(j=0; j<4; j++) 
			out[j+4*i]=(( h[i] & (0xff << (24-8*j)) ) >> (24-8*j));
}

void hmac_sha1(uint8_t *key, uint32_t keylen, uint8_t *data, uint32_t datalen, uint8_t *out) {
	uint8_t i, tmp[128], tmp2[84];
	if(keylen>64 || datalen>64) return;
	for(i=0; i<keylen; i++) {
		tmp[i]=tmp2[i]=key[i];
	}
	for(i=keylen; i<64; i++) {
		tmp[i]=tmp2[i]=0;
	}
	for(i=0; i<64; i++) {
		tmp[i] ^= 0x36;
		tmp2[i] ^= 0x5c;
	}
	for(i=0; i<datalen; i++)
		tmp[64+i]=data[i];
	sha1(tmp, 64+datalen, tmp2+64);
	sha1(tmp2, 84, out);
}

#ifdef __TEST
int main(int argc, char **argv) {
	uint8_t z[3]="abc";
	uint8_t key[4]="key1";
	uint8_t out[20];
	/*
	sha1(z, 3, out);
	for(int i=0;i<20;i++) printf("%02hhx", out[i]);
	return(0);
	*/
	hmac_sha1(key, 4, z, 3, out);
	for(int i=0;i<20;i++) printf("%02hhx", out[i]);
	printf("\n");
	return(0);
}
#endif
