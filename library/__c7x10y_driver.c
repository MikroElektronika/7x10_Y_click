/*

    __c7x10y_driver.c

    Copyright (c) 2011-2017 MikroElektronika.  All right reserved.

----------------------------------------------------------------------------- */

#include "__c7x10y_driver.h"
#include "__c7x10y_hal.c"
#include "7x5font.h"

/* ------------------------------------------------------------------- MACROS */

#define C7X10Y_SPEED_SLOW  0
#define C7X10Y_SPEED_MED   1
#define C7X10Y_SPEED_FAST  2

/* ---------------------------------------------------------------- VARIABLES */

#ifdef   __C7X10Y_DRV_I2C__
static uint8_t _slaveAddress;
#endif

const uint8_t _C7X10Y_SPEED_SLOW = 0;
const uint8_t _C7X10Y_SPEED_MED  = 1;
const uint8_t _C7X10Y_SPEED_FAST = 2;

static bool    scroll;
static uint8_t frame[7][2];
static uint8_t textFrame[7][40];
static uint8_t textLength;

/**
 * Number of ticks for controlling scroll speed.
 */
static volatile uint16_t timerTick;
static uint8_t scrollSpeed;

/* -------------------------------------------- PRIVATE FUNCTION DECLARATIONS */

static void _memset( uint8_t* ptr, uint8_t value, uint16_t len );
static uint16_t _strlen( char* s );

static void latch( void );
static void rowReset( void );
static void rowClock( void );
static void moduleReset( void );

static void drawFrame( void );
static void shiftFrame( void );

/* --------------------------------------------- PRIVATE FUNCTION DEFINITIONS */

static void _memset( uint8_t* input, uint8_t value, uint16_t len )
{
    uint8_t* ptr = input;

    while (0 != len--)
    {
        *ptr++ = value;
    }
}

static uint16_t _strlen( char *s )
{
    uint16_t c = 0;

    while (0 != *s)
    {
        s++;
        c++;
    }

    return c;
}

static void latch( void )
{
    hal_gpio_csSet( 0 );
    Delay_1us();
    hal_gpio_csSet( 1 );
    Delay_1ms();
}

static void rowReset( void )
{ 
    hal_gpio_pwmSet( 1 );
    Delay_1us();
    hal_gpio_pwmSet( 0 );
}

static void rowClock( void )
{
    hal_gpio_anSet( 1 );
    Delay_1us();
    hal_gpio_anSet( 0 );
}

static void moduleReset( void )
{
    hal_gpio_rstSet( 0 );
    Delay_1us();
    hal_gpio_rstSet( 1 );
}

static void drawFrame( uint8_t* fr )
{
    int i;
    uint8_t *ptr = fr;

    moduleReset();
    rowReset();
    
    for (i = 0; i < 7; i++)
    {
        hal_spiWrite( ptr + (i * 2), 2 );
        latch();
        rowClock();
    }
}

/* 
 * 1. Shift right 1 from index 1
 * 2. Capture shifted bit from index 1
 * 3. Mask 0x40 from index 0 and or captured value
 * 4. Shift index 0 1
 */
static void shiftFrame()
{
    static uint16_t position = 0;
    static uint8_t character = 0;
    uint8_t carry;
    int row, col;

    for (row = 0; row < 7; row++)
    {
        for (col = 1; col > -1; col--)
        {
            frame[row][col] >>= 1;           // Pop the bit off

            if (1 == col)
            {
                carry = frame[row][0];
                carry &= 0x01;
                carry <<= 4;
            }
            else
            {
                int tmp = !( character % 2 ) ? character + 3 : character + 1;
                carry = textFrame[row][tmp];
                carry &= ( 1 << position );
                carry <<= ( 4 - position );
            }

            frame[row][col] |= carry;
        }
    }

    position++;

    if (4 < position)
    {
        position = 0;
        character++;

        if (character > textLength)
        {
            for (row = 0; row < 7; row++)
            {
                for (col = 0; col < 2; col++)
                {
                    frame[row][col] = textFrame[row][col];
                }
            }
            character = 0;
            scroll = false;
        }
    }
}

/* --------------------------------------------------------- PUBLIC FUNCTIONS */

#ifdef   __C7X10Y_DRV_SPI__

void c7x10y_spiDriverInit(T_C7X10Y_P gpioObj, T_C7X10Y_P spiObj)
{
    hal_spiMap( (T_HAL_P)spiObj );
    hal_gpioMap( (T_HAL_P)gpioObj );
    moduleReset();
}

#endif
#ifdef   __C7X10Y_DRV_I2C__

void c7x10y_i2cDriverInit(T_C7X10Y_P gpioObj, T_C7X10Y_P i2cObj, uint8_t slave)
{
    _slaveAddress = slave;
    hal_i2cMap( (T_HAL_P)i2cObj );
    hal_gpioMap( (T_HAL_P)gpioObj );

    // ... power ON
    // ... configure CHIP
}

#endif
#ifdef   __C7X10Y_DRV_UART__

void c7x10y_uartDriverInit(T_C7X10Y_P gpioObj, T_C7X10Y_P uartObj)
{
    hal_uartMap( (T_HAL_P)uartObj );
    hal_gpioMap( (T_HAL_P)gpioObj );

    // ... power ON
    // ... configure CHIP
}

#endif

/* ----------------------------------------------------------- IMPLEMENTATION */

bool c7x10y_refreshDisplay()
{
    if (scroll)
    {
        switch (scrollSpeed)
        {
            case C7X10Y_SPEED_SLOW:
                if (timerTick > 7)
                {
                    shiftFrame();
                    timerTick = 0;
                }
            break;
            case C7X10Y_SPEED_MED:
                if (timerTick > 5)
                {
                    shiftFrame();
                    timerTick = 0;
                }
            break;
            case C7X10Y_SPEED_FAST:
                if (timerTick > 1)
                {
                    shiftFrame();
                    timerTick = 0;
                }
            break;
        }

        drawFrame( frame );
        return scroll;
    } 
    else 
    {
        drawFrame( frame );
        return true;
    }
}

void c7x10y_clearDisplay()
{
    _memset( &frame[0][0], 0, 14 );
}

void c7x10y_drawPixel( uint8_t row, uint8_t col )
{
    if (row > 7 || col > 10 || row == 0 || col == 0)
    {
        return;
    }

    if (6 > col)
    {
        frame[ row - 1 ][ 1 ] |= 0x01 << ( col - 1 );
    }
    else
    {
        frame[ row - 1 ][ 0 ] |= 0x01 << ( col - 6 );
    }
}

void c7x10y_drawText( char* txt )
{
    char *ptext = txt;
    uint8_t const *pfont = font;
    int i, j, charCount;
    textLength = _strlen( txt );

    if (40 < textLength)
    {
        textLength = 40;
    }

    _memset( textFrame, 0, sizeof( textFrame ) );

    for (charCount = 0; charCount < textLength; charCount++)
    {
        pfont = &font[ *( ptext++ ) * 8 ];

        if (!( charCount % 2 )) 
        {
            for (i = 0; i < 7; i++)
            {
                textFrame[i][ ( charCount + 1 ) ] = *( pfont++ );
            }
        } 
        else 
        {
            for (i = 0; i < 7; i++)
            {
                textFrame[i][ ( charCount - 1 ) ] = *( pfont++ );
            }
        }
    }

    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < 2; j++)
        {
            frame[i][j] = textFrame[i][j];
        }
    }
}


void c7x10y_drawNumber( uint8_t num )
{
    uint8_t const *pfont = font;
    int i;

    if (99 < num)
    {
        return;
    }

    if (10 > num)
    {
        pfont = &font[( num + 48 ) * 8 ];

        for (i = 0; i < 7; i++)
        {
            frame[i][0] = *( pfont++ );
        }
    } 
    else 
    {
        int tmpNum = num / 10;
        pfont = &font[( tmpNum + 48 ) * 8 ];

        for (i = 0; i < 7; i++)
        {
            frame[i][1] = *( pfont++ );
        }

        tmpNum = num % 10;
        pfont = &font[( tmpNum + 48 ) * 8 ];

        for (i = 0; i < 7; i++)
        {
            frame[i][0] = *( pfont++ );
        }
    }
}

void c7x10y_scrollEnable( uint8_t speed )
{
    scroll = true;
    scrollSpeed = speed;
    timerTick = 0;
}

void c7x10y_scrollDisable()
{
    scroll = false;
}

void c7x10y_tick()
{
    timerTick++;
}

/* -------------------------------------------------------------------------- */
/*
  __C7X10Y_driver.c

  Copyright (c) 2011-2017 MikroElektronika.  All right reserved.

    This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.

----------------------------------------------------------------------------- */
