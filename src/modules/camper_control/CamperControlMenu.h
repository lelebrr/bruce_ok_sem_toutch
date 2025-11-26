#ifndef __CAMPER_CONTROL_MENU_H__
#define __CAMPER_CONTROL_MENU_H__

#include <MenuItemInterface.h>
#include <esp_now.h>
#include <WiFi.h>

// Estrutura para os dados enviados e recebidos via ESP-NOW
typedef struct struct_message {
    float temp;
    float temp_uit;
    float hum;
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

class CamperControlMenu : public MenuItemInterface {
public:
    CamperControlMenu();
    ~CamperControlMenu();

    // Métodos herdados de MenuItemInterface
    void optionsMenu(void) override;
    void drawIcon(float scale = 1) override;
    void drawIconImg() override;
    bool getTheme() override;

    // Métodos específicos do nosso módulo
    void begin(); // Para inicializar o ESP-NOW
    void drawScreen();
    void handleTouch(uint16_t x, uint16_t y);

private:
    // Funções de callback do ESP-NOW
    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
    static void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

    void send_message();
};

#endif
