#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

const int buttonPin = 25;
int buttonState = 0;
int lastButtonState = 0;
const unsigned long holdtime = 1000; // 1 sec

TFT_eSPI tft = TFT_eSPI();

// -------- Pomodoro -------- //
enum PomodoroState {
    IDLE,
    RUNNING,
    DONE
};

PomodoroState pomo_state = IDLE;
unsigned long pomodoro_timer[2] = {0, 0};
String pomodoro_counter_string;
unsigned long start_time = 0;
String time_spent_s;
unsigned long max_timer = 180;
unsigned long lastDisplayUpdate = 0;
const unsigned long DISPLAY_INTERVAL = 1000; // 1 second
unsigned long pressStartTime = 0;



void handleIdle() {
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
			if (pomodoro_timer[0] + 10 > max_timer)
			{
				Serial.println("Reset timer");
				pomodoro_timer[0] = 0; // Reset timer
			} else {
				Serial.println("Increments");
				pomodoro_timer[0] += 10;
			}
			
			Serial.println(pomodoro_timer[0]);
		} else if (pressDuration >= holdtime) {
			Serial.println("Timer started");
			start_time = millis();
			pomo_state = RUNNING;
		}
	}

	pomodoro_counter_string = String(pomodoro_timer[0]);
	tft.fillRect(0, 40, 160, 40, TFT_BLACK); // Clear area to not leave pixels
	tft.setTextColor(TFT_BLUE, TFT_BLACK);
	tft.setTextDatum(MC_DATUM); // Middle Center
	tft.drawString(pomodoro_counter_string, 65, 75, 1); // Found center
}

void handleRunning() {
	unsigned long now = millis();
	unsigned long elapsed = now - start_time;

	if (elapsed/1000.0 >= pomodoro_timer[0]) // Target time reached
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
	
	if (now - lastDisplayUpdate >= DISPLAY_INTERVAL) 
	{
		tft.fillRect(0, 40, 160, 40, TFT_BLACK); // Clear area to not leave pixels
		tft.setTextColor(TFT_BLUE, TFT_BLACK);
		tft.setTextDatum(MC_DATUM); // Middle Center

		time_spent_s = String(elapsed/1000);

		tft.drawString(time_spent_s, 65, 75, 1); // Found center
	}
}

void handleDone() {
	Serial.println("BUZZ BUZZ BUZZ - Timer completed");
	pomo_state = IDLE;
}

void setup() {
	Serial.begin(115200);
	delay(100);

	// initialize the pushbutton pin as an input:
	pinMode(buttonPin, INPUT);

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
	case IDLE:
		handleIdle();
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