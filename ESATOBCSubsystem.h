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

#include <Energia.h>
#include "ESATSubsystem.h"

class ESATOBCSubsystem: public ESATSubsystem
{
  public:
    byte identifier;
    boolean storeTelemetry;

    // Start the OBC.
    virtual void begin();

    // Return the start order of this subsystem.  Subsystems with a
    // low order number start/begin before subsystems with a high
    // order number.
    virtual byte getStartOrder();

    // Return the identifier of this subsystem.
    virtual byte getSubsystemIdentifier();

    // Handle a command of given code and parameters.
    virtual void handleCommand(byte commandCode, String parameters);

    // Return a string with the hexadecimal dump
    // of the subsystem's telemetry.
    virtual String readTelemetry();

    // Update the subsystem.
    virtual void update();

  private:
    enum CommandCode
    {
      STORE_ID = 0,
      SET_TIME = 1,
      STORE_TELEMETRY = 2,
      DOWNLOAD_TELEMETRY = 3
    };

    static const byte CLOCK_OFFSET = 0;
    static const byte IMU_OFFSET = 1;
    static const byte EPS_OFFSET = 2;
    static const byte STORAGE_OFFSET = 3;
    static const byte COMMS_OFFSET = 4;
    static const byte COMMS_MASK = (1 << 0) | (1 << 1);
    
    // Download stored telemetry 
    boolean DownloadStoredTelemetry;
    String downloadStoredTelemetryFromTimestamp;
    String downloadStoredTelemetryToTimestamp;
    

    void handleStoreIdCommand(String parameters);
    void handleSetTimeCommand(String parameters);
    void handleStoreTelemetry(String parameters);
    void handleDownloadTelemetry(String parameters);

    byte loadIdentifier();
};

extern ESATOBCSubsystem OBCSubsystem;
#endif
