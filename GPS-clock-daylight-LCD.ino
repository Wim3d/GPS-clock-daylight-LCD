/*
   Written by W.J. Hoogervorst, May 2018
   Arduino Nano or Pro mini 5V 168
*/

#include <Timezone.h>
#include "TinyGPS++.h"
#include "SoftwareSerial.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

//define serial connection for GPS
SoftwareSerial GPS_serial(4, 3);  // RX (to TX op GPS) , TX (to RX of GPS)

//define GPS variable
TinyGPSPlus gps;

// time variables
time_t local, utc;

void setup() {
  lcd.begin(16, 2);  // initialize the lcd for 16 chars 2 lines, turn on backlight
  lcd.clear();
  lcd.backlight();

  lcd.setCursor(0, 0); //Start at character 0 on line 0
  lcd.print("Waiting for");
  lcd.setCursor(0, 1);
  lcd.print("GPS fix");
  GPS_serial.begin(9600); //This opens up communications to the GPS

  smartDelay(1000);

  while (gps.satellites.value() == 0)
  {
    smartDelay(1000);
  }
}

void loop()
{
  smartDelay(1000);
  int Year = gps.date.year();
  byte Month = gps.date.month();
  byte Day = gps.date.day();
  byte Hour = gps.time.hour();
  byte Minute = gps.time.minute();
  byte Second = gps.time.second();
  setTime(Hour, Minute, Second, Day, Month, Year);
  // Set Time from GPS data string
  utc = now();

  TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Time (Frankfurt, Paris) *Sun, 2*
  TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Time (Frankfurt, Paris)
  Timezone CE(CEST, CET);
  local = CE.toLocal(utc);

  lcd.clear();
  lcd.setCursor(0, 0); //Start at character 0 on line 0
  lcd.print("S:");
  lcd.setCursor(2, 0);
  lcd.print(gps.satellites.value());
  lcd.setCursor(4, 0); //Start at character 0 on line 1
  lcd.print("UTC:");
  lcd.setCursor(8, 0);
  //lcd.print(gps.time.value());
  if (hour(utc) < 10) // add a zero if minute is under 10
    lcd.print("0");
  lcd.print(hour(utc));
  lcd.print(":");
  if (minute(utc) < 10) // add a zero if minute is under 10
    lcd.print("0");
  lcd.print(minute(utc));
  lcd.print(":");
  if (second(utc) < 10) // add a zero if minute is under 10
    lcd.print("0");
  lcd.print(second(utc));

  lcd.setCursor(0, 1); //Start at character 0 on line 0
  lcd.print("Local:");
  lcd.setCursor(8, 1);

  if (hour(local) < 10) // add a zero if minute is under 10
    lcd.print("0");
  lcd.print(hour(local));
  lcd.print(":");
  if (minute(local) < 10) // add a zero if minute is under 10
    lcd.print("0");
  lcd.print(minute(local));
  lcd.print(":");
  if (second(local) < 10) // add a zero if minute is under 10
    lcd.print("0");
  lcd.print(second(local));
}


static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    // If data has come in from the GPS module
    while (GPS_serial.available())
      gps.encode(GPS_serial.read()); // Send it to the encode function
    // tinyGPS.encode(char) continues to "load" the tinGPS object with new
    // data coming in from the GPS module. As full NMEA strings begin to come in
    // the tinyGPS library will be able to start parsing them for pertinent info
  } while (millis() - start < ms);
}
