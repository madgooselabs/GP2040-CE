#include "addons/midi_host.h"
#include "addons/midi_host_listener.h"
#include "storagemanager.h"
#include "drivermanager.h"
#include "usbhostmanager.h"
#include "peripheralmanager.h"
#include "class/hid/hid_host.h"

bool MIDIHostAddon::available() {
    stdio_init_all();
    //const KeyboardHostOptions& keyboardHostOptions = Storage::getInstance().getAddonOptions().keyboardHostOptions;
    //return keyboardHostOptions.enabled && PeripheralManager::getInstance().isUSBEnabled(0);
    return true;
}

void MIDIHostAddon::setup() {
    listener = new MIDIHostListener();
}

void MIDIHostAddon::preprocess() {
    ((MIDIHostListener*)listener)->process();
}
