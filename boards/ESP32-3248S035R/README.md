# Sunton ESP32-3248S035R (Cheap Yellow Display 3,5" resistiva)

Este documento descreve a integração da placa Sunton ESP32-3248S035R (também conhecida como Cheap Yellow Display 3,5" resistiva) com o firmware Bruce.

A ideia é ter um suporte completo, estável e alinhado às práticas do projeto, sem gambiarras, usando:

- Display TFT 3,5" 320x480 com driver ST7796 via HSPI
- Touch resistivo com controlador compatível XPT2046 (no mesmo barramento do display)
- Segundo barramento SPI (VSPI) para microSD, CC1101, NRF24 etc.
- I2C externo para sensores e periféricos
- Controle de brilho via PWM
- Sem PSRAM, sem bateria e sem áudio integrado

---

## Visão geral do hardware

Principais características da placa:

- ESP32 (sem PSRAM), 4 MB de flash
- Tela TFT de 3,5" com resolução 320x480 (orientação landscape)
- Touch resistivo (controlador XPT2046 ou equivalente)
- Barramento HSPI dedicado ao display + touch
- Barramento VSPI para microSD e módulos RF
- Conector I2C (Grove / header externo)
- Alimentação via USB (sem bateria integrada)

Referência de pinout e documentação da placa:

- https://www.espboards.dev/esp32/cyd-esp32-3248s035/

---

## Arquivos desta placa

Todos os arquivos específicos da Sunton ESP32-3248S035R ficam em:

- `boards/ESP32-3248S035R/`

Conteúdo:

- `ESP32-3248S035R.ini`  
  Ambiente(s) PlatformIO específicos da placa. Define macros de display, touch, SPI, I2C etc.
- `pins_arduino.h`  
  Pinagem padrão usada pelo core Arduino (SPI principal, VSPI, I2C).
- `User_Setup.h`  
  Configuração da biblioteca TFT_eSPI para o driver ST7796 e touch resistivo.
- `interface.cpp`  
  Implementação da interface da placa:
  - Setup de GPIOs
  - Calibração e leitura do touch
  - Controle de brilho
  - Funções de deep-sleep/power off
- `interface_simple.cpp.bak`  
  Versão antiga/simplificada mantida apenas como backup/referência.

---

## Ambientes PlatformIO

### Configuração global

No arquivo `platformio.ini` da raiz, a placa Sunton já é o ambiente padrão:

```ini
[platformio]
default_envs =
    ESP32-3248S035R-resistive
```

Isso significa que, se você simplesmente rodar `pio run` na raiz, ele vai compilar para essa placa.

### Ambientes específicos

Em `boards/ESP32-3248S035R/ESP32-3248S035R.ini` existem dois ambientes:

- `env:ESP32-3248S035R`
  - `board = ESP32-3248S035R`  
    (JSON correspondente em `boards/_boards_json/ESP32-3248S035R.json`)
  - Aponta o diretório de variante para `boards/ESP32-3248S035R`.
  - Define macros para display, touch, SPI, I2C, brilho etc.
- `env:ESP32-3248S035R-resistive`
  - `extends = env:ESP32-3248S035R`
  - Adiciona:
    - `-DBRUCE_VERSION="dev"`
    - `-DGIT_COMMIT_HASH="custom"`
    - `-DUSE_RESISTIVE_TOUCH=1`
    - `-DTOUCH_TYPE=1`

### Comandos recomendados

Na raiz do projeto:

```bash
# Compilar apenas
pio run -e ESP32-3248S035R-resistive

# Compilar e gravar
pio run -e ESP32-3248S035R-resistive -t upload

# Abrir monitor serial
pio device monitor -b 115200
```

---

## Pinout resumido

### Display TFT (HSPI)

Pinos usados pelo driver ST7796:

- `TFT_MISO` – GPIO 12
- `TFT_MOSI` – GPIO 13
- `TFT_SCLK` – GPIO 14
- `TFT_CS`   – GPIO 15
- `TFT_DC`   – GPIO 2
- `TFT_RST`  – GPIO 4
- `TFT_BL`   – GPIO 27 (backlight – controlado por PWM)

Esses pinos estão definidos em:

- `boards/ESP32-3248S035R/pins_arduino.h`
- `boards/ESP32-3248S035R/User_Setup.h`
- `boards/ESP32-3248S035R/ESP32-3248S035R.ini` (via `build_flags`)

### Touch resistivo (XPT2046 compatível)

O touch usa o mesmo barramento SPI do display (HSPI), apenas com CS e IRQ dedicados:

- `TOUCH_CS`  – GPIO 33
- `TOUCH_IRQ` – GPIO 36

Definidos em:

- `User_Setup.h`
- `ESP32-3248S035R.ini` (via `-DTOUCH_CS=33`)

O driver usado é o helper de touch da própria TFT_eSPI (não utilizamos a biblioteca `XPT2046_Touchscreen`).

### Barramento SPI 2 (VSPI – microSD, CC1101, NRF24 etc.)

No arquivo `pins_arduino.h`:

```cpp
static const uint8_t SS   = 5;
static const uint8_t MOSI = 23;
static const uint8_t MISO = 19;
static const uint8_t SCK  = 18;
```

No `ESP32-3248S035R.ini`:

```ini
-DSDCARD_CS=5
-DSDCARD_SCK=18
-DSDCARD_MISO=19
-DSDCARD_MOSI=23

; SPI bus for CC1101 / NRF24 / W5500 will be configured from these pins
-DSPI_SCK_PIN=18
-DSPI_MOSI_PIN=23
-DSPI_MISO_PIN=19
-DSPI_SS_PIN=27
```

Observações:

- MicroSD usa CS em `GPIO 5` (`SDCARD_CS`).
- `SPI_SS_PIN=27` é exposto como CS genérico para módulos como CC1101/NRF24/W5500, caso o firmware utilize essa macro.
- O pino 27 também é usado como `TFT_BL` (backlight), portanto **para projetos customizados é recomendado** usar um CS dedicado (ex.: 25, 26, 32) e ajustar as macros conforme necessário.

### I2C padrão (header externo / Grove)

- `SDA` / `GROVE_SDA` – GPIO 21
- `SCL` / `GROVE_SCL` – GPIO 22

Esses pinos são usados para sensores e periféricos I2C externos.

### Botão BOOT / Wakeup

- `GPIO0` – botão BOOT da placa
  - Usado como fonte de wake-up do deep-sleep (ver funções `powerOff()` e `goToDeepSleep()`).

---

## Configuração do display e touch (TFT_eSPI)

A configuração do display e do touch está em:

- `boards/ESP32-3248S035R/User_Setup.h`

Principais definições:

```cpp
#define ST7796_DRIVER
#define USE_HSPI_PORT

#define TFT_WIDTH  320
#define TFT_HEIGHT 480

#define SPI_FREQUENCY       27000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY 2500000
```

Pinos:

```cpp
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  4

#define TFT_BL 27

#define TOUCH_CS  33
#define TOUCH_IRQ 36
#define TOUCH_DRIVER 2046
```

Fonts habilitadas:

```cpp
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT
```

O arquivo `.ini` da placa replica essas definições via `build_flags` para garantir consistência entre TFT_eSPI e o restante do código.

---

## Interface da placa (`interface.cpp`)

O arquivo `boards/ESP32-3248S035R/interface.cpp` cuida de toda a integração da placa com o core do Bruce.

### `_setup_gpio()`

- Configura CS do TFT (`TFT_CS`) como saída e coloca em HIGH.
- Configura CS do touch (`TOUCH_CS`) como saída e coloca em HIGH.
- Configura CS do microSD (`SDCARD_CS`, se definido) como saída e coloca em HIGH.
- Garante que os dispositivos SPI iniciem em estado seguro (não selecionados).

### `_post_setup_gpio()`

Responsável pelo segundo estágio da configuração:

1. Garante que `TOUCH_CS` esteja como OUTPUT.
2. Tenta abrir o arquivo `/calData` no LittleFS:
   - Se **não existir**:
     - Ajusta a rotação do display: `tft.setRotation(ROTATION)`.
     - Executa `tft.calibrateTouch(calData, TFT_WHITE, TFT_BLACK, 10)` para calibrar o touch.
     - Salva os 5 valores de calibração no arquivo `/calData`.
   - Se **existir**:
     - Lê os 5 valores de calibração linha a linha.
     - Imprime os valores no Serial.
3. Chama `tft.setTouch(calData)` para aplicar a calibração.
4. Inicializa o controle de brilho:
   - Configura o canal PWM via `ledcSetup(TFT_BRIGHT_CHANNEL, TFT_BRIGHT_FREQ, TFT_BRIGHT_Bits)`.
   - Associa `TFT_BL` ao canal PWM com `ledcAttachPin`.
   - Define brilho inicial máximo: `ledcWrite(TFT_BRIGHT_CHANNEL, 255)`.

### `getBattery()` e `isCharging()`

A placa é típica de bancada, alimentada por USB e sem medidor de bateria integrado:

- `int getBattery()` retorna `0`.
- `bool isCharging()` retorna sempre `false`.

Essas funções são stubs para manter compatibilidade com a API do Bruce, sem expor informações de bateria inexistentes.

### `_setBrightness(uint8_t brightval)`

Converte um valor de brilho (0–100%) em duty-cycle PWM:

- Casos especiais:
  - 100 → duty 255
  - 75 → duty ~130
  - 50 → duty ~70
  - 25 → duty ~20
  - 0 → duty 0
- Para outros valores intermediários, calcula proporcionalmente: `(brightval * 255) / 100`.

Aplica o resultado com `ledcWrite(TFT_BRIGHT_CHANNEL, dutyCycle)`.

### `InputHandler(void)`

Se `USE_TFT_eSPI_TOUCH` estiver definido:

1. Verifica um intervalo mínimo entre leituras (`millis() - d_tmp > 200`) ou `LongPress`.
2. Chama `checkPowerSaveTime()` para manter a lógica de economia de energia.
3. Lê o touch com `tft.getTouch(&t.x, &t.y)`.
4. Se houver toque:
   - Zera flags de navegação (NextPress, PrevPress, UpPress etc.).
   - Ajusta as coordenadas com base em `bruceConfig.rotation`:
     - Rotação 3: espelha eixo X/Y com correção de altura.
     - Rotação 0: faz swap entre x/y, com correção de largura.
     - Rotação 2: faz outro mapeamento rotacionado.
   - Se `wakeUpScreen()` não acordar a tela, marca `AnyKeyPress = true`.
   - Atualiza `touchPoint.x`, `touchPoint.y` e `touchPoint.pressed`.
   - Chama `touchHeatMap(touchPoint)` para alimentar o mapa de calor de toques.
   - Atualiza `d_tmp` com o `millis()` atual.

Isso garante que o touch resistivo esteja totalmente integrado com:

- A navegação da UI
- O teclado on-screen
- A lógica de power-save usada pelo Bruce

### `powerOff()` e `goToDeepSleep()`

Implementam o deep-sleep via ESP-IDF:

- `powerOff()`:
  - `esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, LOW);`
  - `esp_deep_sleep_start();`
- `goToDeepSleep()`:
  - Apenas chama `powerOff()` (alias).

O wake-up é realizado pelo botão BOOT (GPIO0).

### `checkReboot()`

Atualmente é um placeholder vazio, reservado para lógica de reboot via long-press ou eventos especiais, se necessário no futuro.

---

## Controle de brilho (backlight)

Macros relevantes definidos em `ESP32-3248S035R.ini`:

```ini
-DTFT_BRIGHT_CHANNEL=0
-DTFT_BRIGHT_Bits=8
-DTFT_BRIGHT_FREQ=5000
```

Fluxo:

1. `_post_setup_gpio()` configura o PWM usando essas constantes.
2. `_setBrightness()` recebe um valor de 0 a 100 e converte para duty-cycle.
3. O backlight (`TFT_BL`, pino 27) é controlado via `ledcWrite`.

Na UI do Bruce, o ajuste de brilho conversa diretamente com essa função, mantendo a experiência consistente com outras placas.

---

## Áudio, LEDs e outras particularidades

Para a Sunton ESP32-3248S035R, foram feitas algumas escolhas para manter o suporte estável:

- Áudio desabilitado:
  - Não há saída de áudio dedicada na placa.
  - Comentários em `src/modules/others/audio.cpp` indicam a desativação para esta placa.
- Controle de LED:
  - `src/core/led_control.cpp` contém apenas um placeholder para esta placa.
  - Não há LED RGB dedicado definido por padrão.
- Sem PSRAM:
  - Menos memória disponível do que em placas com PSRAM.
  - O código foi ajustado para funcionar bem dentro dessas limitações.

Se você quiser:

- Adicionar áudio externo (amplificador/I2S)
- Mapear LEDs adicionais
- Usar periféricos extras

o recomendado é:

- Ver como isso é feito em outras placas (ex.: M5CoreS3, Cardputer, etc.).
- Criar um novo ambiente `.ini` derivado do `ESP32-3248S035R-resistive` com as suas mudanças.
- Manter o ambiente oficial estável para evitar regressões.

---

## Dicas de integração de periféricos

### MicroSD

- Usa o barramento VSPI:
  - `SCK = 18`
  - `MOSI = 23`
  - `MISO = 19`
  - `CS = 5` (`SDCARD_CS`)
- Verifique se o cartão está formatado de forma compatível e com boa qualidade para evitar corrupção.

### CC1101 / NRF24 / W5500

- Compartilham o barramento VSPI.
- Use CS dedicados por módulo:
  - Ex.: `CS_CC1101 = 25`, `CS_NRF24 = 26`, etc.
- Ajuste os `build_flags` ou a lógica de seleção de módulo no código para usar os novos pinos.

### I2C

- `GROVE_SDA = 21`
- `GROVE_SCL = 22`
- Ideal para sensores e periféricos como:
  - Acelerômetros
  - Sensores ambientais
  - Displays adicionais I2C, etc.

---

## Calibração do touch e resolução de problemas

### Como funciona a calibração

- No primeiro boot:
  - Se `/calData` não existir, o firmware abre uma tela de calibração do touch.
  - Você toca nos pontos indicados, e a calibração é salva.
- Em boots seguintes:
  - O Bruce lê os valores de `/calData` e aplica via `tft.setTouch(calData)`.

### Recalibrando o touch

Se o touch estiver impreciso ou “virado”:

1. Acesse o sistema de arquivos do LittleFS:
   - Via WebUI do Bruce, ou
   - Via gerenciador de arquivos integrado.
2. Apague o arquivo `/calData`.
3. Reinicie a placa.
4. Siga novamente o assistente de calibração.

### Problemas comuns

- Tela acende, mas não mostra nada:
  - Verifique se o ambiente correto está selecionado: `ESP32-3248S035R-resistive`.
  - Confirme se o flash foi gravado no offset correto (0x00000).
- Touch não responde:
  - Confirme os pinos de touch (33 e 36) na sua placa/variante.
  - Refaça a calibração apagando `/calData`.
- Artefatos ou instabilidade no display:
  - Pode ser sensibilidade ao SPI_FREQUENCY do ST7796.
  - Teste frequências um pouco menores no seu fork, se necessário.

---

## Resumo rápido para começar

1. Instale o PlatformIO (VSCode ou CLI).
2. Conecte a Sunton ESP32-3248S035R via USB.
3. Na raiz do repositório, execute:

   ```bash
   pio run -e ESP32-3248S035R-resistive -t upload
   ```

4. Aguarde o boot e siga a calibração do touch na tela.
5. Após a calibração, o touch estará integrado com a UI do Bruce e com o teclado on-screen.
6. Use o WebUI e o gerenciamento de arquivos para explorar as demais features.

Se surgir qualquer erro de compilação, linkagem ou comportamento estranho, compartilhe os logs: com eles é possível ajustar finamente macros, pinos, frequências de SPI e outras configurações para tirar o máximo proveito desta placa.