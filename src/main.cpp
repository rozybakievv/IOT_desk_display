#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

const int buttonPin = 25;
const int pageButtonPin = 32;
const int buzzerPin = 27;
int buttonState = 0;
int pageButtonState = 0;
int lastPageButtonState = 0;
int lastButtonState = 0;
const unsigned long holdtime = 1000; // 1 sec

TFT_eSPI tft = TFT_eSPI();

enum PageState {
    PAGE_POMODORO,
	PAGE_WEATHER,
    PAGE_BREATHING
};

const int PAGE_COUNT = 3;

PageState currentPage = PAGE_POMODORO;

void nextPage()
{
    currentPage = (PageState)((currentPage + 1) % PAGE_COUNT);
    tft.fillScreen(TFT_BLACK);  // clear once on page change
}

bool pageButtonPressed()
{
    return (pageButtonState == HIGH && lastPageButtonState == LOW);
}

bool weatherDrawn = false;
bool breathingDrawn = false;

String formatTime(unsigned int mm, unsigned int ss)
{
    String result = "";

    if (mm < 10) result += "0";
    result += String(mm);
    result += ":";

    if (ss < 10) result += "0";
    result += String(ss);

    return result;
}

void clearMiddleScreen() {
	tft.fillRect(0, 20, 160, 100, TFT_BLACK); // Clear area to not leave pixels
}

// -------- Pomodoro -------- //
enum PomodoroState {
    SET_TIMER,
	SET_BREAK,
    RUNNING,
	BREAK,
    DONE
};

// Buzzer
unsigned long buzz_amount = 0;
const unsigned long buzz_max = 3;
unsigned long buzz_start = 0;
unsigned long buzz_end = 500; // 0.5s

// Timer
const unsigned long timer_increment = 10; // In minutes
unsigned long max_timer = 180;
const unsigned long break_increment = 5; // In minutes
bool doneInitialized = false;

// Pomodoro variables
PomodoroState pomo_state = SET_TIMER;
unsigned long pomodoro_timer[2] = {0, 0};
unsigned long start_time = 0;
String time_spent_s;

unsigned long lastDisplayUpdate = 0;
const unsigned long DISPLAY_INTERVAL = 1000; // 1 second
unsigned long pressStartTime = 0;

long lastDrawnTimer = -1;
long lastDrawnBreak = -1;

bool titleTimerSet = false;
bool titleBreakSet = false;
bool titleRunningSet = false;
bool titleRunningBreakSet = false;

// Functions
void handleSetTimer() {
	// Draw title
	if (!titleTimerSet)
	{
		tft.setTextColor(TFT_PURPLE, TFT_BLACK);
		tft.setTextDatum(MC_DATUM);
		tft.drawString("Set Timer", 65, 100, 1);

		titleTimerSet = true;
	}

	// Start hold timer
	if (buttonState == HIGH && lastButtonState == LOW)
	{
		pressStartTime = millis();
	}

	// Check if selected (held) or if increment (pressed)
	if (buttonState == LOW && lastButtonState == HIGH)
	{
		unsigned long pressDuration = millis() - pressStartTime;

		if (pressDuration <= holdtime)
		{
			if (pomodoro_timer[0] + timer_increment > max_timer)
			{
				Serial.println("Reset timer");
				pomodoro_timer[0] = 0; // Reset timer
			} else {
				Serial.println("Increments");
				pomodoro_timer[0] += timer_increment;
			}
			
			Serial.println(pomodoro_timer[0]);
		} else if (pressDuration >= holdtime) {
			Serial.println("Continue to Set Timer");
			clearMiddleScreen();
			start_time = millis();
			lastDrawnBreak = -1;
			titleTimerSet = false;
			pomo_state = SET_BREAK;
		}
	}

	// Redraw time and title each increment
	if (pomodoro_timer[0] != lastDrawnTimer)
	{
		lastDrawnTimer = pomodoro_timer[0];

		titleTimerSet = false; // Redraw title

		unsigned long remainingSeconds = pomodoro_timer[0] * 60;
		unsigned int mm = remainingSeconds / 60;
		unsigned int ss = remainingSeconds % 60;

		String timeStr = formatTime(mm, ss);

		clearMiddleScreen();
		tft.setTextColor(TFT_BLUE, TFT_BLACK);
		tft.setTextDatum(MC_DATUM); // Middle Center
		tft.drawString(timeStr, 65, 75, 1); // Found center
	}
}

void handleSetBreak() {
	// Draw title
	if (!titleBreakSet)
	{
		tft.setTextColor(TFT_PURPLE, TFT_BLACK);
		tft.setTextDatum(MC_DATUM);
		tft.drawString("Set Break Timer", 65, 100, 1);

		titleBreakSet = true;
	}

	// Start button hold timer
	if (buttonState == HIGH && lastButtonState == LOW)
	{
		pressStartTime = millis();
	}

	// Check if selected (held) or if increment (press)
	if (buttonState == LOW && lastButtonState == HIGH)
	{
		unsigned long pressDuration = millis() - pressStartTime;

		if (pressDuration <= holdtime)
		{
			if (pomodoro_timer[1] + break_increment > max_timer)
			{
				Serial.println("Reset timer");
				pomodoro_timer[1] = 0; // Reset timer
			} else {
				Serial.println("Increments");
				pomodoro_timer[1] += break_increment;
			}
			
			Serial.println(pomodoro_timer[1]);
		} else if (pressDuration >= holdtime) {
			Serial.println("Timer started");
			clearMiddleScreen();
			start_time = millis();
			titleBreakSet = false;
			pomo_state = RUNNING;
		}
	}

	// Redraw each increment
	if (pomodoro_timer[1] != lastDrawnBreak)
	{
		lastDrawnBreak = pomodoro_timer[1];

		titleBreakSet = false; // Redraw title

		unsigned long remainingSeconds = pomodoro_timer[1] * 60;
		unsigned int mm = remainingSeconds / 60;
		unsigned int ss = remainingSeconds % 60;

		String timeStr = formatTime(mm, ss);

		clearMiddleScreen();
		tft.setTextColor(TFT_BLUE, TFT_BLACK);
		tft.setTextDatum(MC_DATUM);
		tft.drawString(timeStr, 65, 75, 1);
	}
}

void handleRunningTimer() {
	unsigned long now = millis();
	unsigned long elapsed = (now - start_time)/1000; // In seconds

	// Draw title
	if (!titleRunningSet)
	{
		tft.setTextColor(TFT_PURPLE, TFT_BLACK);
		tft.setTextDatum(MC_DATUM);
		tft.drawString("Focus", 65, 100, 1);

		titleRunningSet = true;
	}

	if (elapsed >= pomodoro_timer[0] * 60) // Target time reached
	{
		pomodoro_timer[0] = 0;
		start_time = now;
		lastDisplayUpdate = -1;
		titleRunningSet = false;
		titleRunningBreakSet = false;
		pomo_state = BREAK;

		clearMiddleScreen();
	}

	// Display remaining timer time
	if (now - lastDisplayUpdate >= DISPLAY_INTERVAL) 
	{
		lastDisplayUpdate = now;

		titleRunningSet = false; // Redraw title

		unsigned long remainingSeconds = pomodoro_timer[0] * 60 - elapsed;
		unsigned int mm = remainingSeconds / 60;
		unsigned int ss = remainingSeconds % 60;

		String timeStr = formatTime(mm, ss);

		clearMiddleScreen();
		tft.setTextColor(TFT_BLUE, TFT_BLACK);
		tft.setTextDatum(MC_DATUM); // Middle Center
		tft.drawString(timeStr, 65, 75, 1); // Found center
	}

	// Start hold timer
	if (buttonState == HIGH && lastButtonState == LOW)
	{
		pressStartTime = millis();
	}

	// Check if button held > reset and go back to setting timer
	if (buttonState == LOW && lastButtonState == HIGH)
	{
		unsigned long pressDuration = millis() - pressStartTime;
		
		if (pressDuration >= holdtime) {
			Serial.println("Going back to set timer");
			clearMiddleScreen();
			doneInitialized = false;
			lastDrawnTimer = -1;
			lastDrawnBreak = -1;
			titleTimerSet = false;
			titleBreakSet = false;
			pomodoro_timer[0] = 0;
			pomodoro_timer[1] = 0;
			pomo_state = SET_TIMER;
		}
	}
}

void handleRunningBreak() {
	unsigned long now = millis();
	unsigned long elapsed = (now - start_time)/1000; // In seconds

	// Draw title
	if (!titleRunningBreakSet)
	{
		tft.setTextColor(TFT_PURPLE, TFT_BLACK);
		tft.setTextDatum(MC_DATUM);
		tft.drawString("Break Time !", 65, 100, 1);

		titleRunningBreakSet = true;
	}

	if (elapsed >= pomodoro_timer[1] * 60) // Target time reached
	{
		Serial.println("Break completed");
		pomodoro_timer[1] = 0;
		start_time = 0;
		lastDisplayUpdate = -1;
		pomo_state = DONE;

		clearMiddleScreen();
	}

	if (now - lastDisplayUpdate >= DISPLAY_INTERVAL) 
	{
		lastDisplayUpdate = now;

		unsigned long remainingSeconds = pomodoro_timer[1] * 60 - elapsed;
		unsigned int mm = remainingSeconds / 60;
		unsigned int ss = remainingSeconds % 60;

		String timeStr = formatTime(mm, ss);

		clearMiddleScreen();
		tft.setTextColor(TFT_BLUE, TFT_BLACK);
		tft.setTextDatum(MC_DATUM); // Middle Center
		tft.drawString(timeStr, 65, 75, 1); // Found center

		titleRunningBreakSet = false; // Redraw title
	}

	// Start hold timer
	if (buttonState == HIGH && lastButtonState == LOW)
	{
		pressStartTime = millis();
	}

	// Check if button held > reset and go back to setting timer
	if (buttonState == LOW && lastButtonState == HIGH)
	{
		unsigned long pressDuration = millis() - pressStartTime;
		
		if (pressDuration >= holdtime) {
			Serial.println("Going back to set timer");
			clearMiddleScreen();
			doneInitialized = false;
			lastDrawnTimer = -1;
			lastDrawnBreak = -1;
			titleTimerSet = false;
			titleBreakSet = false;
			pomodoro_timer[0] = 0;
			pomodoro_timer[1] = 0;
			pomo_state = SET_TIMER;
		}
	}
}

void handleDone() {
	unsigned long now = millis();

	if (!doneInitialized)
    {
        doneInitialized = true;
        buzz_start = now;
        buzz_amount = 0;
        digitalWrite(buzzerPin, HIGH);
    }

	if (now - buzz_start >= buzz_end)
	{
		buzz_start = now;

        if (digitalRead(buzzerPin) == HIGH)
        {
            digitalWrite(buzzerPin, LOW);
            buzz_amount++;
        }
        else
        {
            digitalWrite(buzzerPin, HIGH);
        }
	}
	
	if (buzz_amount >= buzz_max)
	{
		Serial.println("BUZZ BUZZ BUZZ - Timer completed");
		clearMiddleScreen();
		digitalWrite(buzzerPin, LOW);
		buzz_amount = 0;
		doneInitialized = false;
		lastDrawnTimer = -1;
		pomo_state = SET_TIMER;
	}
}

void handlePomodoroPage()
{
    switch (pomo_state)
    {
        case SET_TIMER:
            handleSetTimer();
            break;

        case SET_BREAK:
            handleSetBreak();
            break;

        case RUNNING:
            handleRunningTimer();
            break;

        case BREAK:
            handleRunningBreak();
            break;

        case DONE:
            handleDone();
            break;
    }
}

// -------- Weather Page -------- //
void handleWeatherPage()
{
    if (!weatherDrawn)
    {
        tft.setTextColor(TFT_CYAN, TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("Weather", 65, 60, 2);
        tft.drawString("22°C  Sunny", 65, 90, 1);
        weatherDrawn = true;
    }
}


// -------- Breathing Page -------- //
void handleBreathingPage()
{
    if (!breathingDrawn)
    {
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("Breathe", 65, 60, 2);
        tft.drawString("Inhale... Exhale...", 65, 90, 1);
        breathingDrawn = true;
    }
}


void setup() {
	Serial.begin(115200);
	delay(100);

	// Initialize the pushbutton pin as an input:
	pinMode(buttonPin, INPUT);

	// Initialize button
	pinMode(pageButtonPin, INPUT);

	// Init buzzer
	pinMode(buzzerPin, OUTPUT);
	digitalWrite(buzzerPin, LOW); // off

	tft.init();
	tft.setRotation(0);   // 0–3 if orientation is wrong
	tft.fillScreen(TFT_BLACK);

	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.setTextSize(1);
	tft.setCursor(0, 0);
	tft.println("Pomodoro timer");

	tft.setTextColor(TFT_BLUE, TFT_BLACK);
	tft.setTextDatum(MC_DATUM); // Middle Center
	tft.drawString("0.00", 65, 75, 1); // Found center
}

void loop() {
  	buttonState = digitalRead(buttonPin);
	pageButtonState = digitalRead(pageButtonPin);

	if (pageButtonPressed())
    {
        nextPage();
        tft.fillScreen(TFT_BLACK);

        // reset UI flags
        titleTimerSet = false;
        titleBreakSet = false;
        titleRunningSet = false;
        titleRunningBreakSet = false;

		weatherDrawn = false;
		breathingDrawn = false;

		lastDrawnTimer = -1;
	}

    switch (currentPage)
    {
        case PAGE_POMODORO:
            handlePomodoroPage();
            break;

        case PAGE_WEATHER:
            handleWeatherPage();
            break;

        case PAGE_BREATHING:
            handleBreathingPage();
            break;
    }
	
	lastButtonState = buttonState;
	lastPageButtonState = pageButtonState;
}