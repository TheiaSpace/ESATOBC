/*
 * Copyright (C) 2019 Theia Space, Universidad Politécnica de Madrid.
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

#include "ESAT_ThermalPayloadSubsystem.h"

// Global variable to select if the Thermal Payload is registered as a
// subsystem or not. This implies to use or leave unused and not
// initialized the OBC microcontroller pins used for the payload.
boolean ESAT_ThermalPayloadSubsystemClass::enabled = true;

// We use the ADC14 pin from the header.
static const int ADC_PIN = ADC14;

// Number of temperature samples taken to get the mean value.
static const byte NUMBER_OF_SAMPLES = 10;

// Analog to digital converter resolution.
static const int ADC_RESOLUTION = 4095;

// Variable to store the current temperature.
static float temperature;

// We use the GPIO0 or GPIO2(PWM) from selection jumper to
// control the heater.
static const int HEATER_CONTROL_PIN = GPIO2;

// Current HEATER status.
static byte heaterStatus;

// We define the temperature thresholds.
static float targetTemperature;
static float allowedTemperatureDeviation;

// Payload modes.
static const byte MODE_NOMINAL = 1;
static const byte MODE_STANDBY = 0;

// Current mode: MODE_NOMINAL or NODE_STANDBY.
static byte mode;

// The telemetry packet sequence count is incremented every time a
// new telemetry packet is generated.
static word telemetryPacketSequenceCount;

// Number of Telemetry packets pending to dispatch per update () cycle.
static word pendingTelemetryPackets;

// Telecommand identifiers.
// Unique identifier of the subsystem.
static const word APPLICATION_PROCESS_IDENTIFIER = 4;

// Version numbers.
static const byte MAJOR_VERSION_NUMBER = 1;
static const byte MINOR_VERSION_NUMBER = 0;
static const byte PATCH_VERSION_NUMBER = 0;

// This function returns the mean value of the NUMBER_OF_SAMPLES
// samples of the ADC_PIN.
unsigned int readADCPin()
{
  unsigned long meanValue = 0;
  for (byte sample = 0; sample < NUMBER_OF_SAMPLES; sample++)
  {
    meanValue = meanValue + analogRead(ADC_PIN);
  }
  meanValue = meanValue / NUMBER_OF_SAMPLES;
  return meanValue;
}

// Return the current heater temperature.
float readTemperature()
{
  // Read ADC value.
  unsigned int ADCValue;
  ADCValue = readADCPin();
  // Get r to use the temperature expression.
  float r = float(ADCValue) / float(ADC_RESOLUTION - ADCValue);
  // Use the expression to get the temperature.
  float T;
  const float A_1 = 3.354016E-03;
  const float B_1 = 2.569850E-04;
  const float C_1 = 2.620131E-06;
  const float D_1 = 6.383091E-08;
  T = A_1
    + B_1 * log(r)
    + C_1 * pow(log(r), 2)
    + D_1 * pow(log(r), 3);
  // Temperature in kelvin.
  T = 1 / T;
  return T;
}

// Turn ON/OFF the heater.
void writeHeater(int status)
{
  switch(status)
  {
    case HIGH:
      digitalWrite(HEATER_CONTROL_PIN, HIGH);
      heaterStatus = HIGH;
      break;
    case LOW:
      digitalWrite(HEATER_CONTROL_PIN, LOW);
      heaterStatus = LOW;
      break;
  }
}

// Set the current operation mode of the payload.
void setMode(byte theMode)
{
  switch(theMode)
  {
    case MODE_NOMINAL:
      mode = MODE_NOMINAL;
      break;
    case MODE_STANDBY:
      mode = MODE_STANDBY;
      writeHeater(LOW);
      break;
    default:
      break;
  }
}

// Start the subsystem.
// This will be called once, during setup().
void ESAT_ThermalPayloadSubsystemClass::begin()
{
  // We set the HEATER_CONTROL_PIN as an output.
  pinMode(HEATER_CONTROL_PIN, OUTPUT);
  // Let's start with in stand-by mode.
  setMode(MODE_STANDBY);
  // We initialize the temperature thresholds.
  targetTemperature = 273.15 + 28.0;
  allowedTemperatureDeviation = 1.0;
  // We start the sequence count.
  telemetryPacketSequenceCount = 0;
  // We start with no Telemetry packets to dispatch.
  pendingTelemetryPackets = 0;
}

// Return the application process identifier of this subsystem.
// Each subsystem must have a unique 11-bit application process
// identifier.
// OnBoardDataHandling uses this to identify the subsystem.
word ESAT_ThermalPayloadSubsystemClass::getApplicationProcessIdentifier()
{
  return APPLICATION_PROCESS_IDENTIFIER;
}

// Handle a telecommand.
// ESAT_OnBoardDataHandling.dispatchTelecommand() forwards
// telecommands to their respective ESATSubsystem modules,
// identified by the value returned by
// getApplicationProcessIdentifier().
void ESAT_ThermalPayloadSubsystemClass::handleTelecommand(ESAT_CCSDSPacket& packet)
{
  // We will see how to handle a telecommand packet in coming sessions.
  // A command to stop and start the blinking of the LED would be
  // interesting, isn't it?
  packet.rewind();
  // We extract the primary header.
  ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
  // We can do some checks with the primary header.  For example:
  // is the application process identifier the correct one?
  if (primaryHeader.applicationProcessIdentifier
      != getApplicationProcessIdentifier())
  {
    return;
  }
  // Is this packet indeed a telecommand?
  if (primaryHeader.packetType != primaryHeader.TELECOMMAND)
  {
    return;
  }
  // Is the packet length correct?
  // The packet data length has to be at least the length of the
  // secondary header.
  if (primaryHeader.packetDataLength < ESAT_CCSDSSecondaryHeader::LENGTH)
  {
    return;
  }
  // We extract the secondary header.
  ESAT_CCSDSSecondaryHeader secondaryHeader =
    packet.readSecondaryHeader();
  // We check that the packet has a valid version number.
  if (secondaryHeader.majorVersionNumber < MAJOR_VERSION_NUMBER)
  {
    return;
  }
  // And use the packet identifier to process the telecommand.
  switch (secondaryHeader.packetIdentifier)
  {
    case 0:
      setMode(packet.readByte());
      break;
    case 1:
      targetTemperature = packet.readFloat();
      break;
    default:
      break;
  }
}

// Fill a packet with the next telecommand packet available.
// Return true if the operation was successful;
// otherwise return false.
// Called from ESAT_OnBoardDataHandling.readTelecommand().
// This function will be called at least once on each loop() cycle
// and again and again as long as it returns true, so make sure to
// return false when there are no telecommand packets ready: for
// example, if there is only one telecommand packet per cycle, the
// first call to readTelecommand() has to return true to signal
// that there was a packet and the next call has to return false
// to signal that there were no more packets.
boolean ESAT_ThermalPayloadSubsystemClass::readTelecommand(ESAT_CCSDSPacket& packet)
{
  // Nothing to do here.
  return false;
}

// Fill a packet with the next telemetry packet available.
// Return true if the operation was successful; otherwise return
// false.
// Called from ESAT_OnBoardDataHandling.readSubsystemsTelemetry().
// This function will be called at least once on each loop() cycle
// and again and again as long as it returns true, so make sure
// to return false when there are no telemetry packets ready: for
// example, if there is only one telemetry packet per cycle, the
// first call to readTelemetry() has to return true to signal that
// there was a packet and the next call has to return false to
// signal that there were no more packets.
boolean ESAT_ThermalPayloadSubsystemClass::readTelemetry(ESAT_CCSDSPacket& packet)
{
  if (pendingTelemetryPackets > 0)
  {
    // We clear the information in the previous packet.
    packet.flush();
    // We create and fill the primary header.
    ESAT_CCSDSPrimaryHeader primaryHeader;
    primaryHeader.packetVersionNumber = 0;
    primaryHeader.packetType =
      primaryHeader.TELEMETRY;
    primaryHeader.secondaryHeaderFlag =
      primaryHeader.SECONDARY_HEADER_IS_PRESENT;
    primaryHeader.applicationProcessIdentifier =
      getApplicationProcessIdentifier();
    primaryHeader.sequenceFlags =
      primaryHeader.UNSEGMENTED_USER_DATA;
    primaryHeader.packetSequenceCount =
      telemetryPacketSequenceCount;
    packet.writePrimaryHeader(primaryHeader);
    // We create and fill the secondary header.
    ESAT_CCSDSSecondaryHeader secondaryHeader;
    secondaryHeader.preamble =
      secondaryHeader.CALENDAR_SEGMENTED_TIME_CODE_MONTH_DAY_VARIANT_1_SECOND_RESOLUTION;
    secondaryHeader.timestamp = ESAT_OBCClock.read();
    secondaryHeader.majorVersionNumber = MAJOR_VERSION_NUMBER;
    secondaryHeader.minorVersionNumber = MINOR_VERSION_NUMBER;
    secondaryHeader.patchVersionNumber = PATCH_VERSION_NUMBER;
    secondaryHeader.packetIdentifier = 0;
    packet.writeSecondaryHeader(secondaryHeader);
    // Finally, we define the user data.
    packet.writeByte(mode);
    packet.writeFloat(temperature);
    packet.writeByte(heaterStatus);
    packet.writeFloat(targetTemperature);
    // We update the telemetryPacketSequenceCount.
    telemetryPacketSequenceCount = telemetryPacketSequenceCount + 1;
    // We return true because we filled
    // a telemetry packet.
    pendingTelemetryPackets = pendingTelemetryPackets - 1;
    return true;
  }
  else
  {
    // We return false because we didn't fill a telemetry packet.
    return false;
  }
}

// Update the subsystem.
// Called from OnBoardDataHandling.updateSubsystems().
// This function will be called once per loop() cycle.
// If this subsystem has counters for keeping track of packets
// pending to be read with readTelecommand() or readTelemetry(),
// this is a good place to reset them.
void ESAT_ThermalPayloadSubsystemClass::update()
{
  if (mode == MODE_NOMINAL)
  {
    temperature = readTemperature();
    if (temperature < (targetTemperature - allowedTemperatureDeviation))
    {
      writeHeater(HIGH);
    }
    if (temperature > (targetTemperature + allowedTemperatureDeviation))
    {
      writeHeater(LOW);
    }
    // This is the number of telemetry packets to dispatch per
    // update() cycle with the Thermal Payload in NOMINAL mode.
    pendingTelemetryPackets = 2;
  }
  else if (mode == MODE_STANDBY)
  {
    // This is the number of telemetry packets to dispatch per
    // update() cycle with the Thermal Payload in STANDBY mode.
    pendingTelemetryPackets = 1;
  }
  else
  {
    // If we ended in an unknown mode, don't dispatch telemetry
    // packets.
    pendingTelemetryPackets = 0;
  }
}

// Send a telemetry packet to this subsystem.
// Called from OnBoardDataHandling.writeTelemetry().
void ESAT_ThermalPayloadSubsystemClass::writeTelemetry(ESAT_CCSDSPacket& packet)
{
  // Nothing to do here.
}

// Unique global instance of ESAT_ThermalPayloadSubsystemClass.
ESAT_ThermalPayloadSubsystemClass ESAT_ThermalPayloadSubsystem;
