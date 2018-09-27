/*
 * Copyright (C) 2018 Theia Space, Universidad Politécnica de Madrid
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

#ifndef ESAT_OBCTelemetryStorageTelecommands_h
#define ESAT_OBCTelemetryStorageTelecommands_h

#include <Arduino.h>
#include <ESAT_CCSDSPacketConsumer.h>

// Telecommand handler for telemetry storage-related commands.
// Used by ESAT_OBCSubsystem.
class ESAT_OBCTelemetryStorageTelecommandsClass: public ESAT_CCSDSPacketConsumer
{
  public:
    // Handle a telecommand packet.
    // Return true on success; otherwise return false.
    boolean consume(ESAT_CCSDSPacket packet);

  private:
    // Command codes.
    enum CommandCode
    {
      STORE_TELEMETRY = 0x01,
      DOWNLOAD_STORED_TELEMETRY = 0x02,
      ERASE_STORED_TELEMETRY = 0x03,
    };

    // Handle the telecommand for enabling or disabling the storage of
    // telemetry.
    // Return true on success; otherwise return false.
    boolean handleStoreTelemetryTelecommand(ESAT_CCSDSPacket packet);

    // Handle the telecommand for downloading stored telemetry.
    // Return true on success; otherwise return false.
    boolean handleDownloadStoredTelemetryTelecommand(ESAT_CCSDSPacket packet);

    // Handle the telecommand for erasing the stored telemetry.
    // Return true on success; otherwise return false.
    boolean handleEraseStoredTelemetryTelecommand(ESAT_CCSDSPacket packet);
};

// Global instance of ESAT_OBCTelemetryStorageTelecommandsClass.
// Used by ESAT_OBCSubsystem.
extern ESAT_OBCTelemetryStorageTelecommandsClass ESAT_OBCTelemetryStorageTelecommands;

#endif /* ESAT_OBCTelemetryStorageTelecommands_h */
