#ifndef _MIDIHostListener_H
#define _MIDIHostListener_H

#include "usblistener.h"
#include "gamepad.h"

class MIDIHostListener : public USBListener {
public:// USB Listener Features
    virtual void setup();
    virtual void mount(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len) {}
    virtual void xmount(uint8_t dev_addr, uint8_t instance, uint8_t controllerType, uint8_t subtype) {}
    virtual void unmount(uint8_t dev_addr) {}
    virtual void report_received(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) {}
    virtual void report_sent(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) {}
    virtual void set_report_complete(uint8_t dev_addr, uint8_t instance, uint8_t report_id, uint8_t report_type, uint16_t len) {}
    virtual void get_report_complete(uint8_t dev_addr, uint8_t instance, uint8_t report_id, uint8_t report_type, uint16_t len) {}
    virtual void midi_mount(uint8_t dev_addr, uint8_t in_ep, uint8_t out_ep, uint8_t num_cables_rx, uint16_t num_cables_tx);
    virtual void midi_umount(uint8_t dev_addr, uint8_t instance);
    virtual void midi_rx(uint8_t dev_addr, uint32_t num_packets);
    virtual void midi_tx(uint8_t dev_addr);
    void process();
private:
    bool _midi_host_enabled;
};

#endif  // _MIDIHostListener_H