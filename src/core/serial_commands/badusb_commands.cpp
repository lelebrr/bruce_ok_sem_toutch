#include "badusb_commands.h"
#include "core/sd_functions.h"
#include "helpers.h"
#include "modules/badusb_ble/ducky_typer.h"

void badusbFileCallback(cmd *c) {
    // badusb run_from_file HelloWorld.txt

    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.indexOf(".txt") == -1) {
        Serial.println("Invalid filename");
        return;
    }
    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs)) return;

    if (!(*fs).exists(filepath)) {
        Serial.println("File does not exist");
        return;
    }

#ifdef USB_as_HID
    ducky_startKb(hid_usb, KeyboardLayout_en_US, false);
    key_input(*fs, filepath, hid_usb);
    delete hid_usb;
    hid_usb = nullptr;

    // TODO: need to reinit serial when finished
    // Kb.end();
    // USB.~ESPUSB(); // Explicit call to destructor
    // Serial.begin(115200);
#endif
}

void badusbBufferCallback(cmd *c) {
    if (!(_setupPsramFs())) return;

    char *txt = _readFileFromSerial();
    String tmpfilepath = "/tmpramfile"; // TODO: Change to use char *txt directly
    File f = PSRamFS.open(tmpfilepath, FILE_WRITE);
    if (!f) return;

    f.write((const uint8_t *)txt, strlen(txt));
    f.close();
    free(txt);

#ifdef USB_as_HID
    ducky_startKb(hid_usb, KeyboardLayout_en_US, false);
    key_input(PSRamFS, tmpfilepath, hid_usb);
    delete hid_usb;
    hid_usb = nullptr;

    PSRamFS.remove(tmpfilepath);
#endif
    PSRamFS.remove(tmpfilepath);
}

void createBadUsbCommands(SimpleCLI *cli) {
    Command fileCmd = cli->addCommand("bu_run_from_file", badusbFileCallback);
    fileCmd.addPosArg("filepath");

    cli->addCommand("bu_run_from_buffer", badusbBufferCallback);
}
