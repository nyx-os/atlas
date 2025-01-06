#pragma once
#include <stddef.h>
#include <stdint.h>

#if __has_include(<cstring>)
#include <cstring>
#else
extern "C" {
void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
}
namespace Atlas {

constexpr size_t strlen(const char *str) {
  size_t len = 0;
  while (*(str++)) {
    len++;
  }
  return len;
}

constexpr size_t strcmp(const char *a, const char *b) {
  while (*a && *b && *a == *b) {
    a++;
    b++;
  }
  return *a - *b;
}

constexpr bool isdigit(char c) { return c >= '0' && c <= '9'; }
constexpr bool tolower(char c) { return c >= 'A' && c <= 'Z' ? c + 32 : c; }

} // namespace Atlas

#endif
