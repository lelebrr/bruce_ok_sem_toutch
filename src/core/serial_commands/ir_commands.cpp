#include "ir_commands.h"
#include "core/sd_functions.h"
#include "helpers.h"
#include "modules/ir/custom_ir.h"
#include "modules/ir/ir_read.h"
#include <ArduinoJson.h>
#include <globals.h>

void irCallback(cmd *c) {
    Serial.println("Turning off IR LED");
    digitalWrite(bruceConfig.irTx, LED_OFF);
}

void irRxCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("raw");
    bool raw = arg.isSet();

    IrRead *i = NULL; // avoid calling the constructor here

    if (raw) {
        i = new IrRead(true, true); // true -> headless mode, true = raw mode
    } else {
        i = new IrRead(true); // true -> headless mode
    }

    Serial.println("Waiting for signal...");
    String r = i->loop_headless(10); // 10s timeout
    if (r.length() == 0) return;

    Serial.println(r);
    delete i;
}

void irTxCallback(cmd *c) {
    // usage: ir tx <protocol> <address without spaces> <command without spaces>
    // e.g. ir tx NEC 04000000 08000000

    Command cmd(c);

    Argument prtcArg = cmd.getArgument("protocol");
    Argument addrArg = cmd.getArgument("address");
    Argument cmdArg = cmd.getArgument("command");
    String protocol = prtcArg.getValue();
    String address = addrArg.getValue();
    String command = cmdArg.getValue();
    protocol.trim();
    address.trim();
    command.trim();

    if (address.length() != 8 || command.length() != 8) {
        Serial.println("Address and command must be 8 characters long");
        return;
    }

    IRCode code;
    code.type = "parsed";
    code.protocol = protocol;
    code.address = address;
    code.command = command;

    sendIRCommand(&code);
}

void irTxRawCallback(cmd *c) {
    // usage: ir tx_raw <frequency> <samples>

    Command cmd(c);

    Argument freqArg = cmd.getArgument("frequency");
    Argument samplesArg = cmd.getArgument("samples");
    String freqStr = freqArg.getValue();
    String samples = samplesArg.getValue();
    freqStr.trim();
    samples.trim();

    uint32_t frequency = freqStr.toInt();

    if (frequency == 0) {
        Serial.println("Invalid frequency: " + String(frequency));
        return;
    }

    if (samples.length() == 0) {
        Serial.println("Missing data samples");
        return;
    }

    IRCode code;
    code.type = "raw";
    code.frequency = frequency;
    code.data = samples;

    sendIRCommand(&code);
}

void irTxFileCallback(cmd *c) {
    // example: ir tx_from_file LG_AKB72915206_power.ir

    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.indexOf(".ir") == -1) {
        Serial.println("Invalid file");
        return;
    }

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs)) return;

    if (!(*fs).exists(filepath)) {
        Serial.println("File does not exist");
        return;
    }

    txIrFile(fs, filepath);
}

void irTxBufferCallback(cmd *c) {
    if (!(_setupPsramFs())) return;

    char *txt = _readFileFromSerial();
    String tmpfilepath = "/tmpramfile"; // TODO: Change to use char *txt directly
    File f = PSRamFS.open(tmpfilepath, FILE_WRITE);
    if (!f) return;

    f.write((const uint8_t *)txt, strlen(txt));
    f.close();
    free(txt);

    txIrFile(&PSRamFS, tmpfilepath);
    PSRamFS.remove(tmpfilepath);
}

void irSendCallback(cmd *c) {
    // tasmota json command  https://tasmota.github.io/docs/Tasmota-IR/#sending-ir-commands
    // e.g. IRSend {\"Protocol\":\"NEC\",\"Bits\":32,\"Data\":\"0x20DF10EF\"}
    // TODO: decode "data" into "address+command" and use existing "send*Command" funcs

    Command cmd(c);

    Argument args = cmd.getArgument(0);
    String args_str = args.getValue();
    args_str.trim();
    // Serial.println(command);

    JsonDocument jsonDoc;
    if (deserializeJson(jsonDoc, args_str)) {
        Serial.println("Failed to parse json");
        Serial.println(args_str);
        return;
    }

    JsonObject args_json = jsonDoc.as<JsonObject>(); // root

    uint16_t bits = 32;         // defaults to 32 bits
    String protocolStr = "nec"; // defaults to NEC protocol
    String dataStr = "";

    if (args_json["Data"].isNull()) {
        Serial.println("json missing data field");
        return;
    } else {
        dataStr = args_json["Data"].as<String>();
    }

    if (!args_json["Protocol"].isNull()) protocolStr = args_json["Protocol"].as<String>();

    if (!args_json["Bits"].isNull()) bits = args_json["Bits"].as<int>();

    sendDecodedCommand(protocolStr, dataStr, bits);
}

void createIrCommands(SimpleCLI *cli) {
    // Create flat IR commands since SimpleCLI doesn't support hierarchical commands
    Command irRxCmd = cli->addCmd("ir_rx", irRxCallback);
    irRxCmd.addFlagArg("raw");

    Command irTxCmd = cli->addCmd("ir_tx", irTxCallback);
    irTxCmd.addPositionalArgument("protocol");
    irTxCmd.addPositionalArgument("address");
    irTxCmd.addPositionalArgument("command");

    Command irTxRawCmd = cli->addCmd("ir_tx_raw", irTxRawCallback);
    irTxRawCmd.addPositionalArgument("frequency");
    irTxRawCmd.addPositionalArgument("samples");

    Command irTxFileCmd = cli->addCmd("ir_tx_file", irTxFileCallback);
    irTxFileCmd.addPositionalArgument("filepath");

    Command irTxBufferCmd = cli->addCmd("ir_tx_buffer", irTxBufferCallback);

    cli->addSingleArgCmd("IRSend", irSendCallback);
}
