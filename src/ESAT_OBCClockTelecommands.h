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

#ifndef ESAT_OBCClockTelecommands_h
#define ESAT_OBCClockTelecommands_h

#include <Arduino.h>
#include <ESAT_CCSDSPacketConsumer.h>

// Telecommand handler for OBC clock-related commands.
// Used by ESAT_OBCSubsystem.
class ESAT_OBCClockTelecommandsClass: public ESAT_CCSDSPacketConsumer
{
  public:
    // Handle a telecommand packet.
    // Return true on success; otherwise return false.
    boolean consume(ESAT_CCSDSPacket packet);

  private:
    // Command codes.
    enum CommandCode
    {
      SET_TIME = 0x00,
    };

    // Handle the telecommand for setting the time of the OBC clock.
    // Return true on success; otherwise return false.
    boolean handleSetTimeTelecommand(ESAT_CCSDSPacket packet);
};

// Global instance of ESAT_OBCClockTelecommandsClass.
// Used by ESAT_OBCSubsystem.
extern ESAT_OBCClockTelecommandsClass ESAT_OBCClockTelecommands;

#endif /* ESAT_OBCClockTelecommands_h */
