#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

const int buttonPin = 25;
int buttonState = 0;
int lastButtonState = 0;
float pomodoro_counter = 0.0;
String pomodoro_counter_string;

TFT_eSPI tft = TFT_eSPI();

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
	tft.println("Hello World");
}

void loop() {
	// read the state of the pushbutton value:
  	buttonState = digitalRead(buttonPin);

	if (buttonState == LOW && lastButtonState == HIGH)
	{
		pomodoro_counter += 5.0;
		pomodoro_counter_string = String(pomodoro_counter);

		tft.setTextColor(TFT_BLUE, TFT_BLACK);
		tft.setTextDatum(MC_DATUM);   // Middle Center
		tft.drawString(pomodoro_counter_string, 65, 75, 1); // Found center
		Serial.println(pomodoro_counter);
		delay(200); // 500 ms delay between each increments
	}

	lastButtonState = buttonState;
}