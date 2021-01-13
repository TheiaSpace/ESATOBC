
#include <ESAT_SDCardStorage.h>
#include <SD.h>

const char NAME8[] = "NOMBRE8_";

ESAT_SDCardStorage storage8 = ESAT_SDCardStorage(NAME8);

char charbuff;

void setup() 
{
  Serial.begin();
  SD.begin(CS_SD);
  delay(3000);  
  boolean retval = storage8.read(charbuff, 0);
  Serial.print("Read at 0 result: ");
  Serial.println(retval);
  Serial.print("Read at 0 value: ");
  Serial.println(charbuff, DEC);
  retval = storage8.read(charbuff, 1);
  Serial.print("Read at 1 result: ");
  Serial.println(retval);
  Serial.print("Read at 1 value: ");
  Serial.println(charbuff, DEC);

  retval = storage8.write(++charbuff, 0);
  Serial.print("Write at 0 result: ");
  Serial.println(retval);
  charbuff = -1;
  retval = storage8.read(charbuff, 0);
  Serial.print("Read at 0 result: ");
  Serial.println(retval);
  Serial.print("Read at 0 value: ");
  Serial.println(charbuff, DEC);
  
  retval = storage8.write(++charbuff, 17);
  Serial.print("Write at 17 result: ");
  Serial.println(retval);
  charbuff = -1;
  retval = storage8.read(charbuff, 17);
  Serial.print("Read at 17 result: ");
  Serial.println(retval);
  Serial.print("Read at 17 value: ");
  Serial.println(charbuff, DEC);
  

}

void loop() {
  // put your main code here, to run repeatedly:

}