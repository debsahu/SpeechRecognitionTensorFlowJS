# Speech Recognition using TensorFlowJS

[![Build Status](https://travis-ci.com/debsahu/SpeechRecognitionTensorFlowJS.svg?branch=master)](https://travis-ci.com/debsahu/SpeechRecognitionTensorFlowJS) [![License: MIT](https://img.shields.io/github/license/debsahu/SpeechRecognitionTensorFlowJS.svg)](https://www.apache.org/licenses/LICENSE-2.0) [![version](https://img.shields.io/github/release/debsahu/SpeechRecognitionTensorFlowJS.svg)](https://github.com/debsahu/SpeechRecognitionTensorFlowJS/releases/tag/1.0.0) [![LastCommit](https://img.shields.io/github/last-commit/debsahu/SpeechRecognitionTensorFlowJS.svg?style=social)](https://github.com/debsahu/SpeechRecognitionTensorFlowJS/commits/master)

## Speech Recognition on Browser, [AsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) served on ESP8266 to control LED_BUILTIN/GPIO16

- Uses [WebAudio API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API) and WebGL GPU acceleration = speech recognition is done on the browser
- `http://` requests for microphone is blocked for chrome, **use firefox instead**
- **tf.min.js** and **speech-commands.min.js** served from SPIFFs (1MB Program/3MB SPIFFs partition needed)
- `/upload` and `/update` is a morden world's take on updates to ESP8266
- Uses HTML templates to report LED_BUILTIN/GPIO16 status
- Speech recognition: "UP" = ON and "DOWN" = OFF, "RIGHT" and "LEFT" ignored

### YouTube

[![SpeechRecognitionTensorFlowJS](https://img.youtube.com/vi/E5KpzR9Igfw/0.jpg)](https://www.youtube.com/watch?v=E5KpzR9Igfw)

### Arduino Libraries needed

[platformio.ini](https://github.com/debsahu/SpeechRecognitionTensorFlowJS/blob/master/platformio.ini) is included, use [PlatformIO](https://platformio.org/platformio-ide) and it will take care of installing the following libraries.

| Library                   | Link                                                       |
|---------------------------|------------------------------------------------------------|
|ESPAsyncTCP                |https://github.com/me-no-dev/ESPAsyncTCP                    |
|ESPAsyncWiFiManager        |https://github.com/alanswx/ESPAsyncWiFiManager              |
|ESP Async WebServer        |https://github.com/me-no-dev/ESPAsyncWebServer              |

## Credits

- Inspiration(and major code with regards to JS): [Rubik's Code](https://rubikscode.net/2019/04/01/drawing-with-voice-speech-recognition-with-tensorflow-js/)
- TensorFlowJS Documentation: [Speech recognition](https://github.com/tensorflow/tfjs-models/tree/master/speech-commands)
- [@RuiSantosdotme](https://github.com/RuiSantosdotme) for [HTML Templates](https://github.com/RuiSantosdotme/ESP32-Course/tree/master/code/SPIFFS/ESP32_Async_Web_Server)
