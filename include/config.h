#pragma once

// -------- Pins -------- //
#define BUTTON_PIN          25
#define PAGE_BUTTON_PIN     32
#define BUZZER_PIN          27

// -------- Display -------- //
#define DISPLAY_WIDTH       160
#define DISPLAY_HEIGHT      128
#define DISPLAY_CENTER_X    65
#define DISPLAY_CENTER_Y    75      
#define DISPLAY_TITLE_Y     100     // Y position for page titles

// -------- Timing -------- //
#define HOLD_TIME           1000    // select button hold duration (ms)
#define DISPLAY_INTERVAL    1000    // screen refresh rate (ms)

// -------- Pomodoro -------- //
#define TIMER_INCREMENT     1      // work timer increment (minutes)
#define BREAK_INCREMENT     1       // break timer increment (minutes)
#define MAX_TIMER           180     // max time (minutes) 

// -------- Buzzer -------- //
#define BUZZ_MAX            3       // # of buzzes when timer complete
#define BUZZ_DURATION       500     // (ms)

// -------- Weather -------- //
#define WEATHER_LOCATION    "Montreal"
#define API_CALL_INTERVAL   30      // minutes - how often to refresh