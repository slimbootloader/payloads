/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdint.h>
#include <string.h>
#include "mpconfigport.h"
#include "sblservice.h"

void *memcpy(void *dst, const void *src, size_t n) {
  CopyMem(dst, src, n);
}

void *memmove(void *dst, const void *src, size_t n) {
  CopyMem(dst, src, n);
}

void *memset(void *s, int c, size_t n) {
  SetMem(s, (UINTN)n, (UINT8)c);
}

int memcmp(const void *s1, const void *s2, size_t n) {
  return CompareMem(s1, s2, n);
}

size_t strlen(const char *str) {
  return AsciiStrLen(str);
}

int strcmp(const char *s1, const char *s2) {
  return AsciiStrCmp(s1, s2);
}

int strncmp(const char *s1, const char *s2, size_t n) {
  return AsciiStrnCmp(s1, s2, n);
}

char *strcat_s(char *dest, int dlen, const char *src) {
  AsciiStrCatS (dest, dlen, src);
  return dest;
}

char *strcpy_s(char *dest, int dlen, const char *src){
  AsciiStrCpyS (dest, dlen, src);
  return dest;
}

char *strchr(const char *s, int c)
{
  char pattern[2];
  pattern[0] = c;
  pattern[1] = 0;
  return AsciiStrStr (s, pattern);
}

char *strstr(const char *haystack, const char *needle) {
  return AsciiStrStr(haystack, needle);
}
