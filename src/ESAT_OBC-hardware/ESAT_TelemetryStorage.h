/*
 * Copyright (C) 2017, 2018, 2019 Theia Space, Universidad Politécnica de Madrid
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

#ifndef ESAT_TelemetryStorage_h
#define ESAT_TelemetryStorage_h

#include <Arduino.h>
#include <SD.h>
#include <ESAT_CCSDSPacket.h>
#include <ESAT_Timestamp.h>

// Telemetry storage library.
// The SPI interface must be configured before using this library. You
// must have called SPI.begin(CS_SD) before using this library.
// Use the global instance ESAT_TelemetryStorage.
class ESAT_TelemetryStorageClass
{
  public:
    // True on input/output error.  Must be reset manually.
    boolean error;

    // Start reading the packet store between the given timestamps:
    // - telemetry generated at begin or after begin;
    // - telemetry generated at end or before end.
    // Set the error flag on input/output error.
    void beginReading(ESAT_Timestamp begin,
                      ESAT_Timestamp end);

    // End reading the packet store.
    void endReading();

    // Erase the contents of the telemetry store.
    // Set the error flag on input/output error.
    void erase();

    // Read the next packet from the packet store with timestamp
    // coincident with or after begin and coincident with or before
    // end, and write it into the given packet buffer.
    // Return true on success; otherwise return false.
    // Set the error flag on failure.
    // Must be called after beginReading() and before endReading().
    boolean read(ESAT_CCSDSPacket& packet);

    // Return true between beginReading() and endReading();
    // return false the rest of the time.
    boolean reading() const;

    // Return the size in bytes of the telemetry store.
    // Set the error flag on error.
    unsigned long size();

    // Write a packet to the packet store.
    // Set the error flag on failure.
    // This function does nothing when the packet store is open for
    // reading, which happens between beginReading() and endReading().
    void write(ESAT_CCSDSPacket& packet);

  private:
    // Store telemetry in this file.
    static const char TELEMETRY_FILE[];

    // Read telemetry generated at this timestamp or after this
    // timestamp.
    ESAT_Timestamp beginTimestamp;

    // Read telemetry generated at this timestamp or before this
    // timestamp.
    ESAT_Timestamp endTimestamp;

    // Store telemetry in this file.
    File file;

    // Set to true between beginReading() and endReading();
    // false the rest of the time.
    boolean readingInProgress = false;
};

// Global instance of the telemetry storage library.
extern ESAT_TelemetryStorageClass ESAT_TelemetryStorage;

#endif /* ESAT_TelemetryStorage_h */
