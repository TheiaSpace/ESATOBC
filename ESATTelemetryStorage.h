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

#ifndef ESATTelemetryStorage_h
#define ESATTelemetryStorage_h

#include <Energia.h>
#include <SD.h>
#include "ESATTimestamp.h"
#include <ESATCCSDSPacket.h>

class ESATTelemetryStorage
{
  public:
    // True on input/output error.  Must be reset manually.
    boolean error;

    // Begin the SD card subsystem.
    // This will set the error flag on input/output error.
    void begin();

    // Start reading the packet store.
    // Set the error flag on input/output error or if the packet store
    // is already open.
    void beginReading();

    // Start writing to the packet store.
    // Set the error flag on input/output error or if the packet store
    // is already open.
    void beginWriting();

    // End reading the packet store.
    void endReading();

    // End writing to the packet store.
    void endWriting();

    // Read a packet from the packet store into the given packet buffer.
    // Return true on success; otherwise return false.
    // Set the error flag on failure.
    // Must be called after beginReading() and before endReading().
    boolean read(ESATCCSDSPacket& packet);

    // Write a packet to the packet store.
    // Set the error flag on failure.
    // Must be called after beginWriting() and before endWriting().
    void write(ESATCCSDSPacket& packet);

  private:
    // The chip select line of the SD card reader is connected to this pin.
    static const byte PIN = SS1;

    // Store telemetry in this file.
    File file;

    // Store telemetry in this file.
    static const char TELEMETRY_FILE[];
};

extern ESATTelemetryStorage TelemetryStorage;

#endif
