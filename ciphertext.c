//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <poll.h>

#include "aes.h"

unsigned char key[16];
AES_KEY expanded;
unsigned char zero[16];
unsigned char scrambledzero[16];


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


int main(int argc, char ** argv) {

    if (read(0, key, sizeof key) < sizeof key) 
        return 111;

    AES_set_encrypt_key(key, 128, &expanded);
    AES_encrypt(zero, scrambledzero, &expanded);

    //struct sockaddr_in server;
    int s;
    char packet[40];
    //char response[40];
    //struct pollfd p;
    int j;
    //long int random_num;
    //if (!argv[1]) return 100;
    
    //if (!inet_aton(argv[1], & server.sin_addr)) return 100;
    //server.sin_family = AF_INET;
    //server.sin_port = htons(10000);
    //while ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1) sleep(1);
    //while (connect(s, (struct sockaddr * ) & server, sizeof server) == -1) sleep(1);

    char out[40];

    for (;;) {
        for (j = 0; j < sizeof packet; ++j) {
          	//random_num = random();
          	packet[j] = random();
		  	//printf("%ld ", random_num);
          	//printf("%d ", packet[j]);
        }
        //printf("\n");
        handle(out, packet, sizeof packet);

        if (!memcmp(packet, out, 16)) {
            for (j = 0; j < 16; ++j)
              printf("%02x ", 255 & (unsigned int) out[j + 16]);
            printf("\n");
            return 0;
        }
    }
}
