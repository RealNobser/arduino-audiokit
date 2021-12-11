#pragma once
#include "AudioKitSettings.h"

// include drivers
#include "audio_hal/driver/es7148/es7148.h"
#include "audio_hal/driver/es7210/es7210.h"
#include "audio_hal/driver/es7243/es7243.h"
#include "audio_hal/driver/es8311/es8311.h"
#include "audio_hal/driver/es8374/es8374.h"
#include "audio_hal/driver/es8388/es8388.h"
#include "audio_hal/driver/tas5805m/tas5805m.h"
#include "audiokit_board.h"
#include "audiokit_logger.h"
#include "SPI.h"
#ifdef ESP32
#include "esp_a2dp_api.h"
#include "audio_system.h"
#include "driver/i2s.h"
#endif

#if ESP_IDF_VERSION_MAJOR < 4 && !defined(I2S_COMM_FORMAT_STAND_I2S)
#define I2S_COMM_FORMAT_STAND_I2S \
  (I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_LSB)
#define I2S_COMM_FORMAT_STAND_MSB \
  (I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB)
#define I2S_COMM_FORMAT_STAND_PCM_LONG \
  (I2S_COMM_FORMAT_PCM | I2S_COMM_FORMAT_PCM_LONG)
#define I2S_COMM_FORMAT_STAND_PCM_SHORT \
  (I2S_COMM_FORMAT_PCM | I2S_COMM_FORMAT_PCM_SHORT)
typedef int eps32_i2s_sample_rate_type;
#else
typedef uint32_t eps32_i2s_sample_rate_type;
#endif

/**
 * @brief Configuation for AudioKit
 *
 */
struct AudioKitConfig {
  i2s_port_t i2s_num = (i2s_port_t)0;
  gpio_num_t mclk_gpio = (gpio_num_t)0;

  audio_hal_adc_input_t adc_input =
      AUDIOKIT_DEFAULT_INPUT; /*!<  set adc channel with audio_hal_adc_input_t
                               */
  audio_hal_dac_output_t dac_output =
      AUDIOKIT_DEFAULT_OUTPUT;       /*!< set dac channel */
  audio_hal_codec_mode_t codec_mode; /*!< select codec mode: adc, dac or both */
  audio_hal_iface_mode_t master_slave_mode =
      AUDIOKIT_DEFAULT_MASTER_SLAVE; /*!< audio codec chip mode */
  audio_hal_iface_format_t fmt =
      AUDIOKIT_DEFAULT_I2S_FMT; /*!< I2S interface format */
  audio_hal_iface_samples_t sample_rate =
      AUDIOKIT_DEFAULT_RATE; /*!< I2S interface samples per second */
  audio_hal_iface_bits_t bits_per_sample =
      AUDIOKIT_DEFAULT_BITSIZE; /*!< i2s interface number of bits per sample */

  /// Returns true if the CODEC is the master
  bool isMaster() { return master_slave_mode == AUDIO_HAL_MODE_MASTER; }

  /// provides the bits per sample
  int bitsPerSample() {
    switch (bits_per_sample) {
      case AUDIO_HAL_BIT_LENGTH_16BITS:
        return 16;
      case AUDIO_HAL_BIT_LENGTH_24BITS:
        return 24;
      case AUDIO_HAL_BIT_LENGTH_32BITS:
        return 32;
    }
    // LOGE("bits_per_sample not supported: %d", bits_per_sample);
    return 0;
  }

  /// Provides the sample rate in samples per second
  int sampleRate() {
    switch (sample_rate) {
      case AUDIO_HAL_08K_SAMPLES: /*!< set to  8k samples per second */
        return 8000;
      case AUDIO_HAL_11K_SAMPLES: /*!< set to 11.025k samples per second */
        return 11000;
      case AUDIO_HAL_16K_SAMPLES: /*!< set to 16k samples in per second */
        return 16000;
      case AUDIO_HAL_22K_SAMPLES: /*!< set to 22.050k samples per second */
        return 22000;
      case AUDIO_HAL_24K_SAMPLES: /*!< set to 24k samples in per second */
        return 24000;
      case AUDIO_HAL_32K_SAMPLES: /*!< set to 32k samples in per second */
        return 32000;
      case AUDIO_HAL_44K_SAMPLES: /*!< set to 44.1k samples per second */
        return 44000;
      case AUDIO_HAL_48K_SAMPLES: /*!< set to 48k samples per second */
        return 48000;
    }
    // LOGE("sample rate not supported: %d", sample_rate);
    return 0;
  }

#ifdef ESP32
  /// Provides the ESP32 i2s_config_t to configure I2S
  i2s_config_t i2sConfig() {
    // use just the oposite of the CODEC setting
    int mode = isMaster() ? I2S_MODE_SLAVE : I2S_MODE_MASTER;
    if (codec_mode == AUDIO_HAL_CODEC_MODE_DECODE) {
      mode = mode | I2S_MODE_TX;
    } else if (codec_mode == AUDIO_HAL_CODEC_MODE_ENCODE) {
      mode = mode | I2S_MODE_RX;
    } else if (codec_mode == AUDIO_HAL_CODEC_MODE_BOTH) {
      mode = mode | I2S_MODE_RX | I2S_MODE_TX;
    }

    const i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)mode,
        .sample_rate = sampleRate(),
        .bits_per_sample = (i2s_bits_per_sample_t)bitsPerSample(),
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,  // default interrupt priority
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = true};
    return i2s_config;
  }

  /// Provides the ESP32 i2s_pin_config_t to configure the pins for I2S
  i2s_pin_config_t i2sPins() {
    i2s_pin_config_t result;
    get_i2s_pins(i2s_num, &result);
    return result;
  }
#endif
};

/**
 * @brief AudioKit API using the audio_hal
 *
 */

class AudioKit {

 public:
  AudioKit() {
    // setup SPI for SD drives
    setupSPI();
  }

  /// Provides a default configuration for input & output
  AudioKitConfig defaultConfig() {
    AudioKitConfig result;
    result.codec_mode = AUDIO_HAL_CODEC_MODE_BOTH;
    return result;
  }

  /// Provides the default configuration for input or output
  AudioKitConfig defaultConfig(bool isOutput) {
    AudioKitConfig result;
    if (isOutput) {
      result.codec_mode = AUDIO_HAL_CODEC_MODE_DECODE;  // dac
    } else {
      result.codec_mode = AUDIO_HAL_CODEC_MODE_ENCODE;  // adc
    }
    return result;
  }

  /// Starts the codec
  bool begin(AudioKitConfig cnfg) {
    LOGI(LOG_METHOD);
    LOGI("Selected board: %d", AUDIOKIT_BOARD);

    audio_hal_conf.adc_input = cfg.adc_input;
    audio_hal_conf.dac_output = cfg.dac_output;
    audio_hal_conf.codec_mode = cfg.codec_mode;
    audio_hal_conf.i2s_iface.mode = cfg.master_slave_mode;
    audio_hal_conf.i2s_iface.fmt = cfg.fmt;
    audio_hal_conf.i2s_iface.samples = cfg.sample_rate;
    audio_hal_conf.i2s_iface.bits = cfg.bits_per_sample;

    hal_handle = audio_hal_init(&audio_hal_conf, &AUDIO_DRIVER);
    if (hal_handle == 0) {
      LOGE("audio_hal_init");
      return false;
    }

    cfg = cnfg;

#ifdef ESP32

    // setup i2s driver - with no queue
    i2s_config_t i2s_config = cfg.i2sConfig();
    if (i2s_driver_install(cfg.i2s_num, &i2s_config, 0, NULL) != ESP_OK) {
      LOGE("i2s_driver_install");
      return false;
    }

    // define i2s pins
    i2s_pin_config_t pin_config = cfg.i2sPins();
    if (i2s_set_pin(cfg.i2s_num, &pin_config) != ESP_OK) {
      LOGE("i2s_set_pin");
      return false;
    }

    if (i2s_mclk_gpio_select(cfg.i2s_num, cfg.mclk_gpio) != ESP_OK) {
      LOGE("i2s_mclk_gpio_select");
      return false;
    }

#endif

    // call start
    if (!setActive(true)) {
      LOGE("setActive");
      return false;
    }

    return true;
  }

  /// Stops the CODEC
  bool end() {
    LOGI(LOG_METHOD);

#ifdef ESP32
    // uninstall i2s driver
    i2s_driver_uninstall(cfg.i2s_num);
#endif
    // stop codec driver
    audio_hal_ctrl_codec(hal_handle, cfg.codec_mode, AUDIO_HAL_CTRL_STOP);
    // deinit
    audio_hal_deinit(hal_handle);
    return true;
  }

  /// Provides the actual configuration
  AudioKitConfig config() { return cfg; }

  /// Sets the codec active / inactive
  bool setActive(bool active) {
    return audio_hal_ctrl_codec(
               hal_handle, cfg.codec_mode,
               active ? AUDIO_HAL_CTRL_START : AUDIO_HAL_CTRL_STOP) == ESP_OK;
  }

  /// Mutes the output
  bool setMute(bool mute) {
    return audio_hal_set_mute(hal_handle, mute) == ESP_OK;
  }

  /// Defines the Volume
  bool setVolume(int vol) {
    return (vol > 0) ? audio_hal_set_volume(hal_handle, vol) == ESP_OK : false;
  }

  /// Determines the volume
  int volume() {
    int volume;
    if (audio_hal_get_volume(hal_handle, &volume) != ESP_OK) {
      volume = -1;
    }
    return volume;
  }

#ifdef ESP32

  /// Writes the audio data via i2s to the DAC
  size_t write(const void *src, size_t size,
               TickType_t ticks_to_wait = portMAX_DELAY) {
    LOGD("write: %zu", size);
    size_t bytes_written = 0;
    if (i2s_write(cfg.i2s_num, src, size, &bytes_written, ticks_to_wait) !=
        ESP_OK) {
      LOGE("i2s_write");
    }
    return bytes_written;
  }

  /// Reads the audio data via i2s from the ADC
  size_t read(void *dest, size_t size,
              TickType_t ticks_to_wait = portMAX_DELAY) {
    LOGD("read: %zu", size);
    size_t bytes_read = 0;
    if (i2s_read(cfg.i2s_num, dest, size, &bytes_read, ticks_to_wait) !=
        ESP_OK) {
      LOGE("i2s_read");
    }
    return bytes_read;
  }

#endif

  /**
   * @brief  Get the gpio number for auxin detection
   *
   * @return  -1      non-existent
   *          Others  gpio number
   */
  int8_t pinAuxin() { return get_auxin_detect_gpio(); }

  /**
   * @brief  Get the gpio number for headphone detection
   *
   * @return  -1      non-existent
   *          Others  gpio number
   */
  int8_t pinHeadphoneDetect() { return get_headphone_detect_gpio(); }

  /**
   * @brief  Get the gpio number for PA enable
   *
   * @return  -1      non-existent
   *          Others  gpio number
   */
  int8_t pinPaEnable() { return get_pa_enable_gpio(); }

  /**
   * @brief  Get the gpio number for adc detection
   *
   * @return  -1      non-existent
   *          Others  gpio number
   */
  int8_t pinAdcDetect() { return get_adc_detect_gpio(); }

  /**
   * @brief  Get the mclk gpio number of es7243
   *
   * @return  -1      non-existent
   *          Others  gpio number
   */
  int8_t pinEs7243Mclk() { return get_es7243_mclk_gpio(); }

  /**
   * @brief  Get the record-button id for adc-button
   *
   * @return  -1      non-existent
   *          Others  button id
   */
  int8_t pinInputRec() { return get_input_rec_id(); }

  /**
   * @brief  Get the number for mode-button
   *
   * @return  -1      non-existent
   *          Others  number
   */
  int8_t pinInputMode() { return get_input_mode_id(); }

  /**
   * @brief Get number for set function
   *
   * @return -1       non-existent
   *         Others   number
   */
  int8_t pinInputSet() { return get_input_set_id(); };

  /**
   * @brief Get number for play function
   *
   * @return -1       non-existent
   *         Others   number
   */
  int8_t pinInputPlay() { return get_input_play_id(); }

  /**
   * @brief number for volume up function
   *
   * @return -1       non-existent
   *         Others   number
   */
  int8_t pinVolumeUp() { return get_input_volup_id(); }

  /**
   * @brief Get number for volume down function
   *
   * @return -1       non-existent
   *         Others   number
   */
  int8_t pinVolumeDown() { return get_input_voldown_id(); }

  /**
   * @brief Get green led gpio number
   *
   * @return -1       non-existent
   *        Others    gpio number
   */
  int8_t pinResetCodec() { return get_reset_codec_gpio(); }

  /**
   * @brief Get DSP reset gpio number
   *
   * @return -1       non-existent
   *         Others   gpio number
   */
  int8_t pinResetBoard() { return get_reset_board_gpio(); }

  /**
   * @brief Get DSP reset gpio number
   *
   * @return -1       non-existent
   *         Others   gpio number
   */
  int8_t pinGreenLed() { return get_green_led_gpio(); }

  /**
   * @brief Get green led gpio number
   *
   * @return -1       non-existent
   *         Others   gpio number
   */
  int8_t pinBlueLed() { return get_blue_led_gpio(); }

  /**
   * @brief SPI CS Pin for SD Drive
   * 
   * @return int8_t 
   */
  int8_t pinSpiCs() {
    return spi_cs_pin;
  }

 protected:
  AudioKitConfig cfg;
  audio_hal_codec_config_t audio_hal_conf;
  audio_hal_handle_t hal_handle;
  audio_hal_codec_i2s_iface_t iface;
  int8_t spi_cs_pin;
 

  /**
   * @brief Setup the SPI so that we can access the SD Drive
   */
  void setupSPI() {
    LOGD(LOG_METHOD);

#ifdef ESP32
    spi_bus_config_t cfg;
    spi_device_interface_config_t if_cfg;

    // determine the pins from the hal
    get_spi_pins(&cfg, &if_cfg);
    spi_cs_pin = if_cfg.spics_io_num;

    if (cfg.sclk_io_num!=-1){

      // LOGI("SPI sclk: %d", cfg.sclk_io_num);
      // LOGI("SPI miso: %d", cfg.miso_io_num);
      // LOGI("SPI mosi: %d", cfg.mosi_io_num);
      // LOGI("SPI cs: %d", spi_cs_pin);

      // Open the SPI bus with the pins for this board
      SPI.begin(cfg.sclk_io_num, cfg.miso_io_num, cfg.mosi_io_num, spi_cs_pin);
    } else {
      SPI.begin(14, 2, 15, 13);
    }
#else
    #warning "SPI initialization for the SD drive not supported - you might need to take care of this yourself" 
#endif
  }



};