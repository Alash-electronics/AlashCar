// Имитация Arduino.h для тестов машинки: уровни пинов, ШИМ, время и Stream с очередью символов.
#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <string>
class __FlashStringHelper;
#define F(x) (reinterpret_cast<const __FlashStringHelper *>(x))
#define OUTPUT 1
#define LOW 0
#define HIGH 1
using std::max;
inline uint32_t g_ms = 1000;
inline uint32_t millis() { return g_ms; }
inline void delay(uint32_t ms) { g_ms += ms; }
inline uint8_t g_level[40], g_pwm[40];
inline int g_pwmWrites = 0;
inline void pinMode(uint8_t, uint8_t) {}
inline void digitalWrite(uint8_t p, uint8_t v) { g_level[p] = v; }
inline void analogWrite(uint8_t p, int v) { g_pwm[p] = (uint8_t)v; g_pwmWrites++; }
class Print {
 public:
  void print(const char *s) { printf("%s", s); }
  void println(const char *s = "") { printf("%s\n", s); }
  void println(const __FlashStringHelper *s) { printf("%s\n", (const char *)s); }
};
class Stream : public Print {
 public:
  std::string buf; size_t pos = 0;
  int available() { return (int)(buf.size() - pos); }
  int read() { return pos < buf.size() ? buf[pos++] : -1; }
};
