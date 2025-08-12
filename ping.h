#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define packed __attribute__((packed))

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;

#define $1 (int8 *)
#define $2 (int16)
#define $4 (int32)
#define $8 (int64)
#define $c (char *)
#define $i (int)

enum e_type { unassigned, echo, echoreply };
typedef enum e_type type;

// intermediate structure
struct s_rawicmp {
  int8 type;
  int8 code;
  int16 checksum;
  int8 data[];
} packed;

// handle packets via this abstraction, convert them to rawicmp
struct s_icmp {
  type kind;
  int16 size;
  int8 *data;
} packed;
typedef struct s_icmp icmp;

int16 checksum(int8 *, int16);


int main(int, char **);
void copy(int8 *, int8 *, int16);
// icmp
icmp *mkicmp(type, const int8 *, int16);
// convert to rawicmp struct, convert to serise of bytes
int8 *evalicmp(icmp *);
// for debug
void showicmp(icmp*);
void hexdump(int8*, int16);
void zero(int8*, int16);
