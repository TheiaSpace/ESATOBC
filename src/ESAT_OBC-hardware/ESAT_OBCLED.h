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

#ifndef ESAT_OBCLED_h
#define ESAT_OBCLED_h

#include <Arduino.h>

// Heartbeat LED on the OBC board.
// Use the global instance ESAT_OBCLED.
class ESAT_OBCLEDClass
{
  public:
    // Configure the LED control line.
    void begin();

    // Toggle the state of the LED.
    void toggle();

  private:
    // Pin number of the LED control line.
    static const int LED_CONTROL_LINE = LED_O;

    // Next state of the LED.
    int nextState;
};


// Global instance of the OBC LED library.
extern ESAT_OBCLEDClass ESAT_OBCLED;

#endif /* ESAT_OBCLED_h */
