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
#include "ESATTimestamp.h"
#include <ESATCCSDSPacket.h>

class ESATStorage
{
  public:
    // True on input/output error.  Must be reset manually.
    boolean error;

    // Begin the SD card subsystem.
    // This will set the error flag on input/output error.
    void begin();

    // Write a packet to the packet store.
    void write(ESATCCSDSPacket& packet);

    // Read the next text line from a file.
    unsigned long read(ESATTimestamp* Timestamp, ESATCCSDSPacket& packet);

  private:
    static const byte PIN = 45;
    File readFile;
    unsigned long fileCharPointer;

    // Store telemetry in this file.
    static const char TELEMETRY_FILE[];
};

extern ESATStorage Storage;

#endif
