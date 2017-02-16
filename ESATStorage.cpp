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
  working = SD.begin(pin);
}

void ESATStorage::write(String filename, String text)
{
  File file = SD.open(filename.c_str(), FILE_WRITE);
  if (file)
  {
    file.println(text);
    file.close();
    working = true;
  }
  else
  {
    working = false;
  }
}

ESATStorage Storage;
