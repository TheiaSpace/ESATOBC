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

// Interface to the ESAT Wifi subsystem from the point of view of the
// on-board data handling subsystem.  There is a global instance:
// ESAT_WifiSubsystem.  This should be the only instance of
// ESAT_WifiSubsystemClass.  Register the ESAT Wifi subsystem on the
// on-board data handling with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_WifiSubsystem).
class ESAT_WifiSubsystemClass: public ESAT_Subsystem
{
  public:
    // Start the communications subsystem.
    // Connect to the network and ground segment server.
    // Use the reader buffer to accumulate incoming
    // packets from main loop cycle to main loop cycle.
    // Use the optional packet data buffer to store the
    // packet data field of a full packet, so that it is
    // possible to accumulate one packet in the reader
    // buffer while there is another packet ready for use.
    // The optional packet data buffer is necessary to receive
    // telemetry from the ESAT Wifi board.
    void begin(byte readerBuffer[],
               unsigned long readerBufferLength,
               byte packetDataBuffer[] = nullptr,
               unsigned long packetDataBufferLength = 0);

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

    // Deprecated method; don't use it.
    // Return true if there is new telemetry available;
    // Otherwise return false.
    boolean telemetryAvailable() __attribute__((deprecated));

    // If ESAT_WifiSubsystem.begin() was called with the optional
    // packet data buffer, ask the ESAT Wifi board for a new series of
    // telemetry packets.
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

    // This line is pulled up when the ESAT Wifi board is not connected
    // to the server and it is pulled down when the ESAT Wifi board is
    // connected to the server.
    static const byte NOT_CONNECTED_SIGNAL_PIN = ESP0;

    // Pull down this line to reset the telemetry queue of the ESAT Wifi
    // board.
    static const byte RESET_TELEMETRY_QUEUE_SIGNAL_PIN = ESP_SLEEP;

    // Command code for setting the time of the EPS clock.
    static const byte SET_CURRENT_TIME = 0x30;

    // Number of microseconds to wait for the telemetry queue reset
    // condition to propagate.
    static const byte TELEMETRY_QUEUE_RESET_DELAY = 100;

    // Use this to store packets temporarily.
    // If we receive a telemetry packet during a call to readTelecommand(),
    // the telemetry packet will go here.
    // If we receive a telecommand packet during a call to readTelemetry(),
    // the telecommand packet will go here.
    ESAT_CCSDSPacket bufferedPacket;

    // Set to true while we are receiving telemetry from the ESAT Wifi
    // board (after a call to beginReadingTelemetry()); set to false
    // when we aren't receiving telemetry from the ESAT Wifi board (before
    // the first call to beginReadingTelemetry() and after a failed
    // readTelemetry()).
    boolean readingTelemetry;

    // Use this to read packets from the ESAT Wifi boad.
    ESAT_CCSDSPacketFromKISSFrameReader wifiReader;

    // Use this to write packets to the ESAT Wifi board.
    ESAT_CCSDSPacketToKISSFrameWriter wifiWriter;

    // Set up the ESAT Wifi board control lines.
    void beginControlLines();

    // Start the reception of a new series of telemetry packets from
    // the ESAT Wifi board.
    void beginReadingTelemetry();

    // Configure the CCSDS-to-KISS bridge with the ESAT Wifi board.
    // Specify a buffer to accumulate incoming packets
    // and an additional buffer to store the received packet
    // so that it is possible to accumulate an incoming packet
    // while another one is stored.
    void beginWifiBridge(byte readerBuffer[],
                         unsigned long readerBufferLength,
                         byte packetDataBuffer[],
                         unsigned long packetDataBufferLength);

    // Command the ESAT Wifi module to connect to the network and ground
    // segment server.
    void connect();

    // Return true if the ESAT Wifi board is connected to the server;
    // otherwise return false.
    boolean isConnected();

    // Set the time of the Wifi board.
    void setTime();

    // Return true if the buffered packet already contains a
    // telecommand packet; otherwise return false;
    boolean telecommandAlreadyBuffered() const;

    // Return true if the buffered packet already contains a
    // telemetry packet; otherwise return false;
    boolean telemetryAlreadyBuffered() const;
};

// Global instance of ESAT_WifiSubsystemClass.  Register WifiSubsystem
// on the on-board data handling module with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_WifiSubsystem).
extern ESAT_WifiSubsystemClass ESAT_WifiSubsystem;

#endif /* ESAT_WifiSubsystem_h */
