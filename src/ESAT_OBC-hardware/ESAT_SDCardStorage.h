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

#ifndef ESAT_SDCardStorage_h
#define ESAT_SDCardStorage_h

#include <Arduino.h>

// SD Card storage library.
// This class allows you to store things on the SD-Card by using a file.
// To use this storage system, an instance of the class should be created
// with a proper filename.
// Then, user should take car of the reading and writing postions of the
// data inside the file. Readings and writings beyond the file size will fail.
// The SPI interface must be configured before using this library. You
// must have called SD.begin(CS_SD) before using this library.
class ESAT_SDCardStorage
{
  public:
    // Default constructor.
    ESAT_SDCardStorage();
    
    // Creates a SD-Card storage instance.
    // Requires the name of the file where to store the data.
    // Due to a file system limitation, filename is limited to 
    // 8 characters. Longer names will be crop.
    ESAT_SDCardStorage(const char* filename);
  
    // Read the character located at the given position and
    // copies it to the given buffer.
    // Return true on success; otherwise return false.
    boolean read(char& data, word pos);

    // Write the given character at the given position.
    // Return true on success; otherwise return false.
    boolean write(char data, word pos);

  private:      
    // Store data in this file.
    char storageFile[9];          
};

#endif /* ESAT_SDCardStorage_h */
