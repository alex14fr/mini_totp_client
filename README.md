# mini_totp_client

(c) 2021 Alexandre Janon <alex14fr at gmail dot com>

A small HOTP/TOTP SHA1 client written in C99, without dependencies.

Compile with `make`.

Usage:

```
./mini_totp_sha1 <base32 secret> [<interval=30>] [<digits=6>] [<UNIX timestamp=now>]

To use HOTP codes, set interval to 1 and UNIX timestamp to the HOTP counter.
```

Test with: (see [RFC6238 Appendix B](https://datatracker.ietf.org/doc/html/rfc6238#appendix-B))

```
$ ./mini_totp_sha1 $(printf 12345678901234567890|base32) 30 8 59
94287082
```

For convenience we include a tiny and rough Simple Network Time Protocol ([RFC4330](https://datatracker.ietf.org/doc/html/rfc4330)) client which echoes UNIX timestamp given by server pool.ntp.org.

SNTP and TOTP are composed by the included shell script `sntp_totp_sha1` :

```
$ ./sntp_totp_sha1 $(printf 12345678901234567890|base32) 30 8
95142869
```



