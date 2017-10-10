/*
 * This file is part of Theia Space's ESAT OBC library.
 *
 * Theia Space's ESAT OBC library is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Theia Space's ESAT OBC library is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Theia Space's ESAT OBC library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "ESATStorage.h"
#include "ESATTimestamp.h"
#include <SD.h>


void ESATStorage::begin()
{
  const boolean correctBegin = SD.begin(PIN);
  if (!correctBegin)
  {
    error = true;
  }
}
void ESATStorage::write(ESATTimestamp Timestamp, byte telemetry[], unsigned long telemetryLength)
{
  char cTimestamp[Timestamp.charTimestampLength];
  Timestamp.toStringTimeStamp(cTimestamp);
  
  char filename[Timestamp.charDateLength + 4] = "";
  Timestamp.getDateWithoutDashes(filename);
  strcat(filename, ".txt");
  
  char cbyte[3];
  
  
  File file = SD.open(filename, FILE_WRITE);
  if (file)
  {
    file.print(cTimestamp);
    file.print(' ');
    for(unsigned long indx = 0; indx < telemetryLength; indx++)
    {
      sprintf(cbyte,"%02X",telemetry[indx]);
      file.print(cbyte);
    }
    file.println();
    file.close();
  }
  else
  {
    error = true;
  }
}

void ESATStorage::openReadFile(ESATTimestamp Timestamp)
{
  char filename[Timestamp.charDateLength + 4] = "";
  Timestamp.getDateWithoutDashes(filename);
  strcat(filename, ".txt");
  
  if(readFile)
  {
    closeReadFile();
  }
  readFile = SD.open(filename, FILE_READ);
}


unsigned long ESATStorage::readLine(ESATTimestamp* Timestamp, byte telemetry[], unsigned int maxTelemetrySize)
{
  Timestamp->update(0,0,0,0,0,0);
  char cTimestamp[Timestamp->charTimestampLength];
  unsigned int telemetryLength = 0;
  unsigned long filePosition;
  int Char;
  int nBytes;
  char cByte[3];
  cByte[2] = '\0';
  if(!readFile)
  {
    error = true;
  }
  else
  {
    nBytes = readFile.read(cTimestamp, Timestamp->charTimestampLength - 1);
    if(nBytes == Timestamp->charTimestampLength - 1){
      cTimestamp[Timestamp->charTimestampLength - 1] = '\0';
      if(Timestamp->update(cTimestamp) == Timestamp->VALID_TIMESTAMP)
      {
        Char = readFile.read();
        if(Char == ' ')
        {
          for(unsigned int telemetryByte = 0; telemetryByte < maxTelemetrySize; telemetryByte++){
            Char = readFile.read();
            if(!charIsHex(Char))
            {
              break;
            }
            cByte[0] = Char; 
            Char = readFile.read();
            if(!charIsHex(Char))
            {
              break;
            }
            cByte[1] = Char; 
            
            sscanf(cByte,"%2x",&Char);
            telemetry[telemetryByte] = (byte)Char;
            telemetryLength = telemetryLength + 1;
          }          
        }
      }    
    }    
  }
  filePosition = readFile.position();
  readFile.seek(filePosition - 1);
  Char = readFile.read();
  while(Char != '\n')
  {
    if(!readFile.available())
    {
      break;
    }
    Char = readFile.read();
  }
  return telemetryLength;
}

boolean ESATStorage::charIsHex(char theChar){
  if('0' <= theChar && theChar <= '9')
  {
    return true;
  }
  else if('A' <= theChar && theChar <= 'F')
  {
    return true;
  }
  else if('a' <= theChar && theChar <= 'f')
  {
    return true;
  }
  else
  {
    return false;
  }
}

void ESATStorage::closeReadFile()
{
  if(readFile)
  {
    readFile.close();
  }
}

void ESATStorage::saveCurrentLinePosition()
{
  if(!readFile)
  {
    error = true;
  }
  else
  {
    fileCharPointer = readFile.position();
  }
  
}
    
void ESATStorage::goToSavedPosition()
{
  if(!readFile)
  {
    error = true;
  }
  else
  {
    readFile.seek(fileCharPointer);
  }
  
}
    
void ESATStorage::resetLinePosition()
{
  fileCharPointer = 0;
}
    
boolean ESATStorage::fileExists(ESATTimestamp Timestamp)
{
  char filename[Timestamp.charDateLength + 4] = "";
  Timestamp.getDateWithoutDashes(filename);
  strcat(filename, ".txt");
  return SD.exists(filename);
  
}
    
int ESATStorage::available()
{
  if(!readFile)
  {
    error = true;
    return false;
  }
  else
  {
    return readFile.available();
  }
  
}


ESATStorage Storage;
