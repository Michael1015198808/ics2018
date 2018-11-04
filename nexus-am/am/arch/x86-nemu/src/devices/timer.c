#include <am.h>
#include <x86.h>
#include <amdev.h>

#define RTC_PORT 0x48   // Note that this is not the standard

int printf(char*,...);
static _UptimeReg boot_time;

size_t timer_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_TIMER_UPTIME: {
			_UptimeReg *uptime = (_UptimeReg *)buf;
			static uint32_t last_hi=0,last_io=0;
      uptime->lo = inl(RTC_PORT);
			if(uptime->lo<last_io){
							++last_hi;
			}
			last_io=uptime->lo;
			uptime->hi=last_hi;
			if(uptime->lo<boot_time.lo){
							uptime->hi--;
			}
			uptime->lo-=boot_time.lo;
			//printf("%u\n",uptime->lo);
      return sizeof(_UptimeReg);
    }
    case _DEVREG_TIMER_DATE: {
      _RTCReg *rtc = (_RTCReg *)buf;
      rtc->second = 0;
      rtc->minute = 0;
      rtc->hour   = 0;
      rtc->day    = 0;
      rtc->month  = 0;
      rtc->year   = 2018;
      return sizeof(_RTCReg);
    }
  }
  return 0;
}

void timer_init() {
				boot_time.hi=boot_time.lo=0;
				boot_time.lo=inl(RTC_PORT);
				//Not true initialization
}

/*
#include <am.h>
#include <x86.h>
#include <amdev.h>

size_t timer_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_TIMER_UPTIME: {
      _UptimeReg *uptime = (_UptimeReg *)buf;
      uptime->hi = 0;
      uptime->lo = 0;
      return sizeof(_UptimeReg);
    }
    case _DEVREG_TIMER_DATE: {
      _RTCReg *rtc = (_RTCReg *)buf;
      rtc->second = 0;
      rtc->minute = 0;
      rtc->hour   = 0;
      rtc->day    = 0;
      rtc->month  = 0;
      rtc->year   = 2018;
      return sizeof(_RTCReg);
    }
  }
  return 0;
}

void timer_init() {
}
*/
