#pragma once
#include <Arduino.h>

enum class KeyAction {
    None,
    Next,
    Prev,
    Up,
    Down,
    Select,
    Escape,
    NextPage,
    PrevPage,
    LongPress
};

struct NavigationState {
    volatile bool any_key_pressed = false;
    volatile KeyAction last_action = KeyAction::None;
    // ... outras variáveis de estado de entrada, como touch ...
};

// Singleton para garantir uma única instância do estado de entrada
class InputManager {
public:
    static InputManager& getInstance() {
        static InputManager instance;
        return instance;
    }

    NavigationState state;

private:
    InputManager() {} // Construtor privado
    InputManager(InputManager const&) = delete; // Impede a cópia
    void operator=(InputManager const&) = delete; // Impede a atribuição
};