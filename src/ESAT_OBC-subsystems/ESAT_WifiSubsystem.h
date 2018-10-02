/*
 * Copyright (C) 2017, 2018 Theia Space, Universidad Politécnica de Madrid
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

#ifndef ESAT_WifiSubsystem_h
#define ESAT_WifiSubsystem_h

#include <Arduino.h>
#include <ESAT_CCSDSPacketFromKISSFrameReader.h>
#include <ESAT_CCSDSPacketToKISSFrameWriter.h>
#include "ESAT_OBC-subsystems/ESAT_Subsystem.h"

// Interface to the Wifi subsystem from the point of view of the
// on-board data handling subsystem.  There is a global instance:
// ESAT_WifiSubsystem.  This should be the only instance of
// ESAT_WifiSubsystemClass.  Register the Wifi subsystem on the on-board
// data handling with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_WifiSubsystem).
class ESAT_WifiSubsystemClass: public ESAT_Subsystem
{
  public:
    // Start the communications subsystem.
    // Connect to the network and ground segment server.
    // Use the buffer to accumulate incoming telecommands from
    // main loop cycle to main loop cycle.
    void begin(byte buffer[], unsigned long bufferLength);

    // Return the identifier of this subsystem.
    word getApplicationProcessIdentifier();

    // Handle a telecommand.
    void handleTelecommand(ESAT_CCSDSPacket& packet);

    // Fill a packet with the next available telecommand.
    // Return true if there was a pending telecommand;
    // otherwise return false.
    boolean readTelecommand(ESAT_CCSDSPacket& packet);

    // Fill a packet with the next telemetry packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    boolean readTelemetry(ESAT_CCSDSPacket& packet);

    // Return true if there is new telemetry available;
    // Otherwise return false.
    boolean telemetryAvailable();

    // Update the subsystem.
    void update();

   // Send a telemetry packet to this subsystem.
    void writeTelemetry(ESAT_CCSDSPacket& packet);

  private:
    // Unique identifier of the subsystem.
    static const word APPLICATION_PROCESS_IDENTIFIER = 3;

    // Version number of the subsystem's interface.
    static const byte MAJOR_VERSION_NUMBER = 2;
    static const byte MINOR_VERSION_NUMBER = 1;
    static const byte PATCH_VERSION_NUMBER = 0;

    // Command code sent for connecting to the wireless network and
    // ground segment server.
    static const byte CONNECT = 0x00;

    // This line is pulled up when the Wifi board is not connected
    // to the server and it is pulled down when the Wifi board is
    // connected to the server.
    static const byte NOT_CONNECTED_SIGNAL_PIN = ESP0;

    // Pull down this line to reset the telemetry queue of the Wifi
    // board.
    static const byte RESET_TELEMETRY_QUEUE_SIGNAL_PIN = ESP_SLEEP;

    // Use this to read packets from the Wifi boad.
    ESAT_CCSDSPacketFromKISSFrameReader wifiReader;

    // Use this to write packets to the Wifi board.
    ESAT_CCSDSPacketToKISSFrameWriter wifiWriter;

    // Set up the connection sensor line.
    void beginConnectionSensor();

    // Configure the CCSDS-to-KISS bridge with the Wifi board.
    // Specify a buffer to accumulate the incomming telecommands.
    void beginWifiBridge(byte buffer[], unsigned long bufferLength);

    // Command the Wifi module to connect to the network and ground
    // segment server.
    void connect();

    // Return true if the Wifi board is connected to the server;
    // otherwise return false.
    boolean isConnected();
};

// Global instance of ESAT_WifiSubsystemClass.  Register WifiSubsystem
// on the on-board data handling module with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_WifiSubsystem).
extern ESAT_WifiSubsystemClass ESAT_WifiSubsystem;

#endif /* ESAT_WifiSubsystem_h */
