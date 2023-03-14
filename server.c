#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


//#include <sys/types.h>
//#include <sys/wait.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <signal.h>
//#include <poll.h>



#include "aes.h"

unsigned char key[16];
AES_KEY expanded;
unsigned char zero[16];
unsigned char scrambledzero[16];


int size;


double packets;
double ttotal;
double t[16][256];
double tsq[16][256];
long long tnum[16][256];
double u[16][256];
double udev[16][256];
char n[16];


void tally(double timing) {
	int j;
  	int b;
  	for (j = 0; j < 16; ++j) {
    	b = 255 & (int) n[j];
    	++packets;
    	ttotal += timing;
    	t[j][b] += timing;
    	tsq[j][b] += timing * timing;
    	tnum[j][b] += 1;
  	}
}

/*
//For RISCV platform
unsigned int timestamp(void) {
	unsigned long cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}
*/

// for x86 platform
unsigned int timestamp(void) {
	unsigned int bottom;
	unsigned int top;
	asm volatile(".byte 15;.byte 49": "=a"(bottom), "=d"(top));
	return bottom;
}

void handle(char out[40], char in [], int len) {
  unsigned char workarea[len * 3];
  int i;
  for (i = 0; i < 40; ++i) out[i] = 0;
  *(unsigned int * )(out + 32) = timestamp();
  if (len < 16) return;
  for (i = 0; i < 16; ++i) out[i] = in [i];
  for (i = 16; i < len; ++i) workarea[i] = in [i];
  AES_encrypt( in , workarea, & expanded);
  /* a real server would now check AES-based authenticator, */
  /* process legitimate packets, and generate useful output */
  for (i = 0; i < 16; ++i) out[16 + i] = scrambledzero[i];
  *(unsigned int * )(out + 36) = timestamp();
}


void studyinput(void) {
	int i, j;
  	char packet[2048];
  	//char response[40];
  	char out[40];

  	for (;;) {
  		if (size < 16) continue;
    	if (size > sizeof packet) continue;
    	/* a mediocre PRNG is sufficient here */
    	for (j = 0; j < size; ++j) packet[j] = random();
    	for (j = 0; j < 16; ++j) n[j] = packet[j];
	
	handle(out, packet, size);

	//printf("%d\n", strlen(out));

    	if (!memcmp(packet, out, 16)) {
          	unsigned int timing;
          	timing = * (unsigned int * )(out + 36);
          	timing -= * (unsigned int * )(out + 32);
         	
         	if (timing < 10000) {
            // clip tail to reduce noise 
            	tally(timing);
            	return;
          }
        }
  	}
}

void printpatterns(void) {
	int j;
  	int b;
  	double taverage;
  	taverage = ttotal / packets;
  	for (j = 0; j < 16; ++j)
  		for (b = 0; b < 256; ++b) {
  			u[j][b] = t[j][b] / tnum[j][b];
      		udev[j][b] = tsq[j][b] / tnum[j][b];
      		udev[j][b] -= u[j][b] * u[j][b];
      		udev[j][b] = sqrt(udev[j][b]);
      	}
    for (j = 0; j < 16; ++j) {
    	for (b = 0; b < 256; ++b) {
    		printf("%2d %4d %3d %lld %.3f %.3f %.3f %.3f\n", j, size, b, tnum[j][b], u[j][b], udev[j][b], u[j][b] - taverage, udev[j][b] / sqrt(tnum[j][b]));
    	}
    }

    fflush(stdout);
}


int timetoprint(long long inputs) {
	if (inputs < 10000) return 0;
	if (!(inputs & (inputs - 1))) return 1;

	return 0;
}


int main(int argc, char **argv)
{
	int i;

	if (read(0, key, sizeof key) < sizeof key) 
		return 111;

	//printf("%ld\n",  sizeof key);
	//for (i=0; i<16; i++)
	///	printf("%d ", key[i]);
	//printf("\n");
	AES_set_encrypt_key(key, 128, &expanded);
	//printf("Error key = %d\n", i);
	//printf("Rounds = %d\n", expanded.rounds);
	AES_encrypt(zero, scrambledzero, &expanded);

	//printf("%s\n", zero);
	//printf("%s\n", scrambledzero);

	long long inputs = 0;
	long long temp = 0;

	if (!argv[1])
		return 100;

	size = atoi(argv[1]);

	//printf("Size = %d\n", size);

	for (;;) {
    		studyinput();
    		++inputs;
		++temp;
    	
		//if (timetoprint(inputs)) printpatterns();
		
		if (temp == (1 << 15)) {
			printf("%lld\n", inputs);
			printpatterns();
			temp = 0;
		}

		if (inputs >= (1 << 27)) break;
	}

	return 0;
}
