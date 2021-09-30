#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
	int sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	char buf[49];
	bzero(&buf,49);
	buf[0]=(1<<3)|3;
	struct hostent *host;
	host=gethostbyname((argc>1 ? argv[1]:"pool.ntp.org"));
	struct sockaddr_in saddr;
	bzero(&saddr, sizeof(struct sockaddr_in));
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(123);
	saddr.sin_addr.s_addr=*(long *)(host->h_addr_list[0]);
	connect(sock,(struct sockaddr *)&saddr,sizeof(struct sockaddr_in));
	write(sock,buf,48);
	read(sock,buf,48);
	if(getenv("DEBUG")) write(1,buf,48);
	int32_t tm=(buf[32]<<24 | buf[33]<<16 | buf[34]<<8 | buf[35])-2208988800;
	// (  date -u -d "1900-01-01 00:00" +%s -> -2208988800 )
	printf("%d\n",tm);
}
