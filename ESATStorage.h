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

#ifndef ESATStorage_h
#define ESATStorage_h

#include <Energia.h>
#include <SD.h>

class ESATStorage
{
  public:
    boolean alive;

    // Begin the SD card subsystem.
    void begin();

    // Write text to a file.
    void write(String filename, String text);
    
    // Open a file to read it
    void openReadFile(String filename);
    
    // Read the next text line from a file.
    String readLine(String filename, unsigned int maxNumCharacters);
    
    // Close the file opened to read it
    void closeReadFile();
    
    // Save the current line position
    void saveCurrentLinePosition();
    
    // Go to the last saved line position
    void goToSavedPosition();
    
    // Reset the line position saved
    void resetLinePosition();
    
    // Does the file exists?
    boolean fileExists(char timestamp[]);
    
    // There are still more lines to read?
    boolean available();
    
    boolean thereAreMoreTextToRead;

  private:
    static const byte pin = 45;
    File readFile;
    unsigned long fileCharPointer;
};

extern ESATStorage Storage;

#endif
