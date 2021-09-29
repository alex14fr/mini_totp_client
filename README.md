# mini_totp_client

(c) 2021 Alexandre Janon <alex14fr at gmail dot com>

A small TOTP SHA1 client written in C, depending only on libcrypto (OpenSSL)

Compile with:

```
$ ./compile
```

Test with: (see [RFC6238](https://datatracker.ietf.org/doc/html/rfc6238#appendix-B))

```
$ $ ./mini_totp_sha1 $(printf 12345678901234567890|base32) 30 8 1
94287082
```


