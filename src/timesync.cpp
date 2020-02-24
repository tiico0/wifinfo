/*
  NTP-TZ-DST (v2)
  NetWork Time Protocol - Time Zone - Daylight Saving Time

  This example shows:
  - how to read and set time
  - how to set timezone per country/city
  - how is local time automatically handled per official timezone definitions
  - how to change internal sntp start and update delay
  - how to use callbacks when time is updated

  This example code is in the public domain.
*/

// initial time (possibly given by an external RTC)
#define RTC_UTC_TEST 1510592825 // 1510592825 = Monday 13 November 2017 17:07:05 UTC

// This database is autogenerated from IANA timezone database
//    https://www.iana.org/time-zones
// and can be updated on demand in this repository
#include <TZ.h>

// "TZ_" macros follow DST change across seasons without source code change
// check for your nearest city in TZ.h

// espressif headquarter TZ
//#define MYTZ TZ_Asia_Shanghai

// example for "Not Only Whole Hours" timezones:
// Kolkata/Calcutta is shifted by 30mn
//#define MYTZ TZ_Asia_Kolkata

// example of a timezone with a variable Daylight-Saving-Time:
// demo: watch automatic time adjustment on Summer/Winter change (DST)
#define MYTZ TZ_Europe_Paris

////////////////////////////////////////////////////////

#include <ESP8266WiFi.h>
#include <Schedule.h>
#include <coredecls.h> // settimeofday_cb()

#include <sys/time.h> // struct timeval
#include <time.h>     // time() ctime()

#include <sntp.h> // sntp_servermode_dhcp()

#include "emptyserial.h"

// for testing purpose:
extern "C" int clock_gettime(clockid_t unused, struct timespec *tp);

////////////////////////////////////////////////////////

#define PTM(w)                \
    Serial.print(" " #w "="); \
    Serial.print(tm->tm_##w);

static void printTm(const char *what, const tm *tm)
{
    Serial.print(what);
    PTM(isdst);
    PTM(yday);
    PTM(wday);
    PTM(year);
    PTM(mon);
    PTM(mday);
    PTM(hour);
    PTM(min);
    PTM(sec);
}

void time_show()
{
    timeval tv;
    timespec tp;
    time_t now;
    uint32_t now_ms, now_us;

    gettimeofday(&tv, nullptr);
    clock_gettime(0, &tp);
    now = time(nullptr);
    now_ms = millis();
    now_us = micros();

    Serial.println();
    printTm("localtime:", localtime(&now));
    Serial.println();
    printTm("gmtime:   ", gmtime(&now));
    Serial.println();

    // time from boot
    Serial.print("clock:     ");
    Serial.print((uint32_t)tp.tv_sec);
    Serial.print("s / ");
    Serial.print((uint32_t)tp.tv_nsec);
    Serial.println("ns");

    // time from boot
    Serial.print("millis:    ");
    Serial.println(now_ms);
    Serial.print("micros:    ");
    Serial.println(now_us);

    // EPOCH+tz+dst
    Serial.print("gtod:      ");
    Serial.print((uint32_t)tv.tv_sec);
    Serial.print("s / ");
    Serial.print((uint32_t)tv.tv_usec);
    Serial.println("us");

    // EPOCH+tz+dst
    Serial.print("time:      ");
    Serial.println((uint32_t)now);

    // timezone and demo in the future
    Serial.printf("timezone:  %s\n", MYTZ);

    // human readable
    Serial.print("ctime:     ");
    Serial.print(ctime(&now));

#if LWIP_VERSION_MAJOR > 1
    // LwIP v2 is able to list more details about the currently configured SNTP servers
    for (int i = 0; i < SNTP_MAX_SERVERS; i++)
    {
        IPAddress sntp = *sntp_getserver(i);
        const char *name = sntp_getservername(i);
        if (sntp.isSet())
        {
            Serial.printf("sntp%d:     ", i);
            if (name)
            {
                Serial.printf("%s (%s) ", name, sntp.toString().c_str());
            }
            else
            {
                Serial.printf("%s ", sntp.toString().c_str());
            }
            Serial.printf("%s Reachability: %d\n", sntp.isV6() ? "IPv6" : "IPv4", sntp_getreachability(i));
        }
    }
#endif

    Serial.println();
}

static void time_is_set_scheduled()
{
    time_t now = time(nullptr);
    Serial.print("SNTP updated, time: ");
    Serial.print(ctime(&now));
}

void time_setup()
{
    // setup RTC time
    // it will be used until NTP server will send us real current time
    time_t rtc = RTC_UTC_TEST;
    timeval tv = {rtc, 0};
    timezone tz = {0, 0};
    settimeofday(&tv, &tz);

    // install callback - called when settimeofday is called (by SNTP or us)
    // once enabled (by DHCP), SNTP is updated every hour
    settimeofday_cb(time_is_set_scheduled);

    // NTP servers may be overriden by your DHCP server for a more local one
    // (see below)
    configTime(MYTZ, "pool.ntp.org");

    // OPTIONAL: disable obtaining SNTP servers from DHCP
    //sntp_servermode_dhcp(0); // 0: disable obtaining SNTP servers from DHCP (enabled by default)
}
