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

#ifndef ESAT_OBCTelemetryTelecommands_h
#define ESAT_OBCTelemetryTelecommands_h

#include <Arduino.h>
#include <ESAT_CCSDSPacketConsumer.h>
#include <ESAT_SemanticVersionNumber.h>

// Telecommand handler for OBC telemetry-related telecommands.
// Used by ESAT_OBCSubsystem.
class ESAT_OBCTelemetryTelecommandsClass: public ESAT_CCSDSPacketConsumer
{
  public:
    // Handle a telecommand packet.
    // Return true on success; otherwise return false.
    boolean consume(ESAT_CCSDSPacket packet);

  private:
    // Command codes.
    enum CommandCode
    {
      ENABLE_TELEMETRY = 0x04,
      DISABLE_TELEMETRY = 0x05,
    };

    // Version number of the interface.
    // This telecommand handler will only accept telecommands
    // with a version number that is backwards-compatible with
    // this version number.
    static const ESAT_SemanticVersionNumber INTERFACE_VERSION_NUMBER;

    // Handle the telecommand for enabling a telemetry packet.
    boolean handleEnableTelemetryTelecommand(ESAT_CCSDSPacket packet);

    // Handle the telecommand for disabling a telemetry packet.
    boolean handleDisableTelemetryTelecommand(ESAT_CCSDSPacket packet);
};

// Global instance of ESAT_OBCTelemetryTelecommandsClass.
// Used by ESAT_OBCSubsystem.
extern ESAT_OBCTelemetryTelecommandsClass ESAT_OBCTelemetryTelecommands;

#endif /* ESAT_OBCTelemetryTelecommands_h */
