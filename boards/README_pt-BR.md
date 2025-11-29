```
.
├── platformio.ini
├── boards
    ├── _boards_json
    │   └── [board].json
    ├── [board]
    │   └── interface.cpp
    │   └── [board].ini
    │   └── pins_arduino.h
    ├── pinouts
        ├── pins_arduino.h
        └── variant.cpp


...
```

# Arquivos
(Substitua `[board]` pelo nome da placa)

## boards/_boards_json/[board].json

Este é o arquivo de configuração da placa.  
Use outros arquivos deste diretório como referência do que é necessário.

Exemplo oficial (e o formato que usamos aqui):

https://github.com/platformio/platform-espressif32/blob/master/boards/esp32-s3-devkitc-1.json

Observação: a maioria das placas usa uma variante compartilhada chamada `pinouts`:

```json
    ...
      "mcu": "esp32s3",
      "variant": "pinouts"
    ...
```

Algumas placas, como `ESP32-3248S035R`, usam um diretório de variante dedicado, que deve
corresponder ao campo `"variant"` (por exemplo, `"variant": "ESP32-3248S035R"`).

Para o caso específico da Sunton ESP32-3248S035R (Cheap Yellow Display 3,5" resistiva), também existem arquivos adicionais dentro de `boards/ESP32-3248S035R`:

- `User_Setup.h` – configuração do TFT_eSPI (display ST7796 + touch resistivo).
- `ESP32-3248S035R.ini` – ambientes PlatformIO dedicados à placa.
- `README.md` – documentação detalhada da placa (pinagem, calibração de touch, etc.).

## boards/pinouts/pins_arduio.h

Aqui você define a flag que incluirá o header de pinagem específico da sua placa, apontando para `boards/[board]/pins_arduino.h`.

## boards/[board]/pins_arduino.h

Aqui ficam as flags e pinouts da placa.  
Use as outras placas como referência para saber o que declarar.

Exemplo oficial (e o formato que usamos aqui):

https://github.com/espressif/arduino-esp32/blob/master/variants/esp32s3/pins_arduino.h

## boards/[board]/interface.cpp

Arquivo onde você implementa o código de setup específico da placa (por exemplo: inicialização de GPIOs, touch, backlight, deep-sleep, etc.).

## boards/[board]/[board].ini

Arquivo de configuração do PlatformIO para o dispositivo.  
Use os `.ini` das outras placas como referência.

Além destes arquivos básicos, algumas placas ainda podem ter arquivos extras dentro do diretório `boards/[board]` (por exemplo, configurações específicas de bibliotecas de display, arquivos de calibração ou documentação adicional).