
#include <ESAT_SDCardStorage.h>

const char NAME8[] = "NOMBRE8_";
const char NAME7[] = "NOMBRE7";
const char NAME9[] = "NOMBRES9_";


ESAT_SDCardStorageClass storage8 = ESAT_SDCardStorageClass(NAME8);


char charbuff;;

void setup() 
{
  Serial.begin();
  delay(3000);
  storage8 = ESAT_SDCardStorageClass(NAME8);
  boolean retval = storage8.read(charbuff, 0);
  Serial.print("Read at 0 result: ");
  Serial.println(retval);
  Serial.print("Read at 0 value: ");
  Serial.println(charbuff);
  retval = storage8.read(charbuff, 1);
  Serial.print("Read at 1 result: ");
  Serial.println(retval);
  Serial.print("Read at 1 value: ");
  Serial.println(charbuff);

  charbuff = 'a';
  retval = storage8.write(charbuff, 0);
  Serial.print("Write at 0 result: ");
  Serial.println(retval);
  charbuff = 'f';
  retval = storage8.read(charbuff, 0);
  Serial.print("Read at 0 result: ");
  Serial.println(retval);
  Serial.print("Read at 0 value: ");
  Serial.println(charbuff);
  

}

void loop() {
  // put your main code here, to run repeatedly:

}
