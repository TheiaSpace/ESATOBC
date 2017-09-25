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

#ifndef ESATClock_h
#define ESATClock_h

#include <Energia.h>

// Real-time clock.
// The underlying hardware is the DS1338 serial real-time clock
// mounted on the ESATOBC board.
// Communications are done through the OBC I2C bus.
class ESATClock
{
  public:
    // True if the clock is alive.
    boolean alive;

    // Start the clock.
    void begin();

    // Read the current time.
    // Return the date and time in ISO 8601 format.
    String read();

    // Set the current time.
    // Pass the date and time in ISO 8601 format.
    void write(String time);

  private:
    static const byte address = 0x68;
    static const byte timeRegister = 0;

    // BCD to binary conversion.
    byte BCDToBinary(byte value);

    // Binary to BCD conversion.
    byte binaryToBCD(byte value);

    // Format a BCD number as a string with leading zeros.
    String format(byte number, byte length);
};

extern ESATClock Clock;

#endif
