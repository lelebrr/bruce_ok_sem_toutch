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

- Compartilham o barramento VSPI (18/19/23).
- Use CS dedicados por módulo:
  - Ex.: `CS_CC1101 = 25`, `CS_NRF24 = 26`, `CS_W5500 = 32`, etc.
- Ajuste os `build_flags` ou a lógica de seleção de módulo no código para usar os novos pinos.
- Mais detalhes e passo a passo de ligação com exemplos de uso estão na seção abaixo.

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

## Exemplo prático: ligando CC1101, NRF24 e W5500 na Sunton

A seguir, um guia passo a passo para ligar e usar os principais módulos RF/Ethernet na Sunton:

- CC1101 (Sub-GHz)
- NRF24L01 (2.4 GHz)
- W5500 (Ethernet)

Todos compartilham o barramento VSPI (SCK=18, MISO=19, MOSI=23) da Sunton.

### 1) Pinos base (VSPI da Sunton)

- `SCK  = GPIO 18`
- `MISO = GPIO 19`
- `MOSI = GPIO 23`

Esses pinos já são usados também pelo microSD. O truque é:

- Compartilhar SCK/MISO/MOSI entre módulos.
- Dar um CS dedicado para cada um.

Recomendação de CS (podem ser ajustados):

- `SDCARD_CS = 5` (já definido no `.ini`)
- `CS_CC1101 = 25`
- `CS_NRF24  = 26`
- `CS_W5500  = 32`

Sempre verifique se esses pinos estão realmente livres na sua versão da placa.

### 2) Ligação física — CC1101

Módulos CC1101 comuns (com header de 8 pinos) normalmente usam:

- `MOSI` → `SI`
- `MISO` → `SO`
- `SCK`  → `SCK`
- `CS`   → `CSN` ou `CS`
- `GDO0`/`GDO2` → pinos de interrupção opcionais, usados para Rx/Tx pronto (podem ser ligados em pinos GPIO livres, se necessário).

Sugestão na Sunton:

- `SI` (MOSI CC1101) → GPIO 23
- `SO` (MISO CC1101) → GPIO 19
- `SCK` → GPIO 18
- `CSN` → GPIO 25 (`CS_CC1101`)
- `GDO0` → GPIO 34 (exemplo, se quiser usar IRQ)
- `GND` → GND
- `VCC` → 3V3

No Bruce, o CC1101 é inicializado em `setup_gpio()` via `initCC1101once(...)`, usando a configuração de `bruceConfigPins.CC1101_bus`. Para a Sunton:

- Certifique-se de que o mapeamento de pinos em `bruceConfigPins` foi configurado (via UI ou arquivo de config) para usar:
  - `mosi = 23`
  - `miso = 19`
  - `sck  = 18`
  - `ss   = 25` (ou o CS que você escolheu)

Exemplo de uso no Bruce (já existente nos menus):

- Menu RF → Config:
  - Selecionar módulo CC1101.
  - Ajustar frequência, TX/RX, etc.
- Menu RF → Custom SubGhz / Replay:
  - Carregar arquivos `.sub` / `.txt` compatíveis.

Não é necessário escrever código extra: basta ligar o hardware nos pinos acima e configurar via menu.

### 3) Ligação física — NRF24L01

Módulos NRF24L01 com antena (PA+LNA) normalmente usam:

- `MOSI` → `MOSI`
- `MISO` → `MISO`
- `SCK`  → `SCK`
- `CSN`  → CS do SPI
- `CE`   → pino de controle (GPIO dedicado)
- `IRQ`  → opcional (interrupções)

Sugestão na Sunton:

- `MOSI` → GPIO 23
- `MISO` → GPIO 19
- `SCK`  → GPIO 18
- `CSN`  → GPIO 26 (`CS_NRF24`)
- `CE`   → GPIO 33 ou outro GPIO livre (se não estiver usando para outra função)
- `IRQ`  → opcional (ex.: GPIO 34)
- `VCC`  → 3V3 (NRF24L01 costuma ser sensível a ruído, use fonte estável)
- `GND`  → GND

No Bruce, há suporte integrado para NRF24:

- Menu NRF24:
  - Jammer
  - 2.4G Spectrum
  - (Futuro) Mousejack/Keyboardjacking.

Para garantir que o Bruce use esses pinos:

- Verifique em `bruceConfigPins` (via UI) o mapeamento do módulo NRF24:
  - `mosi = 23`
  - `miso = 19`
  - `sck  = 18`
  - `ss   = 26` (CSN)
  - `ce   = 33` (por exemplo)

Novamente, não há necessidade de código extra, apenas hardware e configuração.

### 4) Ligação física — W5500 (Ethernet)

Módulos W5500 usam basicamente SPI + alguns sinais adicionais:

- `MOSI`
- `MISO`
- `SCK`
- `CS`
- `RST` (opcional)
- `INT` (opcional)

Sugestão na Sunton:

- `MOSI` → GPIO 23
- `MISO` → GPIO 19
- `SCK`  → GPIO 18
- `CS`   → GPIO 32 (`CS_W5500`)
- `RST`  → pino GPIO livre ou ligado a 3V3 via resistor (conforme o módulo)
- `INT`  → opcional (GPIO livre)
- `VCC`  → 3V3
- `GND`  → GND

No momento, o Bruce não expõe um menu específico de W5500/Ethernet em todos os devices, então este mapeamento é mais para quem quer experimentar/estender:

- Você pode criar scripts ou código customizado para usar o W5500, com base nos exemplos de bibliotecas Ethernet (WIZnet, etc.).
- A vantagem é que a Sunton já expõe facilmente o barramento SPI e você pode compartilhar com SD/CC1101/NRF24.

### 5) Exemplo concreto de configuração via código (para projetos customizados)

Se você estiver criando um firmware derivado do Bruce e quiser fixar os pinos por código (em vez de só usar o menu), um padrão simples para CC1101/NRF24 seria:

```cpp
// Exemplo simplificado de configuração de pinos para uso com CC1101/NRF24 na Sunton
// (para projetos baseados em Bruce, mas com código custom)

#define CC1101_CS_PIN   25
#define NRF24_CS_PIN    26
#define NRF24_CE_PIN    33

SPIClass vspi(VSPI);

void setup_rf_buses() {
    // VSPI base pins - já definidos no .ini, mas explicitando:
    vspi.begin(18 /*SCK*/, 19 /*MISO*/, 23 /*MOSI*/, CC1101_CS_PIN);

    pinMode(CC1101_CS_PIN, OUTPUT);
    digitalWrite(CC1101_CS_PIN, HIGH); // deselecionado

    pinMode(NRF24_CS_PIN, OUTPUT);
    digitalWrite(NRF24_CS_PIN, HIGH);  // deselecionado

    pinMode(NRF24_CE_PIN, OUTPUT);
    digitalWrite(NRF24_CE_PIN, LOW);

    // A partir daqui você inicializa as libs específicas (SmartRC-CC1101, RF24, etc.)
    // usando &vspi e esses pinos de CS/CE.
}
```

No Bruce “oficial”, essa lógica é abstraída pelo `bruceConfigPins` e pelas funções de inicialização já existentes (como `initCC1101once`), então em geral não é necessário mexer no código, apenas cabear corretamente e configurar via UI.

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
6. (Opcional) Conecte CC1101, NRF24 ou W5500 ao barramento VSPI conforme as sugestões acima e configure os pinos via menus de Config/Hardware.
7. Use o WebUI e o gerenciamento de arquivos para explorar as demais features.

Se surgir qualquer erro de compilação, linkagem ou comportamento estranho, compartilhe os logs: com eles é possível ajustar finamente macros, pinos, frequências de SPI e outras configurações para tirar o máximo proveito desta placa.