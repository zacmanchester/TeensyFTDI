/*
  This sketch allows a Teensy 3.1 board to function as an FTDI
  programmer for another Arduino board.
*/
unsigned long baud = 19200;
const int reset_pin = 23; // This can be set to any pin you want
const int led_pin = 13;
const int led_on = HIGH;
const int led_off = LOW;

void setup()
{
	pinMode(led_pin, OUTPUT);
	digitalWrite(led_pin, led_off);
	digitalWrite(reset_pin, HIGH);
	pinMode(reset_pin, OUTPUT);
	Serial.begin(baud);	// USB, communication to PC
  Serial1.begin(baud);// UART, communication to Arduino
}

long led_on_time = 0;

void loop()
{
	unsigned char c, dtr;
	static unsigned char prev_dtr = 0;

	if (Serial.available()) {
		c = Serial.read();
		Serial1.write(c);
		digitalWrite(led_pin, led_on);
		led_on_time = millis();
		return;
	}
	if (Serial1.available()) {
		c = Serial1.read();
		Serial.write(c);
		digitalWrite(led_pin, led_on);
		led_on_time = millis();
		return;
	}
	dtr = Serial.dtr();
	if (dtr && !prev_dtr) {
		digitalWrite(reset_pin, LOW);
		delayMicroseconds(250);
		digitalWrite(reset_pin, HIGH);
	}
	prev_dtr = dtr;
	if (millis() - led_on_time > 3) {
		digitalWrite(led_pin, led_off);
	}
	if (Serial.baud() != baud) {
		baud = Serial.baud();
		if (baud == 57600) {
			// This ugly hack is necessary for talking
			// to the arduino bootloader, which actually
			// communicates at 58824 baud (+2.1% error).
			// Teensyduino will configure the UART for
			// the closest baud rate, which is 57143
			// baud (-0.8% error).  Serial communication
			// can tolerate about 2.5% error, so the
			// combined error is too large.  Simply
			// setting the baud rate to the same as
			// arduino's actual baud rate works.
			Serial1.begin(58824);
		} else {
			Serial1.begin(baud);
		}
	}
}

