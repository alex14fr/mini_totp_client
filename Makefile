CFLAGS=-Wall -Os -D_POSIX_SOURCE
LDFLAGS=-s
LDLIBS=-lcrypto
BINS=sntpclient mini_totp_sha1

all: $(BINS)

clean:
	rm -f $(BINS)

