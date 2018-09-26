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

#ifndef ESAT_OBCClock_h
#define ESAT_OBCClock_h

#include <Arduino.h>
#include <ESAT_Clock.h>

// Real-time clock.
// The underlying hardware is the DS1338 serial real-time clock
// mounted on the ESAT OBC board.
// Communications are done through the OBC I2C bus.  The I2C bus must
// be configured before using this library.  You must have called
// Wire.begin() before using this library.
// Use the global instance ESAT_OBCClock.
class ESAT_OBCClockClass: public ESAT_Clock
{
  public:
    // True on communication error.  Must be reset manually.
    boolean error;

    // Read the current time.
    // Return the date and time in ISO 8601 format.
    // Set the error flag on error.
    ESAT_Timestamp read();

    // Set the current time.
    // Pass the date and time in ISO 8601 format.
    // Set the error flag on error.
    void write(ESAT_Timestamp time);

  private:
    // I2C address of the clock chip.
    static const byte ADDRESS = 0x68;

    // The time is stored at this register.
    static const byte TIME_REGISTER = 0;
};

// Global instance of the OBC clock library.
extern ESAT_OBCClockClass ESAT_OBCClock;

#endif /* ESAT_OBCClock_h */
