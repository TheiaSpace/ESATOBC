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

#ifndef ESATOBCSubsystem_h
#define ESATOBCSubsystem_h

#include <Arduino.h>
#include "ESATSubsystem.h"
#include "ESATClock.h"

class ESATOBCSubsystem: public ESATSubsystem
{
  public:
    boolean storeTelemetry;

    // Start the OBC.
    void begin();

    // Return the identifier of this subsystem.
    word getApplicationProcessIdentifier();

    // Handle a telecommand.
    void handleTelecommand(ESATCCSDSPacket& packet);

    // Fill a packet with the next telemetry packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    boolean readTelemetry(ESATCCSDSPacket& packet);

    // Return true if there is new telemetry available;
    // Otherwise return false.
    boolean telemetryAvailable();

    // Update the subsystem.
    virtual void update();
    
    void handleDownloadTelemetry(ESATCCSDSPacket& packet);

  private:
    // Command codes.
    enum CommandCode
    {
      SET_TIME = 0x00,
      STORE_TELEMETRY = 0x01,
      DOWNLOAD_TELEMETRY = 0x02,
    };

    // Telemetry packet identifiers.
    enum TelemetryPacketIdentifier
    {
      HOUSEKEEPING = 0,
    };
    
    // Download stored telemetry 
    boolean downloadStoredTelemetry;
    ESATTimestamp downloadStoredTelemetryToTimestamp;
    ESATTimestamp downloadStoredTelemetryFromTimestamp;
    unsigned int fileCharPointer;

    // Unique identifier of the subsystem.
    static const word APPLICATION_PROCESS_IDENTIFIER = 0;

    // Version numbers.
    static const byte MAJOR_VERSION_NUMBER = 3;
    static const byte MINOR_VERSION_NUMBER = 0;
    static const byte PATCH_VERSION_NUMBER = 0;

    // True when a new telemetry packet is ready (after update()).
    // False otherwise (after readTelemetry()).
    boolean newHousekeepingTelemetryPacket;

    // The telemetry packet sequence count is incremented every time a
    // new telemetry packet is generated.
    word telemetryPacketSequenceCount;

    // Command handlers.
    void handleSetTimeCommand(ESATCCSDSPacket& packet);
    void handleStoreTelemetry(ESATCCSDSPacket& packet);
    void readStoredTelemetry(ESATCCSDSPacket& packet);
};

extern ESATOBCSubsystem OBCSubsystem;
#endif
