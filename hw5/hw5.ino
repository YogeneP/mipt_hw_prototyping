#include <OneWire.h>
#include "DallasTemperature.h"
#include <HD44780_LCD_PCF8574.h>

#define T_PIN 13

HD44780LCD lcd(4, 20, 0x27, &Wire); // instantiate an object

char tempString[] = "Temperature: ";
char clrString[] = "        ";
char notfoundString[] = "No sensor found!";
char degString[] = "*C";

OneWire ow(T_PIN);
DallasTemperature sensors(&ow);
DeviceAddress sensorAddr;

void setup()
{
  delay(50);
  lcd.PCF8574_LCDInit(lcd.LCDCursorTypeOff);
  lcd.PCF8574_LCDClearScreen();
  lcd.PCF8574_LCDBackLightSet(true);
  Serial.begin(115200);
  if (!ow.search(sensorAddr)) {
    Serial.println("No oneWire device found!");
    lcd.PCF8574_LCDGOTO(lcd.LCDLineNumberOne, 2);
    lcd.PCF8574_LCDSendString(notfoundString);
  }
  Serial.println("Starting...");
  sensors.begin();
  sensors.setResolution(sensorAddr, 12);
  lcd.PCF8574_LCDGOTO(lcd.LCDLineNumberOne, 3);

  lcd.PCF8574_LCDSendString(tempString);
  lcd.PCF8574_LCDGOTO(lcd.LCDLineNumberTwo, 6);
}

void loop()
{ 
  if (ow.search(sensorAddr)) {
    sensors.requestTemperatures();
    float t = sensors.getTempC(sensorAddr);
    String t_str = String(t);
    char* t_chr = const_cast<char*>(t_str.c_str());
    lcd.PCF8574_LCDGOTO(lcd.LCDLineNumberTwo, 6);
    lcd.PCF8574_LCDSendString(clrString);
    lcd.PCF8574_LCDGOTO(lcd.LCDLineNumberTwo, 6);
    lcd.PCF8574_LCDSendString(t_chr);
    lcd.PCF8574_LCDSendString(degString);
    Serial.print("Temp C: ");
    Serial.println(t);
  }
  delay(1000);
}
