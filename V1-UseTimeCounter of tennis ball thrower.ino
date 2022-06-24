#include <Arduino.h>
#include <Ds1302.h>
#include <LiquidCrystal.h>

// RTC CLock Ports
const int PIN_CLK = 9, PIN_DAT = 10, PIN_ENA = 11;
Ds1302 rtc(PIN_ENA, PIN_CLK, PIN_DAT);

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
//rs, en, d4, d5, d6, d7
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Identify if the machine is ON
const int onButton = 8;
bool isMachineOn = false;
bool lastIsMachineOn = false;

// Voltimetro
const int battVoltPort = 0;
float battVolt = LOW;


// Data of using
String lastResetFormated = "";
Ds1302::DateTime onTime;
Ds1302::DateTime offTime;
 
void setup() {
  setupSerial();
  setupLcd();
  setupRtc();
}

void loop() {
  proccessTimes();
  printLineUp();
  printLineDown();
  delay(1000);
}

void printLineUp()
{
  lcd.setCursor(0, 0);
  battVolt = (analogRead(0)*(5.0/1023.0))*5.0;
  String text = lastResetFormated + ' ' + String(battVolt).substring(0, 4) + "V";
  lcd.print(text);
  Serial.println(text);
}

void printLineDown()
{
  lcd.setCursor(0, 1);
  String onDay = "";
  String onHour = "";
  String offHour = "";

  if (onTime.year > 0)
  {
    onDay = "D" + String(onTime.day);
    onHour = getHourFromDate(onTime);
  }

  if (offTime.year > 0)
  {
    offHour = getHourFromDate(offTime);
  }

  String text = onDay + ' ' + onHour + ' ' + offHour;
  lcd.print(text);
  Serial.println(text);
}

void proccessTimes()
{
  isMachineOn = digitalRead(onButton);
  if (isMachineOn)
  {
    if (onTime.year == 0)
    {
      onTime = GetNow();
    }
  }
  else {
    if (lastIsMachineOn)
    {
      offTime = GetNow();
    }
  }
  lastIsMachineOn = isMachineOn;
}

void setupSerial()
{
    Serial.begin(9600);
}

void setupLcd()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("A&G");
}

void setupRtc()
{
    rtc.init();
    Ds1302::DateTime now = GetNow();
    if (now.year <= 1)
    {
        Serial.println("RTC is halted. Setting time...");
        Ds1302::DateTime dt = {
            .year = 22,
            .month = Ds1302::MONTH_JUN,
            .day = 19,
            .hour = 22,
            .minute = 23,
            .second = 0
        };
        rtc.setDateTime(&dt);
    }

    if (lastResetFormated == "")
    {
      lastResetFormated = getFormatedNow();
    }
}

Ds1302::DateTime GetNow()
{
  Ds1302::DateTime now;
  rtc.getDateTime(&now);
  return now;
}

String getFormatedNow()
{
    Ds1302::DateTime now = GetNow();
    String day = getFormatedNumber(now.day);
    String month = getFormatedNumber(now.month);
    String hour = getFormatedNumber(now.hour);
    String minute = getFormatedNumber(now.minute);
    return 'D' + day + ' ' + hour + ':' + minute;
}


String getHourFromDate(Ds1302::DateTime date)
{
    String hour = getFormatedNumber(date.hour);
    String minute = getFormatedNumber(date.minute); 
    return hour + ':' + minute;
}

String getFormatedNumber(int number)
{  
  String ret = String(number);
  if (number < 10) ret = '0' + ret;
  return ret;
}
