/*
 * Copyright (C) 2021 Theia Space, Universidad Politécnica de Madrid
 *
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

#include "ESAT_OBC-hardware/ESAT_SDCardStorage.h"
#include <SD.h>

ESAT_SDCardStorageClass::ESAT_SDCardStorageClass(const char* filename)
{
  // Crop filename to the first 8 characters if it is bigger due to
  // file system limitations.
  byte index;
  for (index = 0; index < 8 && filename[index] != 0 ; ++index)
  {
    storageFile[index] = filename[index];
  }
  storageFile[index] = 0;
}

boolean ESAT_SDCardStorageClass::read(char& data, word pos)
{
  File file = SD.open(storageFile, FILE_READ);
  if (file.available() > pos && pos < file.size())
  {
    (void) file.seek(pos);
    data = file.read();
    file.close();
    return true;
  }
  file.close();
  return false;
}

boolean ESAT_SDCardStorageClass::write(char data, word pos)
{
  File file = SD.open(storageFile, FILE_WRITE);
  if (file.size() > pos) // Edit a position
  {
    file.seek(pos);
    byte written = file.write(data);
    file.close();
    if (written == 1)
    {
      return true;  
    }
    Serial.println("write failed");
    return false;
  }
  if (file.size() == pos) // Append data.
  {
    file.close();
    file = SD.open(storageFile, FILE_APPEND);
    byte written = file.write(data);
    file.close();
    if (written == 1)
    {
      return true;  
    }
    Serial.println("append failed");
    return false;
  }
  // You are trying to write far beyond the file current size.
  return false;
}
