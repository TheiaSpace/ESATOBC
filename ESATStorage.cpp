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
#include <SD.h>

void ESATStorage::begin()
{
  const boolean correctBegin = SD.begin(PIN);
  if (!correctBegin)
  {
    error = true;
  }
}
// void ESATStorage::write(char filename[], ESATTimestamp Timestamp, byte telemetry[], unsigned long telemetryLength)
void write(char filename[], char timestamp[], char text[])
{
  // char cTimestamp[Timestamp.charTimestampLength];
  // char cbyte[3];
  // Timestamp.toStringTimeStamp(cTimestamp);
  // File file = SD.open(filename, FILE_WRITE);
  // if (file)
  // {
    // file.print(cTimestamp);
    // for(unsigned long indx = 0; indx < telemetryLength; indx++)
    // {
      // sprintf(cbyte,"%2X",telemetry[indx];
      // file.print(cbyte);
    // }
    // file.println();
    // file.close();
  // }
  // else
  // {
    // error = true;
  // }
}

void ESATStorage::openReadFile(char filename[])
{
  if(readFile)
  {
    closeReadFile();
  }
  readFile = SD.open(filename, FILE_READ);
}


// unsigned int ESATStorage::readLine(ESATTimestamp Timestamp, byte telemetry[], unsigned int maxNumCharacters)
unsigned int readLine(char timestamp[], char line[], unsigned int maxNumCharacters)
{
  // Timestamp.update(0,0,0,0,0,0);
  // char cTimestamp[Timestamp.charTimestampLength];
  // unsigned int lineLength = 0;
  // strcpy(line,"");
  // strcpy(timestamp,"");
  // int Char;
  // if(!readFile)
  // {
    // error = true;
  // }
  // else
  // {
    // for(unsigned int c = 0; c < Timestamp.charTimestampLength - 1; c++){
      // Char = readFile.read();
      // if(Char == '\n')
      // {
        // return 0;
      // }
      // else if(Char == -1)
      // {
        // return 0;
      // }
      // cTimestamp[c] = Char;
    // }
    // cTimestamp[Timestamp.charTimestampLength - 1] = '\0';
    
    // for(unsigned int c = 0; c < maxNumCharacters; c++){
      // Char = readFile.read();
      // if(!charIsHex(Char))
      // {
        // break;
      // }
      // line[c] = Char;
      // lineLength = lineLength + 1;
    // }
      // line[lineLength] = '\0';
      // lineLength = lineLength + 1;
    
    
  // }
  // return lineLength;
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
  if(!readFile)
  {
    error = true;
  }
  else
  {
    fileCharPointer = 0;
  }
  
}
    
boolean ESATStorage::fileExists(char file[])
{
  return SD.exists(file);
  
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
