#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define packed __attribute__((packed))

typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned int uint32;
typedef unsigned long long int uint64;

enum e_type { unassigned, echo, echoreply };
typedef enum e_type type;

// intermediate structure
struct s_rawicmp {
  uint8 type;
  uint8 code;
  uint16 checksum;
  uint8 data[];
} packed;

// handle packets via this abstraction, convert them to rawicmp
struct s_icmp {
  type kind;
  uint16 size;
  uint8 *data;
} packed;
typedef struct s_icmp icmp;

uint16 checksum(uint8 *, uint16);

int main(int, char **);
void copy(uint8 *, uint8 *, uint16);
// icmp
icmp *mkicmp(type, const uint8 *, uint16);
// convert to rawicmp struct, convert to serise of bytes
uint8 *evalicmp(icmp *);
// for debug
void showicmp(icmp*);
void hexdump(uint8*, uint16);
void zero(uint8*, uint16);
