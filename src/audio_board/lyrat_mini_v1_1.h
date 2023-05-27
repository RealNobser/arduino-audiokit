/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#pragma once

#define AUDIO_DRIVER AUDIO_CODEC_ES8311_DEFAULT_HANDLE
#define AUDIO_DRIVER_ADC AUDIO_CODEC_ES7243_DEFAULT_HANDLE

#define PIN_AUDIO_KIT_SD_CARD_CS 13
#define PIN_AUDIO_KIT_SD_CARD_MISO 2
#define PIN_AUDIO_KIT_SD_CARD_MOSI 15
#define PIN_AUDIO_KIT_SD_CARD_CLK  14

#define SDCARD_OPEN_FILE_NUM_MAX  5
#define SDCARD_INTR_GPIO          34
#define SDCARD_PWR_CTRL           13
#define ES7243_MCLK_GPIO          0

// I2S
#define PIN_I2S_AUDIO_KIT_MCLK 0
#define PIN_I2S_AUDIO_KIT_BCK 5
#define PIN_I2S_AUDIO_KIT_WS 25
#define PIN_I2S_AUDIO_KIT_DATA_OUT 26
#define PIN_I2S_AUDIO_KIT_DATA_IN 35
#define PIN_I2S_AUDIO_KIT_MCLK1 0
#define PIN_I2S_AUDIO_KIT_BCK1 32
#define PIN_I2S_AUDIO_KIT_WS1 33
#define PIN_I2S_AUDIO_KIT_DATA_OUT1 -1
#define PIN_I2S_AUDIO_KIT_DATA_IN1 36


// I2C
#define I2C_MASTER_NUM I2C_NUM_0 /*!< I2C port number for master dev */
#define I2C_MASTER_SCL_IO 23     
#define I2C_MASTER_SDA_IO 18    

#define PIN_KEY1 5
#define PIN_KEY2 4
#define PIN_KEY3 2
#define PIN_KEY4 3
#define PIN_KEY5 1
#define PIN_KEY6 0


#define ES8311_MCLK_SOURCE        0   /* 0 From MCLK of esp32   1 From BCLK */

#define HEADPHONE_DETECT          19
#define PA_ENABLE_GPIO            21

#define BLUE_LED_GPIO             27
#define GREEN_LED_GPIO            22
#define ADC_DETECT                 -1
#define ES7243_MCLK                -1
#define RESET_CODEC                -1
#define RESET_BOARD                -1
#define BLUE_LED_GPIO              -1
#define AUXIN_DETECT_GPIO          -1


#define AUDIO_CODEC_DEFAULT_CONFIG(){                   \
        .adc_input  = AUDIO_HAL_ADC_INPUT_LINE1,        \
        .dac_output = AUDIO_HAL_DAC_OUTPUT_ALL,         \
        .codec_mode = AUDIO_HAL_CODEC_MODE_BOTH,        \
        .i2s_iface = {                                  \
            .mode = AUDIO_HAL_MODE_SLAVE,               \
            .fmt = AUDIO_HAL_I2S_NORMAL,                \
            .samples = AUDIO_HAL_48K_SAMPLES,           \
            .bits = AUDIO_HAL_BIT_LENGTH_16BITS,        \
        },                                              \
};


#ifndef INPUT_KEY_NUM
#  define   INPUT_KEY_NUM     6
#endif

#define INPUT_KEY_DEFAULT_INFO() {                      \
     {                                                  \
        .type = PERIPH_ID_BUTTON,                       \
        .user_id = INPUT_KEY_USER_ID_REC,               \
        .act_id = BUTTON_REC_ID,                        \
    },                                                  \
    {                                                   \
        .type = PERIPH_ID_BUTTON,                       \
        .user_id = INPUT_KEY_USER_ID_MODE,              \
        .act_id = BUTTON_MODE_ID,                       \
    },                                                  \
    {                                                   \
        .type = PERIPH_ID_TOUCH,                        \
        .user_id = INPUT_KEY_USER_ID_SET,               \
        .act_id = BUTTON_SET_ID,                        \
    },                                                  \
    {                                                   \
        .type = PERIPH_ID_TOUCH,                        \
        .user_id = INPUT_KEY_USER_ID_PLAY,              \
        .act_id = BUTTON_PLAY_ID,                       \
    },                                                  \
    {                                                   \
        .type = PERIPH_ID_TOUCH,                        \
        .user_id = INPUT_KEY_USER_ID_VOLUP,             \
        .act_id = BUTTON_VOLUP_ID,                      \
    },                                                  \
    {                                                   \
        .type = PERIPH_ID_TOUCH,                        \
        .user_id = INPUT_KEY_USER_ID_VOLDOWN,           \
        .act_id = BUTTON_VOLDOWN_ID,                    \
    }                                                   \
}