#include "rng.h"
#include <fxcg/rtc.h>

uint64_t s[2];

void init_xorshift128plus() {
	// Unique device ID
	s[0] = *(uint64_t*)0x8001FFD0;
	// RTC ticks
	s[1] = RTC_GetTicks();
}

uint64_t xorshift128plus()
{
	uint64_t s1 = s[0];
	uint64_t s0 = s[1];
	s[0] = s0;
	s1 ^= s1 << 23;
	s1 ^= s1 >> 18;
	s1 ^= s0;
	s1 ^= s0 >> 5;
	s[1] = s1;
	return s[0] + s[1];
}
