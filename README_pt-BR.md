![Bruce Main Menu](./media/pictures/bruce_banner.jpg)

# :shark: Bruce

Bruce é um firmware versátil para ESP32 que reúne uma grande quantidade de funcionalidades ofensivas, com foco em facilitar operações de Red Team.
Ele também oferece suporte a produtos da M5Stack e funciona muito bem em Cardputer, Sticks, M5Cores, T-Decks e T-Embeds.

## :building_construction: Como instalar

### Forma mais fácil: Web Flasher oficial

A maneira mais simples de instalar o Bruce é usando o Web Flasher oficial:

https://bruce.computer/flasher

Ele detecta sua placa compatível, baixa a imagem correta e grava tudo automaticamente pelo navegador (Chrome/Edge).

### Instalação manual via esptool.py

Você também pode baixar o binário mais recente em “Releases” ou “Actions” do GitHub e gravar localmente com o `esptool.py`:

```sh
esptool.py --port /dev/ttyACM0 write_flash 0x00000 Bruce-<device>.bin
```

Substitua `<device>` pelo nome correto do seu dispositivo/placa.

### Dispositivos M5Stack

Se você já usa o M5Launcher para gerenciar seu dispositivo M5Stack, é possível instalar o Bruce via OTA diretamente por ele.

Outra opção é gravar o binário diretamente com a ferramenta [M5Burner](https://docs.m5stack.com/en/download):

1. Abra o M5Burner.
2. Selecione a categoria correspondente ao seu dispositivo.
3. Procure por “Bruce” (os builds oficiais aparecem com “owner” e têm fotos).
4. Clique em “Burn” para gravar.

### Ambiente padrão: Sunton ESP32-3248S035R (Cheap Yellow Display 3,5" resistiva)

Este repositório já está configurado para usar, por padrão, o ambiente da placa Sunton ESP32-3248S035R (Cheap Yellow Display 3,5" resistiva). No `platformio.ini`:

```ini
[platformio]
default_envs =
    ESP32-3248S035R-resistive
```

Ambientes principais:

- `env:ESP32-3248S035R`
  - Define a placa `board = ESP32-3248S035R` usando o JSON em `boards/_boards_json/ESP32-3248S035R.json`.
  - Configura pinos, driver de display (ST7796 via HSPI), touch resistivo e barramento VSPI para microSD/RF.
- `env:ESP32-3248S035R-resistive`
  - Herda todas as configurações do ambiente acima e adiciona:
    - `-DUSE_RESISTIVE_TOUCH=1`
    - `-DTOUCH_TYPE=1`
    - `-DBRUCE_VERSION="dev"`
    - `-DGIT_COMMIT_HASH="custom"`

Comandos básicos (na raiz do projeto):

```bash
# Compilar
pio run -e ESP32-3248S035R-resistive

# Compilar e gravar
pio run -e ESP32-3248S035R-resistive -t upload

# Monitor serial
pio device monitor -b 115200
```

No primeiro boot da placa:

- O firmware executa automaticamente a calibração do touch resistivo.
- Os dados de calibração são salvos no arquivo `/calData` dentro do LittleFS.
- Para recalibrar, basta apagar o arquivo `/calData` (via WebUI ou gerenciador de arquivos) e reiniciar.

Outros detalhes importantes para a Sunton ESP32-3248S035R:

- Driver do display: ST7796, resolução 320x480, operando em modo landscape (ROTATION=1).
- Touch resistivo integrado via TFT_eSPI (XPT2046 compatível), dispensando a biblioteca separada `XPT2046_Touchscreen`.
- Backlight da tela controlado por PWM no pino 27 (TFT_BL).
- Sem PSRAM e sem bateria integrada: funções de bateria/charging retornam valores neutros.
- Áudio está desabilitado para manter simplicidade e compatibilidade com esta placa.

Para detalhes completos (pinout, arquivos da placa, integração com CC1101/NRF24/microSD etc.), consulte:

- `boards/ESP32-3248S035R/README.md` (documentação específica da placa)

## :keyboard: Servidor Discord

Entre em contato com a comunidade no [servidor oficial do Discord](https://discord.gg/WJ9XF9czVT).

## :bookmark_tabs: Wiki

Para mais informações sobre cada funcionalidade suportada pelo Bruce, consulte a wiki:

- [Wiki do projeto](https://github.com/pr3y/Bruce/wiki)  
- [FAQ](https://github.com/pr3y/Bruce/wiki/FAQ)

A wiki é o local principal para:

- Explicações detalhadas de cada módulo (WiFi, BLE, RF, RFID, etc.).
- Guias passo a passo para usar os ataques.
- Dicas de hardware e configurações específicas por dispositivo.

## :computer: Lista de funcionalidades

Abaixo um resumo das principais funcionalidades. Para detalhes, sempre use a Wiki, que está mais completa e atualizada.

<details>
  <summary><h2>WiFi</h2></summary>

- [x] Conectar ao WiFi
- [x] WiFi AP (ponto de acesso)
- [x] Desconectar do WiFi
- [x] [WiFi Atks](https://github.com/pr3y/Bruce/wiki/WiFi#wifi-atks)
  - [x] [Beacon Spam](https://github.com/pr3y/Bruce/wiki/WiFi#beacon-spam)
  - [x] [Target Atk](https://github.com/pr3y/Bruce/wiki/WiFi#target-atk)
    - [x] Informações sobre o alvo
    - [x] Deauth do alvo
    - [x] EvilPortal + Deauth
  - [x] Deauth Flood (mais de um alvo ao mesmo tempo)
- [x] [Wardriving](https://github.com/pr3y/Bruce/wiki/Wardriving)
- [x] [TelNet](https://github.com/pr3y/Bruce/wiki/WiFi#telnet)
- [x] [SSH](https://github.com/pr3y/Bruce/wiki/WiFi#ssh)
- [x] [RAW Sniffer](https://github.com/pr3y/Bruce/wiki/WiFi#raw-sniffer)
- [x] [TCP Client](https://github.com/pr3y/Bruce/wiki/WiFi#tcp-client)
- [x] [TCP Listener](https://github.com/pr3y/Bruce/wiki/WiFi#tcp-listener)
- [x] [DPWO-ESP32](https://github.com/pr3y/Bruce/wiki/WiFi#dpwo-esp32)
- [x] [Evil Portal](https://github.com/pr3y/Bruce/wiki/WiFi#evil-portal)
- [x] [Scan Hosts](https://github.com/pr3y/Bruce/wiki/WiFi#evil-portal)
- [x] [Wireguard Tunneling](https://github.com/pr3y/Bruce/wiki/WiFi#wireguard-tunneling)
- [x] Brucegotchi
  - [x] Amigo do Pwnagotchi
  - [x] Spam de faces & nomes no Pwngrid
    - [x] [Opcional] DoScreen com nomes e faces grandes
    - [x] [Opcional] Flood de identificadores únicos de peers

</details>

<details>
  <summary><h2>BLE</h2></summary>

- [X] [BLE Scan](https://github.com/pr3y/Bruce/wiki/BLE#ble-scan)
- [X] Bad BLE – executa scripts Ducky, similar ao [BadUsb](https://github.com/pr3y/Bruce/wiki/Others#badusb)
- [X] BLE Keyboard – apenas para Cardputer e T-Deck
- [X] iOS Spam
- [X] Windows Spam
- [X] Samsung Spam
- [X] Android Spam
- [X] Spam All (tudo)

</details>

<details>
  <summary><h2>RF</h2></summary>

- [x] Scan/Copy
- [x] [Custom SubGhz](https://github.com/pr3y/Bruce/wiki/RF#replay-payloads-like-flipper)
- [x] Spectrum (espectro de RF)
- [x] Jammer Full (sinal quadrado contínuo na saída)
- [x] Jammer Intermittent (sinal PWM na saída)
- [x] Config
  - [X] RF TX Pin
  - [X] RF RX Pin
  - [X] Módulo RF
    - [x] RF433 T/R M5Stack
    - [x] [CC1101 (Sub-Ghz)](https://github.com/pr3y/Bruce/wiki/CC1101)
  - [X] Frequência RF
- [x] Replay (reprodução de sinais)

</details>

<details>
  <summary><h2>RFID</h2></summary>

- [x] Ler tag
- [x] Ler 125 kHz
- [x] Clonar tag
- [x] Escrever registros NDEF
- [x] Amiibolink
- [x] Chameleon
- [x] Escrever dados
- [x] Apagar dados
- [x] Salvar em arquivo
- [x] Carregar de arquivo
- [x] Config
  - [X] [Módulo RFID](https://github.com/pr3y/Bruce/wiki/RFID#supported-modules)
    - [x] PN532
    - [x] PN532Killer
- [ ] Emular tag

</details>

<details>
  <summary><h2>IR</h2></summary>

- [x] TV-B-Gone
- [x] Receptor IR
- [x] [IR customizado (NEC, NECext, SIRC, SIRC15, SIRC20, Samsung32, RC5, RC5X, RC6)](https://github.com/pr3y/Bruce/wiki/IR#replay-payloads-like-flipper)
- [x] Config
  - [X] IR TX Pin
  - [X] IR RX Pin

</details>

<details>
  <summary><h2>FM</h2></summary>

- [x] [Broadcast padrão](https://github.com/pr3y/Bruce/wiki/FM#play_or_pause_button-broadcast-standard)
- [x] [Broadcast reservado](https://github.com/pr3y/Bruce/wiki/FM#no_entry_sign-broadcast-rerserved)
- [x] [Broadcast stop](https://github.com/pr3y/Bruce/wiki/FM#stop_button-broadcast-stop)
- [ ] [FM Spectrum](https://github.com/pr3y/Bruce/wiki/FM#ocean-fm-spectrum)
- [ ] [Hijack Traffic Announcements](https://github.com/pr3y/Bruce/wiki/FM#car-hijack-ta)
- [ ] [Config](https://github.com/pr3y/Bruce/wiki/FM#bookmark_tabs-config)

</details>

<details>
  <summary><h2>NRF24</h2></summary>

- [X] [NRF24 Jammer](https://github.com/pr3y/Bruce/wiki/BLE#nrf24-jammer)
- [X] Espectro 2.4G
- [ ] Mousejack

</details>

<details>
  <summary><h2>Scripts</h2></summary>

- [X] [Interpretador JavaScript](https://github.com/pr3y/Bruce/wiki/Interpreter)  
  (Créditos a [justinknight93](https://github.com/justinknight93/Doolittle))

</details>

<details>
  <summary><h2>Outros</h2></summary>

- [X] Mic Spectrum
- [X] QRCodes
  - [x] Custom
  - [x] PIX (sistema de transferência bancária do Brasil)
- [x] [Gerenciador de SD Card](https://github.com/pr3y/Bruce/wiki/Others#sd-card-mngr)
  - [x] Ver imagem (jpg)
  - [x] Info do arquivo
  - [x] [Upload para Wigle](https://github.com/pr3y/Bruce/wiki/Wardriving#how-to-upload)
  - [x] Reproduzir áudio
  - [x] Ver arquivo
- [x] [Gerenciador de LittleFS](https://github.com/pr3y/Bruce/wiki/Others#littlefs-mngr)
- [x] [WebUI](https://github.com/pr3y/Bruce/wiki/Others#webui)
  - [x] Estrutura do servidor
  - [x] HTML
  - [x] Gerenciador de SDCard
  - [x] Gerenciador de Spiffs
- [x] Megalodon
- [x] [BADUsb (novos recursos, LittleFS e SDCard)](https://github.com/pr3y/Bruce/wiki/Others#badusb)
- [x] Teclado USB – apenas para Cardputer e T-Deck
- [x] [Openhaystack](https://github.com/pr3y/Bruce/wiki/Others#openhaystack)
- [x] [iButton](https://github.com/pr3y/Bruce/wiki/Others#ibutton)
- [x] [Controle de LED](https://github.com/pr3y/Bruce/wiki/Others#led-control)

</details>

<details>
  <summary><h2>Relógio</h2></summary>

- [X] Suporte a RTC
- [X] Ajuste de hora via NTP
- [X] Ajuste manual

</details>

<details>
  <summary><h2>Connect (ESPNOW)</h2></summary>

- [X] Enviar arquivo
- [X] Receber arquivo
- [X] Enviar comandos
- [X] Receber comandos

</details>

<details>
  <summary><h2>Configurações</h2></summary>

- [x] Brilho
- [x] Tempo para escurecer (dim time)
- [x] Orientação
- [X] Cor da UI
- [x] Som de boot on/off
- [x] Relógio
- [x] Sleep
- [x] Restart

</details>

## Funções específicas por dispositivo

As funcionalidades abaixo são específicas por dispositivo. Tudo que não estiver marcado como restrito está disponível para todos.

| Dispositivo              | CC1101    | NRF24    | FM Radio  | PN532     | Mic   | BadUSB    | RGB Led | Speaker   | Fuel Guage | LITE_VERSION |
| ---                     | :---:     | :---:    | :---:     | :---:     | :---: | :---:     | :---:   | :---:     | :---:      | :---:        |
| [M5Stack Cardputer](https://shop.m5stack.com/products/m5stack-cardputer-kit-w-m5stamps)       | :ok:      | :ok:     | :ok:      | :ok:      | :ok:  | :ok:      | :ok:    | NS4168    | :x:        | :x:         |
| [M5Stack M5StickC PLUS2](https://shop.m5stack.com/products/m5stickc-plus2-esp32-mini-iot-development-kit)  | :ok:      | :ok:         | :ok:      | :ok:      | :ok:  | :ok:¹     | :x:     | Tone      | :x:        | :x:         |
| [M5Stack M5StickC PLUS](https://shop.m5stack.com/products/m5stickc-plus-esp32-pico-mini-iot-development-kit)   | :ok:      | :ok:          | :ok:      | :ok:      | :ok:  | :ok:¹     | :x:     | Tone      | :x:        | :x:²        |
| [M5Stack M5Core BASIC](https://shop.m5stack.com/products/basic-core-iot-development-kit)    | :ok:        | :ok:       | :ok:       | :ok:        | :ok:  | :ok:¹     | :x:     | Tone      | :x:        | :x:         |
| [M5Stack M5Core2](https://shop.m5stack.com/products/m5stack-core2-esp32-iot-development-kit-v1-1)    | :ok:        | :ok:          | :ok:        | :ok:        | :ok:  | :ok:¹     | :x:     | :x:       | :x:        | :x:         |
| [M5Stack M5CoreS3](https://shop.m5stack.com/products/m5stack-cores3-esp32s3-lotdevelopment-kit)/[SE](https://shop.m5stack.com/products/m5stack-cores3-se-iot-controller-w-o-battery-bottom)     | :ok:        | :ok:         | :ok:        | :ok:        | :x:   | :ok:      | :x:     | :x:       | :x:        | :x:         |
| [JCZN CYD&#x2011;2432S028](https://www.aliexpress.us/item/3256804774970998.html)       | :ok:      | :ok:     | :ok:       | :ok:      | :x:   | :ok:¹     | :x:     | :x:       | :x:        | :x:²        |
| Sunton ESP32-3248S035R (Cheap Yellow Display 3.5&quot; resistive) | :ok:      | :ok:     | :ok:       | :ok:      | :x:   | :ok:¹     | :x:     | :x:       | :x:        | :x:         |
| [Lilygo T&#x2011;Embed CC1101](https://lilygo.cc/products/t-embed-cc1101)   | :ok:      | :ok:         | :ok:       | :ok:      | :ok:  | :ok:      | :ok:    | :ok:      | :ok:       | :x:         |
| [Lilygo T&#x2011;Embed](https://lilygo.cc/products/t-embed)          | :ok:       | :ok:      | :ok:       | :ok:      | :ok:  | :ok:      | :ok:    | :ok:      | :x:        | :x:         |
| [Lilygo T-Display-S3](https://lilygo.cc/products/t-display-s3) | :ok:       | :ok:      | :x:       | :x:       | :x:   | :ok:      | :x:     | :x:       | :x:        | :x:         |
| [Lilygo T&#x2011;Deck](https://lilygo.cc/products/t-deck) ([and pro](https://lilygo.cc/products/t-deck-plus-1)) | :ok:       | :x:      | :x:       | :x:       | :x:   | :ok:      | :x:     | :x:       | :x:        | :x:         |
| [Lilygo T-Watch-S3](https://lilygo.cc/products/t-watch-s3) | :x:       | :x:      | :x:       | :x:       | :x:   | :ok:      | :x:     | :x:       | :x:        | :x:         |

² CYD possui uma versão LITE_VERSION para compatibilidade com o Launcher.  
¹ BadUSB em Core, CYD e StickCs: [detalhes aqui](https://github.com/pr3y/Bruce/wiki/Others#badusb)

*LITE_VERSION*: TelNet, SSH, DPWO, WireGuard, ScanHosts, RawSniffer, Brucegotchi, BLEBacon, BLEScan, Interpreter e OpenHaystack NÃO estão disponíveis para manter compatibilidade com o M5Launcher.

## :sparkles: Por que e como é a interface?

O Bruce nasceu de uma observação da comunidade em torno de dispositivos como o Flipper Zero. Embora esses dispositivos oferecessem uma introdução interessante ao mundo da segurança ofensiva, havia uma sensação clara de que dava para ir além, sem ser tão caro, aproveitando o ecossistema robusto e modular do hardware ESP32 (Lilygo, M5Stack e outros).

![Bruce Main Menu](./media/pictures/pic1.png)
![Bruce on M5Core](./media/pictures/core.png)
![Bruce on Stick](./media/pictures/stick.png)
![Bruce on CYD](./media/pictures/cyd.png)

Outras imagens podem ser encontradas em `./media/`.

## :clap: Agradecimentos

+ [@bmorcelli](https://github.com/bmorcelli) pelo novo core, várias funcionalidades novas e por portar o Bruce para muitos dispositivos.
+ [@IncursioHack](https://github.com/IncursioHack) pela adição de recursos para módulos RF e RFID.
+ [@Luidiblu](https://github.com/Luidiblu) pelo logo e ajuda com o design da UI.
+ [@eadmaster](https://github.com/eadmaster) por adicionar muitas funcionalidades.
+ [@rennancockles](https://github.com/rennancockles) por grande parte do código de RFID, refatorações e outras features.
+ [@7h30th3r0n3](https://github.com/7h30th3r0n3) por refatorações e muita ajuda com ataques WiFi.
+ [@Tawank](https://github.com/Tawank) por refatorar o interpretador, entre outras contribuições.
+ [@pablonymous]() por novas funções de RF para leitura de dados RAW.
+ [Smoochiee]() pelo design de PCBs do Bruce.
+ [TH3_KR4K3N]() pelo design da PCB extensor do Stick C Plus.
+ A todas as pessoas que contribuíram de alguma forma para o projeto, muito obrigado.

## :construction: Aviso (Disclaimer)

Bruce é uma ferramenta voltada para operações de segurança ofensiva e Red Team, distribuída sob os termos da Affero General Public License (AGPL).  
Ele é destinado exclusivamente a testes de segurança legais e autorizados. O uso deste software para atividades maliciosas ou não autorizadas é estritamente proibido.

Ao baixar, instalar ou utilizar o Bruce, você concorda em cumprir todas as leis e regulamentos aplicáveis.  
Este software é fornecido gratuitamente, e os desenvolvedores não aceitam pagamentos por cópias ou modificações.  
Os desenvolvedores do Bruce não se responsabilizam por qualquer uso indevido do software. Use por sua conta e risco.