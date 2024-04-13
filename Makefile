CFLAGS=-Wall -Os -D_POSIX_SOURCE
#LDFLAGS=-s
#LDLIBS=-lcrypto
BINS=sntpclient mini_totp_sha1

.PHONY: all clean

all: $(BINS)

mini_totp_sha1: mini_totp_sha1.c sha1.c

clean:
	rm -f $(BINS)

