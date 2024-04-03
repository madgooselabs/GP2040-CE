#include "addons/midi_host_listener.h"
#include "drivermanager.h"
#include "storagemanager.h"
#include "class/hid/hid_host.h"
#include "usb_midi_host.h"

static uint8_t midi_dev_addr = 0;

void MIDIHostListener::setup() {
    _midi_host_enabled = false;
}

void MIDIHostListener::process() {
    bool connected = midi_dev_addr != 0 && tuh_midi_configured(midi_dev_addr);
}

void MIDIHostListener::midi_mount(uint8_t dev_addr, uint8_t in_ep, uint8_t out_ep, uint8_t num_cables_rx, uint16_t num_cables_tx) {
    _midi_host_enabled = true;
    printf("MIDI device address = %u, IN endpoint %u has %u cables, OUT endpoint %u has %u cables\r\n", dev_addr, in_ep & 0xf, num_cables_rx, out_ep & 0xf, num_cables_tx);

    if (midi_dev_addr == 0) {
        // then no MIDI device is currently connected
        midi_dev_addr = dev_addr;
    } else {
        printf("A different USB MIDI Device is already connected.\r\nOnly one device at a time is supported in this program\r\nDevice is disabled\r\n");
    }
}

void MIDIHostListener::midi_umount(uint8_t dev_addr, uint8_t instance) {
    _midi_host_enabled = false;
    if (dev_addr == midi_dev_addr) {
        midi_dev_addr = 0;
        printf("MIDI device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
    } else {
        printf("Unused MIDI device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
    }
}

void MIDIHostListener::midi_rx(uint8_t dev_addr, uint32_t num_packets) {
    if ( _midi_host_enabled == false ) return; // do nothing if we haven't mounted

    if (midi_dev_addr == dev_addr) {
        if (num_packets != 0) {
            uint8_t cable_num;
            uint8_t buffer[48];
            while (1) {
                uint32_t bytes_read = tuh_midi_stream_read(dev_addr, &cable_num, buffer, sizeof(buffer));
                if (bytes_read == 0) return;

                if (buffer[0] >= 0x80) {
                    // status
                    uint8_t command = ((buffer[0] >> 4) & 0x0F);
                    uint8_t channel = ((buffer[0] >> 0) & 0x0F);
                    printf("MIDI RX Cable %u, Channel: %u - ");
                    switch (command) {
                        case 0x08:
                            printf("Note Off: %u ", buffer[1]);
                            printf("Velocity: %u", buffer[2]);
                            break;
                        case 0x09:
                            printf("Note On: %u ", buffer[1]);
                            printf("Velocity: %u", buffer[2]);
                            break;
                        case 0x0A:
                            printf("Pressure Note: %u ", buffer[1]);
                            printf("Level: %u", buffer[2]);
                            break;
                        case 0x0B:
                            printf("Change Controller: %u ", buffer[1]);
                            printf("Value: %u", buffer[2]);
                            break;
                        case 0x0C:
                            printf("Change Program: %u", buffer[1]);
                            break;
                        case 0x0D:
                            printf("Channel Pressure: %u", buffer[1]);
                            break;
                        case 0x0E:
                            printf("Pitch: %u", (buffer[2] << 8) | buffer[1]);
                            break;
                        case 0x0F:
                            printf("System: ");
                            for (uint32_t idx = 1; idx < bytes_read; idx++) {
                                printf("%02x ", buffer[idx]);
                            }
                            break;
                    }
                    printf("\n");
                }

                //printf("MIDI RX Cable #%u:", cable_num);
                //for (uint32_t idx = 0; idx < bytes_read; idx++) {
                //    printf("%02x ", buffer[idx]);
                //}
                //printf("\r\n");
            }
        }
    }
}

void MIDIHostListener::midi_tx(uint8_t dev_addr) {
    (void)dev_addr;
}
