/*
  BrushlessMotor

  Runs a brushless motor for one second.

   This file is part of BreezySTM32.

   BreezySTM32 is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   BreezySTM32 is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with BreezySTM32.  If not, see <http://www.gnu.org/licenses/>.
  */

#include <Arduino.h>
#include <Motor.h>

BrushlessMotor motor;

static uint16_t speed;
static int16_t  direction;

void setup() {                

    motor.attach(8);

    delay(100);

    speed = 1100;
    direction = +1;
}

void loop() {

    motor.setSpeed(speed);

    speed += direction;

    if (speed == 1200)
        direction = -1;

    if (speed == 1100)
        direction = +1;

    delay(10);

}