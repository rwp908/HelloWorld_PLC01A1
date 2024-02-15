/**
 ******************************************************************************
 * @file    main.cpp
 * @author  AST/CLs
 * @version V1.1.0
 * @date    February 23rd, 2016
 * @brief   mbed test application for the STMicroelectronics X-NUCLEO-PLC01A1
 *          PLC Expansion Board.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/

/* expansion board specific header files. */
#include "XNucleoPLC01A1.h"


/* Definitions ---------------------------------------------------------------*/

/* Uncomment this for OUTPUT_CYCLING ENABLE */
//#define OUTPUT_CYCLING


/* Variables -----------------------------------------------------------------*/

/* Array for input data from Digital Input Termination Device */
uint8_t input_array[2] = {0x00, 0x00};
/* Array for output data to Solid State Relay */
uint8_t output_array[2] = {0x00, 0x00};

/* Number of channels in ON state */
uint8_t ch_on = 0x00;


/* Functions -----------------------------------------------------------------*/

/** 
  * @brief  Receive input data from Digital Input Termination Device
  * @param  None
  * @retval None
*/
void digital_input_array_handler(XNucleoPLC01A1 &plc)
{
  plc.plc_input().dig_inp_array_get_input(input_array);
}

/** 
  * @brief Select output function and set outputs
  * @param None
  * @retval None
*/
void ssrelay_handler(XNucleoPLC01A1 &plc)
{
  /* Set output_array as DigInpArray RxBuffer */
  output_array[1] = plc.signal_mirror(input_array[1]);
  
  /* Uncomment the relevant function as required */                                                       
  //output_array[1] = plc.signal_mirror(0xFF);
  //output_array[1] = plc.output_freeze(0xFF,5000);
  //output_array[1] = plc.output_regroup(0xFF);
  //ch_on = plc.input_sum(&output_array[1],0xFF);
  //output_array[1] = plc.set_output(0xFF);
  //output_array[1] = plc.inputs_and(0xFF,0x0F);
  //output_array[1] = plc.inputs_or(0xF0,0x0F);
  //output_array[1] = plc.inputs_not(0x00);
  //output_array[1] = plc.inputs_xor(0xFF,0x00);
  
  /* Parity bits calculation */
  plc.output_parity_bits(output_array);
 
  /* Send output information to solid state relay */
  plc.plc_output().ssrelay_set_output(output_array);
}

void setup(SPI &spi, int bits, int mode = 0, int frequency_hz = 1E6)
{
    /* Set given configuration. */
    spi.format(bits, mode);
    spi.frequency(frequency_hz);
}


/* Main ----------------------------------------------------------------------*/

int main()
{
    /*----- Initialization. -----*/

    /* Printing to the console. */
    printf("PLC Control Application Example\r\n\n");
        
    /* Initializing SPI bus. */
    SPI spi(X_NUCLEO_PLC01A1_PIN_SPI_MOSI, X_NUCLEO_PLC01A1_PIN_SPI_MISO, X_NUCLEO_PLC01A1_PIN_SPI_SCLK);
    setup(spi, X_NUCLEO_PLC01A1_PIN_SPI_BITS);

    /* Initializing X_NUCLEO_PLC01A1 IO Channels Component. */
    XNucleoPLC01A1 plc(X_NUCLEO_PLC01A1_PIN_SPI_CS1, X_NUCLEO_PLC01A1_PIN_SPI_CS2, X_NUCLEO_PLC01A1_PIN_OUT_EN, spi);

    while(1) {
        plc.plc_input().set_read_status(1);
        /* Polling input device to refresh input state */
        if(plc.plc_input().get_read_status()) {

            plc.plc_input().set_read_status(0);

#ifdef OUTPUT_CYCLING
            plc.output_cycling();
#else
            digital_input_array_handler(plc);
            ssrelay_handler(plc);
#endif /* OUTPUT_CYCLING */
        }
        ThisThread::sleep_for(chrono::milliseconds(10));
    }
}
