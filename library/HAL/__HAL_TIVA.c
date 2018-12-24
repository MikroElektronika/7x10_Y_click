/*

    __HAL_TIVA.c

    MikroC Pro for ARM - TIVA MCUs

    Copyright (c) 2011-2017 MikroElektronika.  All right reserved.

----------------------------------------------------------------------------- */

#include "stdint.h"

#ifdef __MIKROC_PRO_FOR_ARM__
#ifdef __TI__
#ifdef __HAL_SPI__

typedef void          ( *T_hal_spiWriteFp )(unsigned int);
typedef unsigned int  ( *T_hal_spiReadFp )(unsigned int);

typedef struct
{
    T_hal_spiWriteFp       spiWrite;
    T_hal_spiReadFp        spiRead;
  
}T_hal_spiObj;

static T_hal_spiWriteFp        fp_spiWrite;                             
static T_hal_spiReadFp         fp_spiRead;                              

static void hal_spiMap(T_HAL_P spiObj)
{
    T_HAL_SPI_OBJ tmp = (T_HAL_SPI_OBJ)spiObj;

    fp_spiWrite = tmp->spiWrite;
    fp_spiRead  = tmp->spiRead;
}

static void hal_spiWrite(uint8_t *pBuf, uint16_t nBytes)
{
    uint8_t *ptr = pBuf;
    while( nBytes-- )
        fp_spiWrite( *( ptr++ ) );
}

static void hal_spiRead(uint8_t *pBuf, uint16_t nBytes)
{
    uint8_t *ptr = pBuf;
    while( nBytes-- )
        *( ptr++ ) = fp_spiRead( 0x00 );
}

static void hal_spiTransfer(uint8_t *pIn, uint8_t *pOut, uint16_t nBytes)
{
    uint8_t *in = pIn;
    uint8_t *out = pOut;
    while( nBytes-- )
        *( out++ ) = fp_spiRead( *( in++ ) );
}

#endif
#ifdef __HAL_I2C__

static uint8_t startF           = 0;

typedef void            ( *T_hal_i2cStartFp )(char, char);
typedef unsigned char   ( *T_hal_i2cWriteFp )(unsigned char, unsigned char);
typedef unsigned char   ( *T_hal_i2cReadFp )(unsigned char*, unsigned char);

typedef struct
{
    T_hal_i2cStartFp       i2cStart;
    T_hal_i2cWriteFp       i2cWrite;
    T_hal_i2cReadFp        i2cRead;
  
}T_hal_i2cObj;

static T_hal_i2cStartFp        fp_i2cStart;                            
static T_hal_i2cWriteFp        fp_i2cWrite;                            
static T_hal_i2cReadFp         fp_i2cRead;                             

static void hal_i2cMap(T_HAL_P i2cObj)
{
    T_HAL_I2C_OBJ tmp = (T_HAL_I2C_OBJ)i2cObj;

    fp_i2cStart    = tmp->i2cStart;
    fp_i2cWrite    = tmp->i2cWrite;
    fp_i2cRead     = tmp->i2cRead;
}

static int hal_i2cStart()
{
    int res = 0;
    startF = 1;
    return res;
}

static int hal_i2cWrite(uint8_t slaveAddress, uint8_t *pBuf, uint16_t nBytes, uint8_t endMode)
{
    int res = 0;
    uint8_t *ptr = pBuf;

    if( startF ) {
        fp_i2cStart(slaveAddress, _I2C_DIR_MASTER_TRANSMIT);
        Delay_1ms();
        startF = 0;
    }
    if(nBytes == 1) {
        res |= fp_i2cWrite( *ptr, _I2C_MASTER_MODE_SINGLE_SEND );
        Delay_1ms();
    } else if(nBytes == 2) {
        res |= fp_i2cWrite( *ptr++, _I2C_MASTER_MODE_BURST_SEND_START );
        Delay_1ms();
        res |= fp_i2cWrite( *ptr, _I2C_MASTER_MODE_BURST_SEND_FINISH );
    } else {
        res |= fp_i2cWrite( *ptr++, _I2C_MASTER_MODE_BURST_SEND_START );
        Delay_1ms();
        while(--nBytes > 1) {
            res |= fp_i2cWrite( *ptr++, _I2C_MASTER_MODE_BURST_SEND_CONT );
            Delay_1ms();
        }
        res |= fp_i2cWrite( *ptr, _I2C_MASTER_MODE_BURST_SEND_FINISH );
    }
    if( endMode == END_MODE_RESTART ){
        startF = 1;
    }
    return res;
}

static int hal_i2cRead(uint8_t slaveAddress, uint8_t *pBuf, uint16_t nBytes, uint8_t endMode)
{
    int res = 0;
    uint8_t *ptr = pBuf;

    if( startF ) {
        fp_i2cStart(slaveAddress, _I2C_DIR_MASTER_RECEIVE);
        Delay_1ms();
        startF = 0;
    }
    if(nBytes == 1) {
        res |= fp_i2cRead(ptr, _I2C_MASTER_MODE_SINGLE_RECEIVE);
        Delay_1ms();
    } else if(nBytes == 2) {
        res |= fp_i2cRead(ptr++, _I2C_MASTER_MODE_BURST_RECEIVE_START);
        Delay_1ms();
        res |= fp_i2cRead(ptr, _I2C_MASTER_MODE_BURST_RECEIVE_FINISH);
    } else {
        res |= fp_i2cRead(ptr++ , _I2C_MASTER_MODE_BURST_RECEIVE_START);
        Delay_1ms();
        while(--nBytes > 1) {
            res |= fp_i2cRead(ptr++ , _I2C_MASTER_MODE_BURST_RECEIVE_CONT);
            Delay_1ms();
        }
        res |= fp_i2cRead(ptr, _I2C_MASTER_MODE_BURST_RECEIVE_FINISH);
    }
    if( endMode == END_MODE_RESTART ) {
        startF = 1;
    }
    return res;
}

#endif
#ifdef __HAL_UART__

typedef void          ( *T_hal_uartWriteFp )(unsigned int);
typedef unsigned int  ( *T_hal_uartReadFp )();
typedef unsigned int  ( *T_hal_uartReadyFp )();

typedef struct
{
    T_hal_uartWriteFp      uartWrite;
    T_hal_uartReadFp       uartRead;
    T_hal_uartReadyFp      uartReady;
  
}T_hal_uartObj;

static T_hal_uartWriteFp       fp_uartWrite;                          
static T_hal_uartReadFp        fp_uartRead;                           
static T_hal_uartReadyFp       fp_uartReady;                          

static void hal_uartMap(T_HAL_P uartObj)
{
    T_HAL_UART_OBJ tmp = (T_HAL_UART_OBJ)uartObj;

    fp_uartWrite = tmp->uartWrite;
    fp_uartRead  = tmp->uartRead;
    fp_uartReady = tmp->uartReady;
}

static void hal_uartWrite(uint8_t input)
{
    fp_uartWrite(input);
}

static uint8_t hal_uartRead()
{
    return ( uint8_t )fp_uartRead();
}

static uint8_t hal_uartReady()
{
    return ( uint8_t )fp_uartReady();
}

#endif
#endif
#endif

/* -------------------------------------------------------------------------- */
/*
  __HAL_TIVA.c

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