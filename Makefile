CC= gcc
AR= gcc-ar r

#CC= /home/danfeng/cache/installs/riscv/toolchain/bin/riscv64-unknown-elf-gcc
#AR= /home/danfeng/cache/installs/riscv/toolchain/bin/riscv64-unknown-elf-gcc-ar r

LIBOBJ=aes_core.o aes_misc.o aes_ecb.o aes_cbc.o aes_cfb.o aes_ofb.o aes_ctr.o

all: server correlate search

server: server.o libcrypto.a #libmylib.a is the dependency for the executable
	$(CC) -o server server.o -lm -L. -lcrypto

study: study.o libcrypto.a #libmylib.a is the dependency for the executable
	$(CC) -o study study.o -lm -L. -lcrypto

ciphertext: ciphertext.o libcrypto.a #libmylib.a is the dependency for the executable
	$(CC) -o ciphertext ciphertext.o -lm -L. -lcrypto

correlate: correlate.o
	$(CC) -o correlate correlate.o -lm

search: search.o libcrypto.a #libmylib.a is the dependency for the executable
	$(CC) -o search search.o -L. -lcrypto

server.o: server.c
	$(CC) -O3 -c server.c

study.o: study.c
	$(CC) -O3 -c study.c

ciphertext.o: ciphertext.c
	$(CC) -O3 -c ciphertext.c

correlate.o: correlate.c
	$(CC) -O3 -c correlate.c

search.o: search.c
	$(CC) -O3 -c search.c

aes_cbc.o: aes_cbc.c opensslconf.h aes_locl.h
	$(CC) -O3 -c aes_cbc.c

aes_cfb.o: aes_cfb.c opensslconf.h aes_locl.h
	$(CC) -c aes_cfb.c

aes_core.o: aes_core.c opensslconf.h aes_locl.h
	$(CC) -c aes_core.c

aes_ctr.o: aes_ctr.c opensslconf.h aes_locl.h
	$(CC) -c aes_ctr.c

aes_ecb.o: aes_ecb.c opensslconf.h aes_locl.h
	$(CC) -c aes_ecb.c

aes_misc.o: aes_misc.c opensslv.h aes_locl.h 
	$(CC) -c aes_misc.c

aes_ofb.o: aes_ofb.c opensslconf.h aes_locl.h
	$(CC) -c aes_ofb.c

libcrypto.a: $(LIBOBJ)
	$(AR) libcrypto.a $(LIBOBJ)

libs: libcrypto.a

clean:
	rm -f server correlate search *.o *.a *.gch #This way is cleaner than your clean
