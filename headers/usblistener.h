#ifndef _USBLISTENER_H_
#define _USBLISTENER_H_

#include <cstdint>

class USBListener
{
public:
    virtual void setup() = 0;
    virtual void mount(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len) = 0;
    virtual void xmount(uint8_t dev_addr, uint8_t instance, uint8_t controllerType, uint8_t subtype) = 0;
    virtual void unmount(uint8_t dev_addr) = 0;
    virtual void report_received(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) = 0;
    virtual void report_sent(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) = 0;
    virtual void set_report_complete(uint8_t dev_addr, uint8_t instance, uint8_t report_id, uint8_t report_type, uint16_t len) = 0;
    virtual void get_report_complete(uint8_t dev_addr, uint8_t instance, uint8_t report_id, uint8_t report_type, uint16_t len) = 0;
    virtual void midi_mount(uint8_t dev_addr, uint8_t in_ep, uint8_t out_ep, uint8_t num_cables_rx, uint16_t num_cables_tx) = 0;
    virtual void midi_umount(uint8_t dev_addr, uint8_t instance) = 0;
    virtual void midi_rx(uint8_t dev_addr, uint32_t num_packets) = 0;
    virtual void midi_tx(uint8_t dev_addr) = 0;
};

#endif
