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

    // Fill a packet with the next telecommand packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    boolean readTelecommand(ESATCCSDSPacket& packet);

    // Fill a packet with the next telemetry packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    boolean readTelemetry(ESATCCSDSPacket& packet);

    // Return true if there is new telemetry available;
    // Otherwise return false.
    boolean telemetryAvailable();

    // Update the subsystem.
    virtual void update();

   // Send a telemetry packet to this subsystem.
    void writeTelemetry(ESATCCSDSPacket& packet);

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

    // Minimum command payload data length in bytes:
    // - Year (2 byte).
    // - Month (1 byte).
    // - Day (1 byte).
    // - Hours (1 byte).
    // - Minutes (1 byte).
    // - Seconds (1 byte).
    // - Major version number (1 byte).
    // - Minor version number (1 byte).
    // - Patch version number (1 byte).
    // - Command code (1 byte).
    static const byte MINIMUM_COMMAND_PAYLOAD_DATA_LENGTH = 11;

    // Unique identifier of the subsystem.
    static const word APPLICATION_PROCESS_IDENTIFIER = 0;

    // Version numbers.
    static const byte MAJOR_VERSION_NUMBER = 3;
    static const byte MINOR_VERSION_NUMBER = 0;
    static const byte PATCH_VERSION_NUMBER = 0;

    // True if we were commanded to download telemetry; false
    // otherwise.
    boolean downloadTelemetry;

    // True when a new telemetry packet is ready (after update()).
    // False otherwise (after readTelemetry()).
    boolean newHousekeepingTelemetryPacket;

    // The telemetry packet sequence count is incremented every time a
    // new telemetry packet is generated.
    word telemetryPacketSequenceCount;

    // Command handlers.
    void handleSetTimeCommand(ESATCCSDSPacket& packet);
    void handleSetModeCommand(ESATCCSDSPacket& packet);
    void handleStoreTelemetry(ESATCCSDSPacket& packet);
    void handleDownloadTelemetry(ESATCCSDSPacket& packet);

    // Fill a new housekeeping telemetry packet.  Return true on
    // success; otherwise return false.
    boolean readHousekeepingTelemetry(ESATCCSDSPacket& packet);

    // Read the next stored telemetry packet and fill the given packet buffer.
    // Return true on success; otherwise return false.
    // Set downloadTelemetry to false on unsuccessful read.
    boolean readStoredTelemetry(ESATCCSDSPacket& packet);
};

extern ESATOBCSubsystem OBCSubsystem;
#endif
