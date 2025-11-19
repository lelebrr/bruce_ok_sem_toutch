#include "crypto_commands.h"
#include "core/passwords.h"
#include "core/sd_functions.h"
#include "helpers.h"
#include "modules/badusb_ble/ducky_typer.h"
#include <globals.h>

void decryptFileCallback(cmd *c) {
    // crypto decrypt_from_file passwords/github.com.txt.enc 1234

    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    Argument pwdArg = cmd.getArgument("password");
    String filepath = arg.getValue();
    String password = pwdArg.getValue();
    filepath.trim();
    password.trim();

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    cachedPassword = password;

    FS *fs;
    if (!getFsStorage(fs)) return;

    if (!(*fs).exists(filepath)) {
        Serial.println("File does not exist");
        return;
    }

    String plaintext = readDecryptedFile(*fs, filepath);
    if (plaintext == "") return;

    Serial.println(plaintext);
}

void encryptFileCallback(cmd *c) {
    // crypto encrypt_to_file passwords/github.com.txt.enc 1234

    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    Argument pwdArg = cmd.getArgument("password");
    String filepath = arg.getValue();
    String password = pwdArg.getValue();
    filepath.trim();
    password.trim();

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    cachedPassword = password;

    char *txt = _readFileFromSerial();
    if (strlen(txt) == 0) return;
    String txtString = String(txt);

    FS *fs;
    if (!getFsStorage(fs)) return;

    File f = fs->open(filepath, FILE_WRITE);
    if (!f) return;

    String cyphertxt = encryptString(txtString, cachedPassword);
    if (cyphertxt == "") return;

    f.write((const uint8_t *)cyphertxt.c_str(), cyphertxt.length());
    f.close();
    Serial.println("File written: " + filepath);
}

void typeFileCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    Argument pwdArg = cmd.getArgument("password");
    String filepath = arg.getValue();
    String password = pwdArg.getValue();
    filepath.trim();
    password.trim();

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    cachedPassword = password;

    FS *fs;
    if (!getFsStorage(fs)) return;

    if (!(*fs).exists(filepath)) {
        Serial.println("File does not exist");
        return;
    }

    String plaintext = readDecryptedFile(*fs, filepath);
    if (plaintext == "") return;

    Serial.println(plaintext);

    key_input_from_string(plaintext);
}

void createCryptoCommands(SimpleCLI *cli) {
    Command decryptCmd = cli->addCommand("crypto_decrypt_from_file", decryptFileCallback);
    decryptCmd.addPosArg("filepath");
    decryptCmd.addPosArg("password");

    Command encryptCmd = cli->addCommand("crypto_encrypt_to_file", encryptFileCallback);
    encryptCmd.addPosArg("filepath");
    encryptCmd.addPosArg("password");

#ifdef USB_as_HID
    Command typeFileCmd = cli->addCommand("crypto_type_from_file", typeFileCallback);
    typeFileCmd.addPosArg("filepath");
    typeFileCmd.addPosArg("password");
#endif
}
