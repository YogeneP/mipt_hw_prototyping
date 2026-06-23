#include <OneWire.h>
#include <DallasTemperature.h>

#define T_PIN 13

OneWire ow(T_PIN);
DallasTemperature sensors(&ow);
DeviceAddress sensorAddr;

void setup()
{
  Serial.begin(115200);
  if (!ow.search(sensorAddr)) {
    Serial.println("No oneWire device found!");
    return; 
  }
  Serial.println("Starting...");
  sensors.begin();
  sensors.setResolution(sensorAddr, 12);
}

void loop()
{ 
  if (ow.search(sensorAddr)) {
    sensors.requestTemperatures();
    float t = sensors.getTempC(sensorAddr);
    Serial.print("Temp C: ");
    Serial.println(t);
  }
  delay(1000);
}
