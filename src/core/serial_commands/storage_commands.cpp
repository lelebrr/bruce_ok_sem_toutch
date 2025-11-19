#include "storage_commands.h"
#include "core/sd_functions.h"
#include "helpers.h"
#include <FS.h>
#include <LittleFS.h>
#include <globals.h>

void listCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs) || !(*fs).exists(filepath)) return;

    File root = fs->open(filepath);
    if (!root || !root.isDirectory()) return;

    File file = root.openNextFile();
    while (file) {
        Serial.print(file.name());
        if (file.isDirectory()) {
            Serial.println("\t<DIR>");
        } else {
            Serial.print("\t");
            Serial.println(file.size());
        }
        // Serial.println(file.path());
        // Serial.println(file.getLastWrite());  // TODO: parse to localtime
        file = root.openNextFile();
    }

    file.close();
    root.close();
}

void readCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.length() == 0) return;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs) || !(*fs).exists(filepath)) return;

    Serial.println(readSmallFile(*fs, filepath));
}

void md5Callback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.length() == 0) return;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs) || !(*fs).exists(filepath)) return;

    Serial.println(md5File(*fs, filepath));
}

void crc32Callback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.length() == 0) return;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs) || !(*fs).exists(filepath)) return;

    Serial.println(crc32File(*fs, filepath));
}

void removeCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.length() == 0) return;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs)) return;
    if (!(*fs).exists(filepath)) {
        Serial.println("File does not exist");
        return;
    }

    if ((*fs).remove(filepath)) {
        Serial.println("File removed");
    } else {
        Serial.println("Error removing file");
    }
}

void writeCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    Argument sizeArg = cmd.getArgument("size");
    String filepath = arg.getValue();
    String sizeStr = sizeArg.getValue();
    filepath.trim();
    int fileSize = sizeStr.toInt();

    if (filepath.length() == 0) return;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    if (fileSize < SAFE_STACK_BUFFER_SIZE) fileSize = SAFE_STACK_BUFFER_SIZE;

    FS *fs;
    if (!getFsStorage(fs)) return;

    char *txt = _readFileFromSerial(fileSize + 2);
    if (strlen(txt) == 0) return;

    File f = fs->open(filepath, FILE_WRITE, true);
    if (!f) return;

    f.write((const uint8_t *)txt, strlen(txt));
    f.close();
    free(txt);

    Serial.println("File written: " + filepath);
}

void renameCallback(cmd *c) {
    Command cmd(c);

    Argument filepathArg = cmd.getArgument("filepath");
    Argument newNameArg = cmd.getArgument("newName");
    String filepath = filepathArg.getValue();
    String newName = newNameArg.getValue();
    filepath.trim();
    newName.trim();

    if (filepath.length() == 0 || newName.length() == 0) return;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;
    if (!newName.startsWith("/")) newName = "/" + newName;

    FS *fs;
    if (!getFsStorage(fs)) return;
    if (!(*fs).exists(filepath)) {
        Serial.println("File does not exist");
        return;
    }

    if ((*fs).rename(filepath, newName)) {
        Serial.println("File renamed to '" + newName + "'");
    } else {
        Serial.println("Error renaming file");
    }
}

void copyCallback(cmd *c) {
    Command cmd(c);

    Argument filepathArg = cmd.getArgument("filepath");
    Argument newNameArg = cmd.getArgument("newName");
    String filepath = filepathArg.getValue();
    String newName = newNameArg.getValue();
    filepath.trim();
    newName.trim();

    if (filepath.length() == 0 || newName.length() == 0) return;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;
    if (!newName.startsWith("/")) newName = "/" + newName;

    FS *fs;
    if (!getFsStorage(fs)) return;
    if (!(*fs).exists(filepath)) {
        Serial.println("File does not exist");
        return;
    }

    fileToCopy = filepath;
    if (pasteFile((*fs), newName)) {
        Serial.println("File copied to '" + newName + "'");
    } else {
        Serial.println("Error copying file");
    }

    fileToCopy = "";
}

void mkdirCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.length() == 0) return;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs)) return;
    if ((*fs).exists(filepath)) {
        Serial.println("Directory already exists");
        return;
    }

    if ((*fs).mkdir(filepath)) {
        Serial.println("Directory created");
    } else {
        Serial.println("Error creating directory");
    }
}

void rmdirCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.length() == 0) return;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs)) return;
    if (!(*fs).exists(filepath)) {
        Serial.println("Directory does not exist");
        return;
    }

    if ((*fs).rmdir(filepath)) {
        Serial.println("Directory removed");
    } else {
        Serial.println("Error removing directory");
    }
}

void statCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.length() == 0) return;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs) || !(*fs).exists(filepath)) return;

    File file = fs->open(filepath);
    if (!file) return;

    Serial.println("File: " + filepath);

    Serial.print("Size: ");
    Serial.print(file.size());
    Serial.print("\t\t");
    if (file.isDirectory()) Serial.print("directory");
    else Serial.print("regular file");
    Serial.println("");

    Serial.print("Modify: ");
    Serial.print(file.getLastWrite()); // TODO: parse to localtime
    Serial.println("");

    file.close();
}

void freeStorageCallback(cmd *c) {
    Command cmd(c);
    Argument arg = cmd.getArgument("storage_type");

    if (arg.getValue() == "sd") {
        if (setupSdCard()) {
            uint64_t totalBytes = SD.totalBytes();
            uint64_t usedBytes = SD.usedBytes();
            uint64_t freeBytes = totalBytes - usedBytes;

            Serial.printf("SD Total space: %llu Bytes\n", totalBytes);
            Serial.printf("SD Used space: %llu Bytes\n", usedBytes);
            Serial.printf("SD Free space: %llu Bytes\n", freeBytes);
        } else {
            Serial.println("No SD card installed");
        }
    } else if (arg.getValue() == "littlefs") {
        Serial.println("LittleFS info not available in this build");
    } else {
        Serial.printf("Invalid arg %s\n", arg.getValue().c_str());
    }
}

void createListCommand(SimpleCLI *cli) {
    Command cmd = cli->addCmd("ls,dir", listCallback);
    cmd.addPosArg("filepath", "");
}

void createReadCommand(SimpleCLI *cli) {
    Command cmd = cli->addCmd("cat,type", readCallback);
    cmd.addPosArg("filepath");
}

void createMd5Command(SimpleCLI *cli) {
    Command cmd = cli->addCmd("md5", md5Callback);
    cmd.addPosArg("filepath");
}

void createCrc32Command(SimpleCLI *cli) {
    // storage crc32
    Command cmd = cli->addCmd("crc32", crc32Callback);
    cmd.addPosArg("filepath");
}

void createRemoveCommand(SimpleCLI *cli) {
    Command cmd = cli->addCmd("rm,del", removeCallback);
    cmd.addPosArg("filepath");
}

void createMkdirCommand(SimpleCLI *cli) {
    Command cmd = cli->addCmd("md,mkdir", mkdirCallback);
    cmd.addPosArg("filepath");
}

void createRmdirCommand(SimpleCLI *cli) {
    Command cmd = cli->addCmd("rmdir", rmdirCallback);
    cmd.addPosArg("filepath");
}

void createStorageCommand(SimpleCLI *cli) {
    createListCommand(cli);
    createReadCommand(cli);
    createRemoveCommand(cli);

    createMkdirCommand(cli);
    createRmdirCommand(cli);

    createMd5Command(cli);
    createCrc32Command(cli);

    // createStorageCommand(cli); // This would cause infinite recursion
}
