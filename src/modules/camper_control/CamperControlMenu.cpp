#include "CamperControlMenu.h"
#include "core/display_functions.h" // Para funções de desenho
#include <TFT_eSPI.h> // Para as definições de cores
#include <globals.h> // Para acesso a variáveis globais como tft, touchPoint, etc.

// === Configurações da Interface ===
// Cores
#define CP_BROWN 0xE5B0
#define CP_L_BROWN 0x9D7C
#define DarkBLUE 0x298B

// Coordenadas e Dimensões dos Botões
#define BTN_WIDTH 100
#define BTN_HEIGHT 100
#define BTN_MAXXFAN_X 80
#define BTN_MAXXFAN_Y 110
#define BTN_KRAAN_X 200
#define BTN_KRAAN_Y 110
#define BTN_VUILWATER_X 304
#define BTN_VUILWATER_Y 110

// MAC Address do dispositivo receptor (precisa ser alterado)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Variáveis de estado globais para este módulo
struct_message incomingReadings;
struct_message outgoingReadings;

bool gedrukt_1 = false;
bool gedrukt_2 = false;
bool gedrukt_3 = false;
bool gedrukt_4 = false;

// Instância da classe para o callback estático
CamperControlMenu* camperInstance = nullptr;

CamperControlMenu::CamperControlMenu() : MenuItemInterface("Camper Control") {
    camperInstance = this;
}

CamperControlMenu::~CamperControlMenu() {}

void CamperControlMenu::begin() {
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_send_cb(OnDataSent);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);
}

void CamperControlMenu::optionsMenu(void) {
    // Loop principal da tela do painel do campista
    returnToMenu = false;
    while (!returnToMenu) {
        drawScreen(); // Desenha a tela

        // Verifica a entrada de toque
        if (touchPoint.pressed) {
            handleTouch(touchPoint.x, touchPoint.y);
            touchPoint.Clear(); // Limpa o ponto de toque para evitar processamento repetido
        }

        // Verifica o botão de voltar (geralmente um botão de hardware ou um gesto)
        if (EscPress) {
            returnToMenu = true;
        }

        vTaskDelay(pdMS_TO_TICKS(50)); // Pequeno delay para não sobrecarregar a CPU
    }
}

void CamperControlMenu::drawIcon(float scale) {
    // Desenha um ícone simples para o menu (pode ser melhorado)
    tft.drawRect(iconAreaX, iconAreaY, iconAreaW, iconAreaH, bruceConfig.fgColor);
    tft.setCursor(iconAreaX + 10, iconAreaY + 10);
    tft.print("Camper");
}

void CamperControlMenu::drawIconImg() {
    // Lógica para desenhar um ícone a partir de uma imagem (se aplicável)
}

bool CamperControlMenu::getTheme() {
    return false; // Não usa tema de imagem por padrão
}

void CamperControlMenu::drawScreen() {
    tft.fillScreen(DarkBLUE);

    // --- Desenha os Botões ---
    // Botão MaxxFan
    if (gedrukt_1) {
        tft.fillRect(BTN_MAXXFAN_X, BTN_MAXXFAN_Y, BTN_WIDTH, BTN_HEIGHT, TFT_RED);
        if (incomingReadings.voeding_EBL_stand == 0) {
            tft.setTextColor(TFT_BLACK);
            tft.drawCentreString("BYPASS", BTN_MAXXFAN_X + BTN_WIDTH / 2, BTN_MAXXFAN_Y + BTN_HEIGHT / 2, 1);
        }
    } else {
        tft.fillRect(BTN_MAXXFAN_X, BTN_MAXXFAN_Y, BTN_WIDTH, BTN_HEIGHT, TFT_GREEN);
    }

    // Botão Kraan (Bomba d'água)
    if (gedrukt_2) {
        tft.fillRect(BTN_KRAAN_X, BTN_KRAAN_Y, BTN_WIDTH, BTN_HEIGHT, TFT_RED);
    } else {
        tft.fillRect(BTN_KRAAN_X, BTN_KRAAN_Y, BTN_WIDTH, BTN_HEIGHT, TFT_GREEN);
    }

    // Botão Vuilwatertank (Aquecimento do tanque)
    if (gedrukt_3) {
        tft.fillRect(BTN_VUILWATER_X, BTN_VUILWATER_Y, BTN_WIDTH, BTN_HEIGHT, TFT_RED);
    } else {
        tft.fillRect(BTN_VUILWATER_X, BTN_VUILWATER_Y, BTN_WIDTH, BTN_HEIGHT, TFT_GREEN);
    }

    // --- Desenha as Informações de Sensores ---
    tft.setTextColor(TFT_WHITE, DarkBLUE);
    char buffer[50];
    sprintf(buffer, "In: %.1f C | RV: %.0f%% | Out: %.1f C", incomingReadings.temp, incomingReadings.hum, incomingReadings.temp_uit);
    tft.drawCentreString(buffer, tft.width() / 2, 30, 2);

    // --- Status EBL ---
    if (incomingReadings.voeding_EBL_stand == 1) {
        tft.fillCircle(400, 283, 9, TFT_RED);
    } else {
        tft.fillCircle(400, 283, 9, TFT_GREEN);
    }
}

void CamperControlMenu::handleTouch(uint16_t x, uint16_t y) {
    // Botão MaxxFan
    if ((x > BTN_MAXXFAN_X) && (x < (BTN_MAXXFAN_X + BTN_WIDTH)) && (y > BTN_MAXXFAN_Y) && (y < (BTN_MAXXFAN_Y + BTN_HEIGHT))) {
        if (!gedrukt_1) {
            if (incomingReadings.voeding_EBL_stand == 1) {
                outgoingReadings.sturing_relais_1 = 1;
                outgoingReadings.sturing_relais_4 = 0;
            } else {
                outgoingReadings.sturing_relais_1 = 0;
                outgoingReadings.sturing_relais_4 = 1;
            }
        } else {
            outgoingReadings.sturing_relais_1 = 0;
            outgoingReadings.sturing_relais_4 = 0;
        }
        send_message();
        return;
    }

    // Botão Kraan
    if ((x > BTN_KRAAN_X) && (x < (BTN_KRAAN_X + BTN_WIDTH)) && (y > BTN_KRAAN_Y) && (y < (BTN_KRAAN_Y + BTN_HEIGHT))) {
        if (incomingReadings.voeding_EBL_stand == 1 && incomingReadings.contact_Aan == 1) {
            outgoingReadings.sturing_relais_2 = !gedrukt_2;
            send_message();
        }
        return;
    }

    // Botão Vuilwatertank
    if ((x > BTN_VUILWATER_X) && (x < (BTN_VUILWATER_X + BTN_WIDTH)) && (y > BTN_VUILWATER_Y) && (y < (BTN_VUILWATER_Y + BTN_HEIGHT))) {
        if (incomingReadings.voeding_EBL_stand == 1) {
            outgoingReadings.sturing_relais_3 = !gedrukt_3;
            send_message();
        }
        return;
    }
}

void CamperControlMenu::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Last Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void CamperControlMenu::OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
    // Sincroniza o estado dos botões
    gedrukt_1 = (incomingReadings.relais_1_stand == 1 || incomingReadings.relais_4_stand == 1);
    gedrukt_2 = incomingReadings.relais_2_stand;
    gedrukt_3 = incomingReadings.relais_3_stand;
}

void CamperControlMenu::send_message() {
    esp_now_send(broadcastAddress, (uint8_t *)&outgoingReadings, sizeof(outgoingReadings));
}
