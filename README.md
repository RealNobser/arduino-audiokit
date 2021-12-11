# Arduino AudioKit HAL

There are different ESP32 AudioKit boards available that can be programmed with the Espressif ADF Framework.

<img src="https://pschatzmann.github.io/arduino-audio-tools/resources/audio-toolkit.png" alt="Audio Kit" />

The ADF Framework contains an abstraction layer to support different CODEC chips. Unfortunately ADF can not be used in Arduino, but it would be quite useful to have this functionality also available.

Because I wanted to have a proper support of the AudioKit for my [Arduino Audio Tools](https://github.com/pschatzmann/arduino-audio-tools) library and since my Audio Tools are  header only, I decided to provide this functionality in a separate project.

I converted the __audio_boards__ and their related __drivers__ into an __Arduino Library__ and provide an easy to use C++ class which configures both the CODEC and I2S. I also tried to abstract away all ESP32 specific funcationality. 

So with this project it should now get quite easy to use these boards also in Arduino.

## Configuration

You can define your board and the default settings in the ```AudioKitSettings.h``` file. 

```
 * @brief AUDIOKIT_BOARD selects a specic board:
 * 1) lyrat_v4_3
 * 2) lyrat_v4_2
 * 3) lyrat_mini_v1_1
 * 4) esp32_s2_kaluga_1_v1_2
 * 5) ai_thinker_v2_2
 */

#define AUDIOKIT_BOARD 1

```
Here the lyrat v3.4 has been selected. 


## Example Sketch

Here is an example sketch that writes audio data to the audio kit board via I2S. By default I2S is set up as master and the codec as slave, the sample size is 16 bits and the sample rate is 44.1k/sec - if you did not change the default settings. 

You can adjust these parameters by setting your requested values in the cfg below:

```
#include "AudioKit.h"
#include "SineWaveGenerator.h"

AudioKit kit;
SineWaveGenerator wave;
const int BUFFER_SIZE = 1024;
uint8_t buffer[BUFFER_SIZE];

void setup() {
  // open in write mode
  auto cfg = kit.defaultConfig(true);
  cfg.sample_rate = AUDIO_HAL_22K_SAMPLES;
  kit.begin(cfg);

  // 1000 hz
  wave.setFrequency(1000);
  wave.setSampleRate(cfg.sampleRate());
}

void loop() {
  size_t l = wave.read(buffer, BUFFER_SIZE);
  kit.write(buffer, l);
}

```
## Logging

The functionality has a built in logger. The default log level has been set to Warning. You can change it like this:

```
  AUDIOKIT_LOG_LEVEL = Debug; // or Info, Warning, Error

```


## Documentation

Here is the documentaion of the [AudioKit class](https://pschatzmann.github.io/arduino-audiokit-hal/html/class_audio_kit.html)


## Installation in Arduino

You can download the library as zip and call include Library -> zip library. Or you can git clone this project into the Arduino libraries folder e.g. with

```
cd  ~/Documents/Arduino/libraries
git clone https://github.com/pschatzmann/arduino-audiokit-hal.git

```

If you want to use the library in PlatformIO, you can find a [detailed description in the Wiki](https://github.com/pschatzmann/arduino-audiokit-hal/wiki/PlatformIO).


## Supprted Devices / Processors

The examples have been tested with a AI Thinker v2.2.  I do not own any other AudioKit devices, so I can't guarantee that they work properly. I also made sure that the code is compiling on other processors, but I did not perform any tests. Please note that because I2S is not standardized in Arduino, you need to take care of the I2S initialization  and processing yourself on other processors.

