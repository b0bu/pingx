#include "ping.h"
#include <stdlib.h>

int8 *evalicmp(icmp *pkt) {
  int8 *p, *ret;
  int16 size;
  int16 check;

  struct s_rawicmp rawpkt;
  struct s_rawicmp *rawptr;

  if (!pkt || !pkt->data)
    return $1 0;

  switch (pkt->kind) {
  case echo:
    rawpkt.type = 8;
    rawpkt.code = 0;

    break;
  case echoreply:
    rawpkt.type = 0;
    rawpkt.code = 0;

    break;
  default:
    return $1 0;
    break;
  }

  rawpkt.checksum = 0;
  size = sizeof(struct s_rawicmp) + pkt->size;

  // if size an odd number increase by 1, 16 bit boundary
  if (size % 2)
    size++;
  p = $1 malloc($i size);
  ret = p;
  assert(p);
  zero($1 p, pkt->size);
  // memset($1 p, 0, pkt->size);

  // copy headers
  copy(p, $1 & rawpkt, sizeof(struct s_icmp));
  // copy data
  p += sizeof(struct s_rawicmp);
  copy(p, pkt->data, pkt->size);

  check = checksum(ret, size);
  rawptr = (struct s_rawicmp *)ret;
  rawptr->checksum = check;

  return ret;
}

void zero(int8 *p, int16 s) { memset($1 p, 0, s); }

void copy(int8 *dst, int8 *src, int16 size) {
  int16 n;
  int8 *sptr, *dptr;

  // while n !0 decrement
  for (dptr = dst, sptr = src, n = size; n; n--) {
    // set equal and advance
    *dptr++ = *sptr++;
  }

  return;
}

void printhex(int8 *d, int16 s) {
  for (int i = 0; i < s; i++) {
    printf("%02X", *d++);
  }
}

void showicmp(icmp *pkt) {
  if (!pkt)
    return;

  printf("kind:\t %s\nsize:\t %d\n",
         (pkt->kind == echo) ? "echo" : "echo reply", $1 pkt->size);
  if (pkt->data)
    // final arg is delimiter
    printhex(pkt->data, pkt->size);
  printf("\n");
}

icmp *mkicmp(type kind, const int8 *data, int16 size) {
  int16 n;
  icmp *p;

  if (!data || !size) {
    return (icmp *)0;
  }

  // size of icmp struct headers + data
  n = sizeof(struct s_icmp) + size;
  p = (icmp *)malloc($i n);
  assert(p);

  memset(p, 0, sizeof(n));

  p->kind = kind;
  p->size = size;
  // is a pointer
  p->data = $1 data;

  return p;
}

int16 checksum(int8 *pkt, int16 size) {
  int16 *p;
  int32 acc, b;
  int16 carry;
  int16 n;
  int16 sum;

  acc = 0;

  for (n = size, p = (int16 *)pkt; n; n -= 2, p++) {
    b = *p;
    acc += b;
  }

  carry = ((acc & 0xffff0000) >> 16);
  sum = (acc & 0x0000ffff);

  return ~(sum + carry);
}

int main(int argc, char *argv[]) {

  int8 *str;
  int8 *raw;
  icmp *pkt;
  int16 size;

  // includes null terminator
  str = $1 malloc(6);
  assert(str);
  zero(str, $2 6);
  strncpy($c str, "Hello", 5);

  // 5 here cast to int16
  pkt = mkicmp(echo, str, $2 5);
  assert(pkt);
  showicmp(pkt);

  raw = evalicmp(pkt);
  assert(raw);
  size = sizeof(struct s_rawicmp) + pkt->size;
  printhex(raw, size);
  free(pkt->data);
  free(pkt);

  return 0;
}
