/*
  Painel de Controle para Camper - Adaptado para Sunton ESP32-3248S035 (Toque Resistivo)

  INSTRUÇÕES DE CONFIGURAÇÃO IMPORTANTES:
  1.  **Biblioteca TFT_eSPI:** Este código requer a biblioteca TFT_eSPI.
  2.  **Configuração do Hardware:** Você DEVE configurar a biblioteca para o seu hardware.
      -   Vá para a pasta de bibliotecas do Arduino e encontre a TFT_eSPI.
      -   Edite o arquivo `User_Setup.h`.
      -   Descomente a linha `#define ST7796_DRIVER`.
      -   Defina os pinos do ESP32 para o display. Para esta placa, eles geralmente são:
          #define TFT_MOSI 13
          #define TFT_SCLK 14
          #define TFT_CS   15
          #define TFT_DC   2
          #define TFT_RST  -1 // ou o pino de reset se estiver conectado
          #define TFT_BL   27 // Controle da luz de fundo
      -   **HABILITE O TOQUE RESISTIVO** definindo os pinos do controlador XPT2046:
          #define TOUCH_CS 33 // Pino Chip Select para o controlador de toque
  3.  **Calibração de Toque:** Os valores de toque resistivo são brutos. Você precisa calibrá-los.
      -   Execute o exemplo `Touch_calibrate` da biblioteca TFT_eSPI para obter os valores MIN e MAX
          para os eixos X e Y da sua tela específica.
      -   Atualize as constantes `TOUCH_X_MIN`, `TOUCH_X_MAX`, `TOUCH_Y_MIN`, e `TOUCH_Y_MAX`
          abaixo com os valores que você encontrou.
  4.  **Arquivos de Imagem:** Os arquivos .h contendo os dados das imagens (ex: "kraan_groen.h")
      devem estar na mesma pasta que este arquivo .ino para que o código compile.
*/

#include <TFT_eSPI.h>
///FG///////
//#include "MAX1704X.h"
////////////////
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "NotoSansBold36.h"
#include "NotoSansMonoSCB20.h"

// Adicione os seus arquivos de cabeçalho de imagem na pasta 'include'
#include "camper_t66.h"
#include "kraan_groen.h"
#include "kraan_rood.h"
#include "maxxfan_groen.h"
#include "maxxfan_rood.h"
#include "vuilwatertank_groen.h"
#include "vuilwatertank_rood.h"


////////////////////////////////test for LCD brightness//////////
//#include <Pangodream_18650_CL.h>
//Pangodream_18650_CL BL;
//
///////////////////////////////////////////////////

////sprite testen
// Pause in milliseconds between screens, change to 0 to time font rendering
#define WAIT 500
//end sprite test

touch_pad_t touchPin;
#define Threshold 50 /* Greater the value, more the sensitivity ESP sleep */


//Timeout scherm instellingen
const unsigned long eventInterval = 20000;
const unsigned long eventInterval_Sleep = 50000;
unsigned long previousTime = 0;
unsigned long currentTime = 0;



#define MAX_IMAGE_WDITH 100  // Sets rendering line buffer lengths, adjust for y
our images
// Position variables must be global (PNGdec does not handle position coordinate
s)
int16_t xpos = 0;
uint16_t *ptr;

//button toggle
int lastButton_1_State;     // the previous state of button
int currentButton_1_State;  // the current state of button
int lastButton_2_State;     // the previous state of button
int currentButton_2_State;  // the current state of button
int lastButton_3_State;     // the previous state of button
int currentButton_3_State;  // the current state of button
int lastButton_4_State;     // the previous state of button
int currentButton_4_State;  // the current state of button
int lastEblState;
int currentEblState;
int textColor;
int textColor_uit;
int textColor_hum;
int contact_Stand;


unsigned long drawTime = 0;
unsigned long myTime;

// As definições de pino para o touch capacitivo foram removidas.
// O toque resistivo será tratado pela biblioteca TFT_eSPI.

int ledBacklight = 80;

#define USE_LINE_BUFFER  // Enable for faster rendering PNG


TFT_eSPI tft = TFT_eSPI();

TFT_eSprite buttonSprite = TFT_eSprite(&tft);
TFT_eSprite imageSprite = TFT_eSprite(&tft);
TFT_eSprite txtSprite = TFT_eSprite(&tft);
TFT_eSprite txt2Sprite = TFT_eSprite(&tft);
TFT_eSprite txt3Sprite = TFT_eSprite(&tft);
TFT_eSprite roundSprite = TFT_eSprite(&tft);
//TFT_eSprite blockSprite   = TFT_eSprite(&tft);


int backLightBrightness = 0;

// voor het tonen van JPG en PNG plaatjes /////
// Include the jpeg decoder library
#include <TJpg_Decoder.h>

//voor PNG
// Os arquivos de imagem .h precisam estar no mesmo diretório do sketch
// para que o compilador os encontre. Como não tenho os arquivos,
// comentei as linhas abaixo. Você precisará descomentá-las e garantir
// que os arquivos estejam no lugar certo.


#define USE_DMA

#ifdef USE_DMA
uint16_t dmaBuffer1[16 * 16];  // Toggle buffer for 16*16 MCU block, 512bytes
uint16_t dmaBuffer2[16 * 16];  // Toggle buffer for 16*16 MCU block, 512bytes
uint16_t *dmaBufferPtr = dmaBuffer1;
bool dmaBufferSel = 0;
#endif


//TFT parameters
#define TS_MINX 9    //  150
#define TS_MINY 18   //  120
#define TS_MAXX 315  //  920
#define TS_MAXY 475  //  940

#define MINPRESSURE 20
#define MAXPRESSURE 100

// Constantes de Calibração de Toque (ATUALIZE COM SEUS VALORES)
#define TOUCH_X_MIN 400
#define TOUCH_X_MAX 3700
#define TOUCH_Y_MIN 400
#define TOUCH_Y_MAX 3700

long lastTouch = millis();  // last touch time for minimum delay between touches
long tThresh = 300;        // minimum time between touches (reduzido para melhor responsividade)

float lastTemp = 0;  //we willen alleen temp update als deze binnengekomen is
float lastTemp_uit = 0;


// The font names are arrays references, thus must NOT be in quotes ""
//#define AA_FONT_SMALL NotoSansBold15
//#define AA_FONT_LARGE NotoSansBold36
#define AA_FONT_MONO NotoSansMonoSCB20
#define AA_FONT_BOLD NotoSansBold36
//#define AA_FONT_24 FreeSansBold24pt


// This next function will be called during decoding of the jpeg file to render
each
// 16x16 or 8x8 image tile (Minimum Coding Unit) to the TFT.
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
  // Stop further decoding as image is running off bottom of screen
  if (y >= tft.height()) return 0;

    // STM32F767 processor takes 43ms just to decode (and not draw) jpeg (-Os co
mpile option)
    // Total time to decode and also draw to TFT:
    // SPI 54MHz=71ms, with DMA 50ms, 71-43 = 28ms spent drawing, so DMA is comp
lete before next MCU block is ready
    // Apparent performance benefit of DMA = 71/50 = 42%, 50 - 43 = 7ms lost els
ewhere
    // SPI 27MHz=95ms, with DMA 52ms. 95-43 = 52ms spent drawing, so DMA is *jus
t* complete before next MCU block is ready!
    // Apparent performance benefit of DMA = 95/52 = 83%, 52 - 43 = 9ms lost els
ewhere
#ifdef USE_DMA
  // Double buffering is used, the bitmap is copied to the buffer by pushImageDM
A() the
  // bitmap can then be updated by the jpeg decoder while DMA is in progress
  if (dmaBufferSel) dmaBufferPtr = dmaBuffer2;
  else dmaBufferPtr = dmaBuffer1;
  dmaBufferSel = !dmaBufferSel;  // Toggle buffer selection
  //  pushImageDMA() will clip the image block at screen boundaries before initi
ating DMA
  tft.pushImageDMA(x, y, w, h, bitmap, dmaBufferPtr);  // Initiate DMA - blockin
g only if last DMA is not complete
  // The DMA transfer of image block to the TFT is now in progress...
#else
  // Non-DMA blocking alternative
  tft.pushImage(x, y, w, h, bitmap);  // Blocking, so only returns when image bl
ock is drawn
#endif
  // Return 1 to decode next block.
  return 1;
}


#define BLACK 0x0000
#define WHITE 0xFFFF
#define BLUE 0x001F
#define RED 0xF800
#define CP_BROWN 0xE5B0
#define CP_L_BROWN 0x9D7C
#define GREEN 0x2685
#define DarkBLUE 0x298B
#define ORANGE 0xFBA0
#define YELLOW 0xFFE0


// Constantes para a Interface do Usuário (UI)
#define BTN_WIDTH 100
#define BTN_HEIGHT 100

#define BTN_MAXXFAN_X 80
#define BTN_MAXXFAN_Y 110

#define BTN_KRAAN_X 200
#define BTN_KRAAN_Y 110

#define BTN_VUILWATER_X 304
#define BTN_VUILWATER_Y 110

int gedrukt_1 = 0;
int gedrukt_2 = 0;
int gedrukt_3 = 0;
int gedrukt_4 = 0;



/////////////////////////////////////////////////////////////////////////////

///ESP NOW instellingen //////
// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress[] = { 0xC8, 0xF0, 0x9E, 0xF6, 0xE8, 0x5C };


// Define variables to store incoming readings
float incomingTemp;
float incomingTemp_uit;
float incomingHum;
//float incomingPres;
int relais_1_stand;
int relais_2_stand;
int relais_3_stand;
int relais_4_stand;
int voeding_EBL_stand;
int contact_Aan;

//Define variables to store relais readings to be sent

int sturing_relais_1;
int sturing_relais_2;
int sturing_relais_3;
int sturing_relais_4;

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
  float temp;
  float temp_uit;
  float hum;
  //float pres;
  int sturing_relais_1;
  int sturing_relais_2;
  int sturing_relais_3;
  int sturing_relais_4;
  int relais_1_stand;
  int relais_2_stand;
  int relais_3_stand;
  int relais_4_stand;
  int voeding_EBL_stand;
  int contact_Aan;

} struct_message;



// Create a struct_message called outgoingReadings to hold sensor readings
struct_message outgoingReadings;

// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery
 Fail");
  if (status == 0) {
    success = "Delivery Success :)";
  } else {
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incomingTemp = incomingReadings.temp;
  incomingTemp_uit = incomingReadings.temp_uit;
  incomingHum = incomingReadings.hum;
  //incomingPres = incomingReadings.pres;
  relais_1_stand = incomingReadings.relais_1_stand;
  relais_2_stand = incomingReadings.relais_2_stand;
  relais_3_stand = incomingReadings.relais_3_stand;
  relais_4_stand = incomingReadings.relais_4_stand;
  voeding_EBL_stand = incomingReadings.voeding_EBL_stand;
  contact_Stand = incomingReadings.contact_Aan;


  if (lastEblState == 1 && voeding_EBL_stand == 0) {  //Relais 1,2 en 3 uit als
EBL uit is, werkt toch niet meer zonder voeding EBL

    if (relais_1_stand == 1) {
      outgoingReadings.sturing_relais_1 = 0;
    }
    if (relais_2_stand == 1) {
      outgoingReadings.sturing_relais_2 = 0;
    }
    if (relais_3_stand == 1) {
      outgoingReadings.sturing_relais_3 = 0;
    }

    send_message();
  }

  if (lastEblState == 0 && voeding_EBL_stand == 1) {  //Als relais 4 aan is (BYP
ASS),dan mag deze switchen naar relais 1 als EBL weer aan is//

    if (relais_4_stand == 1) {
      outgoingReadings.sturing_relais_4 = 0;
      outgoingReadings.sturing_relais_1 = 1;
      send_message();
    }
  }

  lastEblState = voeding_EBL_stand;

  // Sincroniza o estado dos botões locais com o estado real dos relés para corrigir a lógica de toggle
  gedrukt_1 = (relais_1_stand == 1 || relais_4_stand == 1);
  gedrukt_2 = relais_2_stand;
  gedrukt_3 = relais_3_stand;

  updateDisplay();
}

// END ESP NOW instellingen ///


void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  pinMode(27, OUTPUT);
  digitalWrite(27, HIGH);

  // A inicialização do touch capacitivo foi removida.

  // O código do medidor de combustível foi removido para limpeza.



  // TFT scherm starten

  tft.init();
  tft.setRotation(3);
  tft.setSwapBytes(true);
  tft.fillScreen(DarkBLUE);


  // UIt slaap reden
  print_wakeup_reason();
  print_wakeup_touchpad();

  // Set device as  Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.println("WIFI starten");

  delay(2000);


  // tft.fillScreen(DarkBLUE);

  // Opstartscherm tonen
  StartupScreen();  //  Display Startup Screen em starten WIFI
  delay(2000);


  esp_sleep_enable_timer_wakeup(10 * 1000 * 1000);


  imageSprite.createSprite(100, 100);

  buttonSprite.createSprite(100, 100);
  buttonSprite.setSwapBytes(true);

  txtSprite.setColorDepth(16);
  txtSprite.createSprite(350, 25);

  txt2Sprite.setColorDepth(16);
  txt2Sprite.createSprite(75, 25);


  Serial.println("fase 1");


  // Init ESP-NOW
  Serial.println("Init ESP Now");
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet

  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
  // END ESP deel


  Serial.println("fase 2");
  //delay(5000);

  Serial.println("Camper sturing: Ready");
  // delay(5000);


#ifdef USE_DMA
  tft.initDMA();  // To use SPI DMA you must call initDMA() to setup the DMA eng
ine
#endif

  // The jpeg image can be scaled down by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);

  // The colour byte order can be swapped by the decoder
  // using TJpgDec.setSwapBytes(true); or by the TFT_eSPI library:
  tft.setSwapBytes(true);

  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);

  // Must use startWrite first so TFT chip select stays low during DMA and SPI c
hannel settings remain configured
  tft.startWrite();

  // Draw the image, top left at 0,0 - DMA request is handled in the call-back t
ft_output() in this sketch
  TJpgDec.drawJpg(0, 0, camper_t66, sizeof(camper_t66));

  TJpgDec.setJpgScale(2);


  // Must use endWrite to release the TFT chip select and release the SPI channe
l
  tft.endWrite();

  //EBL veld

  txt2Sprite.loadFont(AA_FONT_MONO);
  txt2Sprite.fillSprite(CP_BROWN);
  txt2Sprite.fillRoundRect(0, 0, 75, 25, 5, TFT_BROWN);
  txt2Sprite.setTextColor(TFT_YELLOW, TFT_BROWN);

  txt2Sprite.drawString("EBL", 3, 3);
  txt2Sprite.pushSprite(345, 270, CP_BROWN);  //eerste= L/R  tweede= Up/Down
  txtSprite.unloadFont();

  Serial.println("[APP] Free memory: " + String(esp_get_free_heap_size()) + " by
tes");

  currentButton_1_State = 0;
  currentButton_2_State = 0;
  currentButton_3_State = 0;
  currentButton_4_State = 0;
  currentEblState = 0;
}

void loop() {


  String str;

  currentTime = millis();

  //Serial.print("UpTime:");
  //Serial.println(currentTime);




  if (currentTime < 13000) {  // na reboot alles op nul
    Serial.print("Time:");
    Serial.println(currentTime);

    outgoingReadings.sturing_relais_1 = currentButton_1_State;  //= alles op nul
 sturen
    outgoingReadings.sturing_relais_2 = currentButton_2_State;  //= alles op nul
 sturen
    outgoingReadings.sturing_relais_3 = currentButton_3_State;  //= alles op nul
 sturen
    outgoingReadings.sturing_relais_4 = currentButton_4_State;  //= alles op nul
 sturen
    send_message();
  }

  // Timout scherm test
  //currentTime = millis();

  if (currentTime - previousTime >= eventInterval && digitalRead(27) == 1) {
    Serial.println("SCREEN turning off now");
    digitalWrite(27, LOW);
  }

  if (currentTime - previousTime >= eventInterval_Sleep) {
    Serial.println("Going to sleep now");
    esp_wifi_stop();
    esp_light_sleep_start();
    esp_wifi_start();
  }

  //********************* Lógica de Desenho dos Botões *********************
  // Os botões são redesenhados apenas quando o estado deles muda.

  if (gedrukt_1 != lastButton_1_State) {
    if (gedrukt_1) {
      buttonSprite.pushImage(0, 0, BTN_WIDTH, BTN_HEIGHT, maxxfan_rood);
      if (voeding_EBL_stand == 0) {
        buttonSprite.setTextColor(TFT_BLACK, CP_BROWN);
        buttonSprite.loadFont(AA_FONT_MONO);
        buttonSprite.drawString("BYPASS", 19, 48);
      }
    } else {
      buttonSprite.pushImage(0, 0, BTN_WIDTH, BTN_HEIGHT, maxxfan_groen);
    }
    buttonSprite.pushSprite(BTN_MAXXFAN_X, BTN_MAXXFAN_Y, TFT_WHITE);
    lastButton_1_State = gedrukt_1;
  }

  if (gedrukt_2 != lastButton_2_State) {
    if (gedrukt_2 && voeding_EBL_stand == 1 && contact_Stand == 1) {
      buttonSprite.pushImage(0, 0, BTN_WIDTH, BTN_HEIGHT, kraan_rood);
    } else {
      buttonSprite.pushImage(0, 0, BTN_WIDTH, BTN_HEIGHT, kraan_groen);
    }
    buttonSprite.pushSprite(BTN_KRAAN_X, BTN_KRAAN_Y, TFT_WHITE);
    lastButton_2_State = gedrukt_2;
  }

  if (gedrukt_3 != lastButton_3_State) {
    if (gedrukt_3 && voeding_EBL_stand == 1) {
      buttonSprite.pushImage(0, 0, BTN_WIDTH, BTN_HEIGHT, vuilwatertank_rood);
    } else {
      buttonSprite.pushImage(0, 0, BTN_WIDTH, BTN_HEIGHT, vuilwatertank_groen);
    }
    buttonSprite.pushSprite(BTN_VUILWATER_X, BTN_VUILWATER_Y, TFT_WHITE);
    lastButton_3_State = gedrukt_3;
  }


  // A lógica de estado agora é tratada diretamente na seção de desenho dos botões


  //  Binnen Temperatuur veld
  if (lastTemp != incomingTemp || lastTemp_uit != incomingTemp_uit) {

    Serial.print("IN temp ");
    Serial.println(incomingTemp);
    Serial.print("OUT temp ");
    Serial.println(incomingTemp_uit);
    if (incomingTemp < 18) {
      textColor = BLUE;
    } else if (incomingTemp >= 19 && incomingTemp <= 21) {
      textColor = GREEN;
    } else if (incomingTemp >= 22 && incomingTemp <= 24) {
      textColor = YELLOW;
    } else if (incomingTemp >= 25 && incomingTemp <= 27) {
      textColor = ORANGE;
    } else if (incomingTemp > 27) {
      textColor = RED;
    }

    if (incomingTemp_uit < 18) {
      textColor_uit = BLUE;
    } else if (incomingTemp_uit >= 19 && incomingTemp_uit <= 21) {
      textColor_uit = GREEN;
    } else if (incomingTemp_uit >= 22 && incomingTemp_uit <= 24) {
      textColor_uit = YELLOW;
    } else if (incomingTemp_uit >= 25 && incomingTemp_uit <= 27) {
      textColor_uit = ORANGE;
    } else if (incomingTemp_uit > 27) {
      textColor_uit = RED;
    }

    if (incomingHum < 18) {
      textColor_hum = RED;
    } else if (incomingHum >= 19 && incomingHum <= 30) {
      textColor_hum = ORANGE;
    } else if (incomingHum >= 31 && incomingHum <= 60) {
      textColor_hum = YELLOW;
    } else if (incomingHum >= 61 && incomingHum <= 80) {
      textColor_hum = ORANGE;
    } else if (incomingHum > 80) {
      textColor_hum = RED;
    }





    txtSprite.fillSprite(CP_BROWN);
    txtSprite.setTextColor(BLACK, CP_BROWN);
    txtSprite.loadFont(AA_FONT_MONO);
    //txtSprite.loadFont(AA_FONT_BOLD);
    txtSprite.setTextSize(2);




    int xpos = 0;
    xpos += txtSprite.drawString("In: ", 0, 0);
    txtSprite.setTextColor(textColor, CP_BROWN);
    xpos += txtSprite.drawFloat(incomingReadings.temp, 0, xpos, 0);
    xpos += txtSprite.drawChar(247, xpos, 0);
    xpos += txtSprite.drawString("C ", xpos, 0);
    txtSprite.setTextColor(BLACK, CP_BROWN);
    xpos += txtSprite.drawString("|", xpos, 0);
    xpos += txtSprite.drawString("RV: ", xpos, 0);
    txtSprite.setTextColor(textColor_hum, CP_BROWN);
    xpos += txtSprite.drawFloat(incomingReadings.hum, 0, xpos, 0);
    xpos += txtSprite.drawString("% ", xpos, 0);
    txtSprite.setTextColor(BLACK, CP_BROWN);
    xpos += txtSprite.drawString("|Out: ", xpos, 0);
    txtSprite.setTextColor(textColor_uit, CP_BROWN);
    xpos += txtSprite.drawFloat(incomingReadings.temp_uit, 0, xpos, 0);
    xpos += txtSprite.drawChar(247, xpos, 0);
    xpos += txtSprite.drawString("C", xpos, 0);
    txtSprite.setTextColor(BLACK, CP_BROWN);
    xpos += txtSprite.drawString("|", xpos, 0);
    xpos += txtSprite.drawString("100 ", xpos, 0);
    xpos += txtSprite.drawString("% ", xpos, 0);
    txtSprite.pushSprite(105, 30, TFT_WHITE);  //delay(WAIT);


    //maak een bredere rand
    int thick = 3;
    int radius = 5;
    for (int i = 0; i < thick; i++) {
      tft.drawRect(100 + i, 27 + i, 350 - i * 2, 25 - i * 2, TFT_BLACK);
      //tft.drawRect(200 + i, 41 + i, 90 - i*2, 27 - i*2, TFT_BLACK);
    }
    lastTemp = incomingReadings.temp;
    lastTemp_uit = incomingReadings.temp_uit;
    txtSprite.unloadFont();
  }



  //EBL status weergeven

  if (voeding_EBL_stand == 1) {
    tft.fillCircle(400, 283, 9, RED);
  } else {
    tft.fillCircle(400, 283, 9, GREEN);
  }


  // Lógica de toque para tela resistiva
  uint16_t x, y;
  if (millis() - lastTouch > tThresh && mapTouchToScreen(&x, &y)) {
    lastTouch = millis(); // Reset touch debounce timer
    Click(); // Processa o clique (acende a tela, etc.)
    Serial.print("Toque mapeado para: x="); Serial.print(x); Serial.print(", y="); Serial.println(y);

    // Button Maxfan
    if ((x > BTN_MAXXFAN_X) && (x < (BTN_MAXXFAN_X + BTN_WIDTH)) && (y > BTN_MAXXFAN_Y) && (y < (BTN_MAXXFAN_Y + BTN_HEIGHT))) {
      Serial.print("MaxxFan: ");
      if (gedrukt_1 == 0) {
        if (voeding_EBL_stand == 1) { // Als EBL uit willen we toch ventilator aan
          Serial.println("gedrukt aan");
          outgoingReadings.sturing_relais_1 = 1;
          outgoingReadings.sturing_relais_4 = 0;
          gedrukt_1 = 1;
        } else {
          Serial.println("gedrukt aan Bypass mode");
          outgoingReadings.sturing_relais_1 = 0;
          outgoingReadings.sturing_relais_4 = 1;
          gedrukt_1 = 1;
        }
      } else {
        Serial.println("gedrukt uit");
        outgoingReadings.sturing_relais_1 = 0;
        outgoingReadings.sturing_relais_4 = 0;
        gedrukt_1 = 0;
      }
      send_message();
    }

    // Button Kraan
    if ((x > BTN_KRAAN_X) && (x < (BTN_KRAAN_X + BTN_WIDTH)) && (y > BTN_KRAAN_Y) && (y < (BTN_KRAAN_Y + BTN_HEIGHT))) {
      if (voeding_EBL_stand == 1 && contact_Stand == 1) {
        Serial.println("Waterpomp ");
        if (gedrukt_2 == 0) {
          Serial.println("gedrukt aan");
          outgoingReadings.sturing_relais_2 = 1;
          gedrukt_2 = 1;
        } else {
          outgoingReadings.sturing_relais_2 = 0;
          Serial.println("gedrukt uit");
          gedrukt_2 = 0;
        }
        send_message();
      }
    }

    // Button Vuilwatertank
    if ((x > BTN_VUILWATER_X) && (x < (BTN_VUILWATER_X + BTN_WIDTH)) && (y > BTN_VUILWATER_Y) && (y < (BTN_VUILWATER_Y + BTN_HEIGHT))) {
      if (voeding_EBL_stand == 1) {
        Serial.println("Vuilwatertank ");
        if (gedrukt_3 == 0) {
          Serial.println("gedrukt aan");
          outgoingReadings.sturing_relais_3 = 1;
          gedrukt_3 = 1;
        } else {
          outgoingReadings.sturing_relais_3 = 0;
          Serial.println("gedrukt uit");
          gedrukt_3 = 0;
        }
        send_message();
      }
    }
  }
}

// Função para mapear as coordenadas brutas do toque para as coordenadas da tela
bool mapTouchToScreen(uint16_t *x, uint16_t *y) {
  if (tft.getTouch(x, y)) {
    *x = map(*x, TOUCH_X_MIN, TOUCH_X_MAX, 0, tft.width());
    *y = map(*y, TOUCH_Y_MIN, TOUCH_Y_MAX, 0, tft.height());
    return true;
  }
  return false;
}

void send_message() {

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&outgoingReadings
, sizeof(outgoingReadings));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }

  updateDisplay();
}


void updateDisplay() {

  // Display Readings in Serial Monitor
  Serial.println("INCOMING READINGS");
  Serial.print("Temperature in: ");
  Serial.print(incomingReadings.temp);
  Serial.println(" ºC");
  Serial.print("Temperature out: ");
  Serial.print(incomingReadings.temp_uit);
  Serial.println(" ºC");
  Serial.print("Humidity: ");
  Serial.print(incomingReadings.hum);
  Serial.println(" %");
  Serial.println();
  Serial.print("Stand relais_1: ");
  Serial.println(incomingReadings.relais_1_stand);
  Serial.print("Stand relais_2: ");
  Serial.println(incomingReadings.relais_2_stand);
  Serial.print("Stand relais_3: ");
  Serial.println(incomingReadings.relais_3_stand);
  Serial.print("Stand relais_4: ");
  Serial.println(incomingReadings.relais_4_stand);
  Serial.print("Voeding EBL stand ");
  Serial.println(incomingReadings.voeding_EBL_stand);
  Serial.print("Contact stand ");
  Serial.println(incomingReadings.contact_Aan);
}

void Click() {
  previousTime = currentTime;
  if (digitalRead(27) == LOW) {
    Serial.println("SCREEN turning on now");
    digitalWrite(27, HIGH);
  }
}

//voor ESP sleep
void callback() {
  //placeholder callback function
  Serial.println("Awake now........");
}

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;
  Serial.println("wake_up_reason......");

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0: Serial.println("Wakeup caused by external signal
 using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1: Serial.println("Wakeup caused by external signal
 using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup caused by timer"); break
;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad");
 break;
    case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup caused by ULP program"); b
reak;
    default: Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_r
eason); break;
  }
}

void print_wakeup_touchpad() {
  touchPin = esp_sleep_get_touchpad_wakeup_status();
  Serial.println("wake_up_touchpad......");
  switch (touchPin) {
    case 0: Serial.println("Touch detected on GPIO 4"); break;
    case 1: Serial.println("Touch detected on GPIO 0"); break;
    case 2: Serial.println("Touch detected on GPIO 2"); break;
    case 3: Serial.println("Touch detected on GPIO 15"); break;
    case 4: Serial.println("Touch detected on GPIO 13"); break;
    case 5: Serial.println("Touch detected on GPIO 12"); break;
    case 6: Serial.println("Touch detected on GPIO 14"); break;
    case 7: Serial.println("Touch detected on GPIO 27"); break;
    case 8: Serial.println("Touch detected on GPIO 33"); break;
    case 9: Serial.println("Touch detected on GPIO 32"); break;
    default: Serial.println("Wakeup not by touchpad"); break;
  }
}


//////// Fuel gauge/////

/////// END //////////
