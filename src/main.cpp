#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

const int buttonPin = 25;
const int buzzerPin = 27;
int buttonState = 0;
int lastButtonState = 0;
const unsigned long holdtime = 1000; // 1 sec

TFT_eSPI tft = TFT_eSPI();

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

// -------- Pomodoro -------- //
enum PomodoroState {
    SET_TIMER,
	SET_BREAK,
    RUNNING,
    DONE
};

const unsigned long timer_increment = 1; // In minutes
const unsigned long break_increment = 5; // In minutes
unsigned long buzz_amount = 0;
const unsigned long buzz_max = 3;
unsigned long buzz_start = 0;
unsigned long buzz_end = 1000; // 1s
bool doneInitialized = false;
bool break_time = false;

PomodoroState pomo_state = SET_TIMER;
unsigned long pomodoro_timer[2] = {0, 0};
unsigned long start_time = 0;
String time_spent_s;
unsigned long max_timer = 180;
unsigned long lastDisplayUpdate = 0;
const unsigned long DISPLAY_INTERVAL = 1000; // 1 second
unsigned long pressStartTime = 0;

long lastDrawnTimer = -1;
long lastDrawnBreak = -1;

void handleTimer() {
	if (buttonState == HIGH && lastButtonState == LOW)
	{
		// Start hold timer
		pressStartTime = millis();
	}

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
			start_time = millis();
			lastDrawnBreak = -1;
			pomo_state = SET_BREAK;
		}
	}

	// TODO: Draw title -> set main timer

	if (pomodoro_timer[0] != lastDrawnTimer)
	{
		lastDrawnTimer = pomodoro_timer[0];

		unsigned long remainingSeconds = pomodoro_timer[0] * 60;
		unsigned int mm = remainingSeconds / 60;
		unsigned int ss = remainingSeconds % 60;

		String timeStr = formatTime(mm, ss);

		tft.fillRect(0, 40, 160, 40, TFT_BLACK); // Clear area to not leave pixels
		tft.setTextColor(TFT_BLUE, TFT_BLACK);
		tft.setTextDatum(MC_DATUM); // Middle Center
		tft.drawString(timeStr, 65, 75, 1); // Found center
	}
}

void handleBreak() {
	if (buttonState == HIGH && lastButtonState == LOW)
	{
		// Start hold timer
		pressStartTime = millis();
	}

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
			start_time = millis();
			pomo_state = RUNNING;
		}
	}

	// TODO: Draw title -> set break timer

	if (pomodoro_timer[1] != lastDrawnBreak)
	{
		lastDrawnBreak = pomodoro_timer[1];
		
		unsigned long remainingSeconds = pomodoro_timer[1] * 60;
		unsigned int mm = remainingSeconds / 60;
		unsigned int ss = remainingSeconds % 60;

		String timeStr = formatTime(mm, ss);

		tft.fillRect(0, 40, 160, 40, TFT_BLACK); // Clear area to not leave pixels
		tft.setTextColor(TFT_BLUE, TFT_BLACK);
		tft.setTextDatum(MC_DATUM); // Middle Center
		tft.drawString(timeStr, 65, 75, 1); // Found center
	}
}

void handleRunning() {
	unsigned long now = millis();
	unsigned long elapsed = (now - start_time)/1000; // In seconds

	if (elapsed >= pomodoro_timer[0] * 60) // Target time reached
	{
		pomodoro_timer[0] = 0;
		pomodoro_timer[1] = 0;
		start_time = 0;
		pomo_state = DONE;

		tft.fillRect(0, 40, 160, 40, TFT_BLACK); // Clear area to not leave pixels
		tft.setTextColor(TFT_BLUE, TFT_BLACK);
		tft.setTextDatum(MC_DATUM); // Middle Center
		tft.drawString("0", 65, 75, 1); // Found center
	} 
	

	// Display remaining timer time
	if (!break_time)
	{
		if (now - lastDisplayUpdate >= DISPLAY_INTERVAL) 
		{
			lastDisplayUpdate = now;

			unsigned long remainingSeconds = pomodoro_timer[0] * 60 - elapsed;
			unsigned int mm = remainingSeconds / 60;
			unsigned int ss = remainingSeconds % 60;

			String timeStr = formatTime(mm, ss);

			tft.fillRect(0, 40, 160, 40, TFT_BLACK); // Clear area to not leave pixels
			tft.setTextColor(TFT_BLUE, TFT_BLACK);
			tft.setTextDatum(MC_DATUM); // Middle Center
			tft.drawString(timeStr, 65, 75, 1); // Found center
		}
	} else {
		// Display remaning break time
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
		digitalWrite(buzzerPin, LOW);
		buzz_amount = 0;
		doneInitialized = false;
		lastDrawnTimer = -1;
		pomo_state = SET_TIMER;
	}
}

void setup() {
	Serial.begin(115200);
	delay(100);

	// Initialize the pushbutton pin as an input:
	pinMode(buttonPin, INPUT);

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

	switch (pomo_state)
	{
	case SET_TIMER:
		handleTimer();
		break;
	
	case SET_BREAK:
		handleBreak();
		break;
	
	case RUNNING:
		handleRunning();
		break;

	case DONE:
		handleDone();
		break;

	default:
		break;
	}
	
	lastButtonState = buttonState;
}