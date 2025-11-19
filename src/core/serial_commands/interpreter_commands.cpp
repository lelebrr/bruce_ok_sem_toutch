#include "interpreter_commands.h"
#include "core/sd_functions.h"
#include "helpers.h"
#include "modules/bjs_interpreter/interpreter.h"

void jsFileCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    /*
    if(filepath.isEmpty()) {
        Serial.println("Running inline script");

        char *txt = strdup(filepath.c_str());
        run_bjs_script_headless(txt);
        // *txt is freed by js interpreter
        return;
    }*/

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs)) return;

    run_bjs_script_headless(*fs, filepath);
}

/*
uint32_t jsBufferCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("fileSize");
    String strFileSize = arg.getValue();
    strFileSize.trim();

    int fileSize = strFileSize.toInt() + 2;
    if(fileSize < SAFE_STACK_BUFFER_SIZE) fileSize = SAFE_STACK_BUFFER_SIZE;
    char *txt = _readFileFromSerial( fileSize );

    return run_bjs_script_headless(txt);
    // *txt is freed by js interpreter
}*/

void jsBufferCallback(cmd *c) {
    if (!(_setupPsramFs())) return;

    char *txt = _readFileFromSerial();
    String tmpfilepath = "/tmpramfile"; // TODO: Change to use char *txt directly
    File f = PSRamFS.open(tmpfilepath, FILE_WRITE);
    if (!f) return;

    f.write((const uint8_t *)txt, strlen(txt));
    f.close();
    free(txt);

    run_bjs_script_headless(PSRamFS, tmpfilepath);
    PSRamFS.remove(tmpfilepath);
}

void createInterpreterCommands(SimpleCLI *cli) {
    Command fileCmd = cli->addCommand("js_run_from_file", jsFileCallback);
    fileCmd.addPosArg("filepath");

    cli->addCommand("js_run_from_buffer", jsBufferCallback);
}
