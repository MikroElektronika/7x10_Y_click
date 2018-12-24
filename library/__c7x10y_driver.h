/*

    __c7x10y_driver.h

    Copyright (c) 2011-2017 MikroElektronika.  All right reserved.

----------------------------------------------------------------------------- */

/**
@file   __c7x10y_driver.h
@brief    7x10_Y Driver
@mainpage 7x10_Y Click
@{
	
@image html libstock_fb_view.jpg

@}
@defgroup   C7X10Y 7x10_Y Driver
@brief      7x10_Y Click Driver
@{

| Global Library Prefix | **C7X10Y** |
|:---------------------:|:-----------------:|
| Version               | **1.0.0**    |
| Date                  | **Oct 2017.**      |
| Developer             | **MikroE Team**     |


### Library Descritption ###

*/
/**
    @example Click_7x10_Y_STM.c
    @example Click_7x10_Y_TIVA.c
    @example Click_7x10_Y_CEC.c
    @example Click_7x10_Y_KIN.c
    @example Click_7x10_Y_MSP.c
    @example Click_7x10_Y_PIC.c
    @example Click_7x10_Y_PIC32.c
    @example Click_7x10_Y_DSPIC.c
    @example Click_7x10_Y_AVR.c
    @example Click_7x10_Y_FT90x.c
    @example Click_7x10_Y_STM.mbas
    @example Click_7x10_Y_TIVA.mbas
    @example Click_7x10_Y_CEC.mbas
    @example Click_7x10_Y_KIN.mbas
    @example Click_7x10_Y_MSP.mbas
    @example Click_7x10_Y_PIC.mbas
    @example Click_7x10_Y_PIC32.mbas
    @example Click_7x10_Y_DSPIC.mbas
    @example Click_7x10_Y_AVR.mbas
    @example Click_7x10_Y_FT90x.mbas
    @example Click_7x10_Y_STM.mpas
    @example Click_7x10_Y_TIVA.mpas
    @example Click_7x10_Y_CEC.mpas
    @example Click_7x10_Y_KIN.mpas
    @example Click_7x10_Y_MSP.mpas
    @example Click_7x10_Y_PIC.pas
    @example Click_7x10_Y_PIC32.pas
    @example Click_7x10_Y_DSPIC.pas
    @example Click_7x10_Y_AVR.pas
    @example Click_7x10_Y_FT90x.pas

*/
/* -------------------------------------------------------------------------- */

#include "stdint.h"
#include "stdbool.h"

#ifndef _C7X10Y_H_
#define _C7X10Y_H_

/** @section                        C7x10Y_DCC Driver Compilation Config
 *  @{                        *///----------------------------------------------

   #define   __C7X10Y_DRV_SPI__                                            
//  #define   __C7X10Y_DRV_I2C__                                            
// #define   __C7X10Y_DRV_UART__  

#define T_C7X10Y_P    const uint8_t*

/** @section                                 C7x10Y_VAR Driver Variables
 *  @{                                 *///-------------------------------------

extern const uint8_t _C7X10Y_SPEED_SLOW;
extern const uint8_t _C7X10Y_SPEED_MED;
extern const uint8_t _C7X10Y_SPEED_FAST;

/// @}
/** @section                                    C7x10Y_TYPES Click Types
 *  @{                                    *///----------------------------------

// ...




/// @}

#ifdef __cplusplus
extern "C"{
#endif

/** @section                           C7x10Y_INIT Driver Init Functions
 *  @{                           *///-------------------------------------------

#ifdef   __C7X10Y_DRV_SPI__
void c7x10y_spiDriverInit(T_C7X10Y_P gpioObj, T_C7X10Y_P spiObj);
#endif
#ifdef   __C7X10Y_DRV_I2C__
void c7x10y_i2cDriverInit(T_C7X10Y_P gpioObj, T_C7X10Y_P i2cObj, uint8_t slave);
#endif
#ifdef   __C7X10Y_DRV_UART__
void c7x10y_uartDriverInit(T_C7X10Y_P gpioObj, T_C7X10Y_P uartObj);
#endif

/// @}
/** @section                                C7x10Y_FUNC Driver Functions
 *  @{                                *///--------------------------------------

/**
 * @brief Refresh display
 *
 * Draws content of frame buffer.
 *
 * @note 
 * Needs to be called as frequently as possible.
 */
bool c7x10y_refreshDisplay( void );

/**
 * @brief Clear display
 *
 * Clears content of frame buffer.
 */
void c7x10y_clearDisplay( void );

/**
 * @brief Draws a pixel
 *
 * Pixel provided will be placed inside frame buffer.
 *
 * @param[in] row   matrix row
 * @param[in] col   matrix column
 *
 * @note 
 * Minimum row value is 1 Maximum is 7
 * Minimum col value is 1 Maximum is 10
 */
void c7x10y_drawPixel( uint8_t row, uint8_t col );

/**
 * @brief Write Text
 *
 * Text will be placed inside internal text buffer, but first two 
 * characters are automaticly placen inside frame buffer.
 *
 * @param[in] txt    text to be written
 */
void c7x10y_drawText( char *txt );

/**
 * @brief Draws numbers
 *
 * Provided numbers are converted and placed inside frame buffer.
 *
 * @param[in] num    number to draw
 */
void c7x10y_drawNumber( uint8_t num );

/**
 * @brief Enable scrolling feature
 *
 * This operation requires calling of ```c7x10y_tick``` function after each 
 * scroll step or from interrupt routine.
 *
 * @param[in] speed    scrolling speed 
 *
 * @note Scrolling speed depends of provided speed and already defined constants
 * should be used as argument. Also scroll speed depends of ```c7x10y_tick``` calling
 * frequency.
 */
void c7x10y_scrollEnable( uint8_t speed );

/**
 * @brief Disables scrolling
 */
void c7x10y_scrollDisable( void );

/**
 * @brief Tick called to control scrolling speed
 *
 * This function is used on the scrolling feature.  It is to be called
 * after starting the scroll and around each display refresh. It does not
 * need to be called if scrolling is not needed.
 */
void c7x10y_tick( void );

///@}

#ifdef __cplusplus
} // extern "C"
#endif
#endif

/// @}
/* -------------------------------------------------------------------------- */
/*
  __C7X10Y_driver.h

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