/*
Arduino.h : Arduino API header for BreezySTM32 library

Copyright (C) 2017 Simon D. Levy 

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

extern "C" {

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t level);

unsigned long millis(void);
void delay(unsigned long);

void setup(void);
void loop(void);

class HardwareSerial {

    private:

        uint8_t _id;
        void *  _uart;

    protected:

        HardwareSerial(uint8_t id);

    public:

        void begin(uint32_t baud);

        uint8_t available(void);

        uint8_t read(void);

        void write(uint8_t byte);

        void flush(void);

        void printf(const char * fmt, ...)
        {
            va_list ap;       
            va_start(ap, fmt);     
            char buf[1000];
            vsprintf(buf, fmt, ap);
            for (char *p=buf; *p; p++)
               this->write(*p);
            va_end(ap);  
            this->flush();
        }


};

class HardwareSerial0 : public HardwareSerial {

    public:

        HardwareSerial0(void) : HardwareSerial(0) { }
};

extern HardwareSerial0 Serial;

class HardwareWire {

    public:

        void begin();

        void beginTransmission(uint8_t addr);

        int8_t write(uint8_t reg, uint8_t data);

        int8_t endTransmission(void);

        bool read(uint8_t addr_, uint8_t reg, uint8_t len, uint8_t *buf);
};

extern HardwareWire Wire;

} // extern "C"
