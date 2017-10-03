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
  alive = SD.begin(pin);
}

void ESATStorage::write(String filename, String text)
{
  File file = SD.open(filename.c_str(), FILE_WRITE);
  if (file)
  {
    file.println(text);
    file.close();
    alive = true;
  }
  else
  {
    alive = false;
  }
}

void ESATStorage::openReadFile(String filename)
{
  if(readFile)
  {
    closeReadFile();
  }
  thereAreMoreTextToRead = true;
  readFile = SD.open(filename.c_str(), FILE_READ);
}


String ESATStorage::readLine(String filename, unsigned int maxNumCharacters)
{
  String line = "";
  int Char;
  if(!readFile)
  {
    alive = false;
  }
  else
  {
    alive = true;
    for(unsigned int c = 0; c < maxNumCharacters; c++){
      Char = readFile.read();
      if(Char == '\n')
      {
        break;
      }
      if(Char == -1)
      {
        thereAreMoreTextToRead = false;
        break;
      }
      line += (char)Char;
    }
  }
  line.trim();
  return line;
}

void ESATStorage::closeReadFile()
{
  if(readFile)
  {
    readFile.close();
  }
}

ESATStorage Storage;
