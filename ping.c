#include "ping.h"
#include <stdlib.h>

uint8 *evalicmp(icmp *pkt) {
  uint8 *p, *ret;
  uint16 size;
  uint16 check;

  struct s_rawicmp rawpkt;
  struct s_rawicmp *rawptr;

  if (!pkt || !pkt->data)
    return (uint8 *)0;

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
    return (uint8 *)0;
    break;
  }

  rawpkt.checksum = 0;
  size = sizeof(struct s_rawicmp) + pkt->size;

  // if size an odd number increase by 1, 16 bit boundary
  if (size % 2)
    size++;

  p = malloc((uint8)size);
  ret = p;
  assert(p);
  zero((uint8 *)p, pkt->size);
  // memset($1 p, 0, pkt->size);

  // copy headers
  copy(p, (uint8 *)&rawpkt, sizeof(struct s_icmp));
  // copy data
  p += sizeof(struct s_rawicmp);
  copy(p, pkt->data, pkt->size);

  check = checksum(ret, size);
  rawptr = (struct s_rawicmp *)ret;
  rawptr->checksum = check;

  return ret;
}

void zero(uint8 *p, uint16 s) { memset((uint8 *)p, 0, s); }

void copy(uint8 *dst, uint8 *src, uint16 size) {
  uint16 n;
  uint8 *sptr, *dptr;

  // while n !0 decrement
  for (dptr = dst, sptr = src, n = size; n; n--) {
    // set equal and advance
    *dptr++ = *sptr++;
  }

  return;
}

void printhex(uint8 *d, uint16 s) {
  for (int i = 0; i < s; i++) {
    printf("%02x", *d++);
  }
}

void showicmp(icmp *pkt) {
  if (!pkt)
    return;

  printf("kind:\t %s\nsize:\t %d\n",
         (pkt->kind == echo) ? "echo" : "echo reply", (uint8)pkt->size);
  if (pkt->data)
    // final arg is delimiter
    printhex(pkt->data, pkt->size);
  printf("\n");
}

icmp *mkicmp(type kind, const uint8 *data, uint16 size) {
  uint16 n;
  icmp *p;

  if (!data || !size) {
    return (icmp *)0;
  }

  // size of icmp struct headers + data
  n = sizeof(struct s_icmp) + size;
  p = (icmp *)malloc((int)n);
  assert(p);

  memset(p, 0, sizeof(n));

  p->kind = kind;
  p->size = size;
  // is a pointer
  p->data = (uint8 *)data;

  return p;
}

uint16 checksum(uint8 *pkt, uint16 size) {
  uint16 *p;
  uint32 acc, b;
  uint16 carry;
  uint16 n;
  uint16 sum;

  acc = 0;

  for (n = size, p = (uint16 *)pkt; n; n -= 2, p++) {
    b = *p;
    acc += b;
  }

  carry = ((acc & 0xffff0000) >> 16);
  sum = (acc & 0x0000ffff);

  return ~(sum + carry);
}

int main(int argc, char *argv[]) {

  uint8 *str;
  uint8 *raw;
  icmp *pkt;
  uint16 size;

  // includes null terminator
  str = (uint8 *)malloc(6);
  assert(str);
  zero(str, (uint16)6);
  strncpy((char *)str, "Hello", 5);

  // 5 here cast to int16
  pkt = mkicmp(echo, str, (uint16)5);
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
