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

#ifndef ESATCOMMSSubsystem_h
#define ESATCOMMSSubsystem_h

#include <Energia.h>
#include "ESATCommand.h"
#include "ESATSubsystem.h"

class ESATCOMMSSubsystem: public ESATSubsystem
{
  public:
    int status;

    // Start the communications subsystem.
    virtual void begin();

    // Return the start order of this subsystem.  Subsystems with a
    // low order number start/begin before subsystems with a high
    // order number.
    virtual byte getStartOrder();

    // Return the identifier of this subsystem.
    virtual byte getSubsystemIdentifier();

    // Handle a command of given code and parameters.
    virtual void handleCommand(byte commandCode, String parameters);

    // Return a string with the next pending command.
    // If there are no pending commands, return an empty string.
    ESATCommand readCommand();

    // Return a string with the hexadecimal dump
    // of the subsystem's telemetry.
    virtual String readTelemetry();

    // Update the subsystem.
    virtual void update();

    // Write a telemetry packet.
    void writePacket(String packet);

  private:
    static const byte address = 2;
    static const unsigned long baudRate = 115200;
};

extern ESATCOMMSSubsystem COMMSSubsystem;

#endif
