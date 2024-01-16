#include "maplelib.h"
#include "maple.h"
#include "menu.h"
#include "display.h"

MapleLib::MapleLib() {
//    memset(flashData, 0, sizeof(flashData));
//    memcpy(flashData, (uint8_t *)XIP_BASE + (FLASH_OFFSET * 9), sizeof(flashData)); // read into variable
//
//    // Pre-format VMU pages since rumble timer interrupt interferes with on-the-fly formatting
//    if (firstBoot || version != CURRENT_FW_VERSION) { // flash is 0xFF when erased! also run if FW version is different (post-update)
//        uint Interrupts = save_and_disable_interrupts();
//
//        for (int page = 1; page <= 8; page++) {
//            currentPage = page;
//
//            readFlash(); // includes checkFormatted
//
//            while (SectorDirty) {
//                uint Sector = 31 - __builtin_clz(SectorDirty);
//                SectorDirty &= ~(1 << Sector);
//                uint SectorOffset = Sector * FLASH_SECTOR_SIZE;
//
//                flash_range_erase((FLASH_OFFSET * currentPage) + SectorOffset, FLASH_SECTOR_SIZE);
//                flash_range_program((FLASH_OFFSET * currentPage) + SectorOffset, &MemoryCard[SectorOffset], FLASH_SECTOR_SIZE);
//            }
//        }
//        restore_interrupts(Interrupts);
//        currentPage = 1;
//
//        // Also set up some reasonable analog stick, trigger and flag defaults
//        xMin = 0x00;
//        xCenter = 0x80;
//        xMax = 0xff;
//        xDeadzone = 0x0f;
//        xAntiDeadzone = 0x04;
//        invertX = 0;
//
//        yMin = 0x00;
//        yCenter = 0x00;
//        yMax = 0xff;
//        yDeadzone = 0x0f;
//        yAntiDeadzone = 0x04;
//        invertY = 0;
//
//        lMin = 0x00;
//        lMax = 0xff;
//        lDeadzone = 0x00;
//        lAntiDeadzone = 0x04;
//        invertL = 0;
//
//        rMin = 0x00;
//        rMax = 0xff;
//        rDeadzone = 0x00;
//        rAntiDeadzone = 0x04;
//        invertR = 0;
//
//        oledFlip = 0;
//        swapXY = 0;
//        swapLR = 0;
//        autoResetEnable = 0;
//        autoResetTimer = 0x5A; // 180s
//        version = CURRENT_FW_VERSION;
//
//        firstBoot = 0; // first boot setup done
//
//        updateFlashData();    
//    }
//
//    splashSSD1306();
//
//#if ENABLE_RUMBLE
//    // PWM setup for rumble
//    gpio_init(15);
//    gpio_set_function(15, GPIO_FUNC_PWM);
//    gpio_disable_pulls(15);
//    gpio_set_drive_strength(15, GPIO_DRIVE_STRENGTH_12MA);
//    gpio_set_slew_rate(15, GPIO_SLEW_RATE_FAST);
//    uint slice_num = pwm_gpio_to_slice_num(15);
//
//    pwm_config config = pwm_get_default_config();
//    pwm_config_set_clkdiv(&config, 16.f);
//    pwm_init(slice_num, &config, true);
//    pwm_set_gpio_level(15, 0);
//
//    struct repeating_timer timer;
//    // negative interval means the callback function is called every 500us regardless of how long callback takes to execute
//    add_repeating_timer_us(-500, vibeHandler, NULL, &timer);
//#endif
//
//    // Page cycle interrupt
//    gpio_init(PAGE_BUTTON);
//    gpio_set_dir(PAGE_BUTTON, GPIO_IN);
//    gpio_pull_up(PAGE_BUTTON);
//    gpio_set_irq_enabled(PAGE_BUTTON, GPIO_IRQ_EDGE_FALL, true); 
//    gpio_add_raw_irq_handler(PAGE_BUTTON, pageToggle);
//    irq_set_enabled(IO_IRQ_BANK0, true); // enable all gpio interrupts (pagetoggle and input_act)
//
//    lastPress = to_ms_since_boot(get_absolute_time());
//
//    SetupButtons();
//
//    if (!gpio_get(ButtonInfos[3].InputIO) && !gpio_get(ButtonInfos[8].InputIO)) { // Y + Start
//        runMenu();
//        updateFlashData();
//        clearDisplay();
//        updateDisplay();
//    }
//
//    // Read current VMU into memory
//    if(vmuEnable) readFlash();
//
//    if(autoResetEnable){
//        gpio_set_irq_enabled(INPUT_ACT, GPIO_IRQ_EDGE_FALL, true);
//        gpio_add_raw_irq_handler(INPUT_ACT, softResetHandler);
//    }
//
//  // Start Core1 Maple RX
//  multicore_launch_core1(core1_entry);
//
//  // Controller packets
//  BuildInfoPacket();
//  BuildAllInfoPacket();
//  BuildControllerPacket();
//
//  // Subperipheral packets
//  BuildACKPacket();
//  BuildSubPeripheral0InfoPacket();
//  BuildSubPeripheral0AllInfoPacket();
//  BuildSubPeripheral1InfoPacket();
//  BuildSubPeripheral1AllInfoPacket();
//  BuildMemoryInfoPacket();
//  BuildLCDInfoPacket();
//  BuildPuruPuruInfoPacket();
//  BuildDataPacket();
//
//  SetupMapleTX();
//  SetupMapleRX();
//
//  uint StartOfPacket = 0;
//  while (true) {
//    uint EndOfPacket = multicore_fifo_pop_blocking();
//
//    // TODO: Improve. Would be nice not to move here
//    for (uint i = StartOfPacket; i < EndOfPacket; i += 4) {
//      *(uint *)&Packet[i - StartOfPacket] = __builtin_bswap32(*(uint *)&RecieveBuffer[i & (sizeof(RecieveBuffer) - 1)]);
//    }
//
//    uint PacketSize = EndOfPacket - StartOfPacket;
//    ConsumePacket(PacketSize);
//    StartOfPacket = ((EndOfPacket + 3) & ~3);
//
//    if (NextPacketSend != SEND_NOTHING) {
//#if SHOULD_SEND
//      if (!dma_channel_is_busy(TXDMAChannel)) {
//        switch (NextPacketSend) {
//        case SEND_CONTROLLER_INFO:
//          SendPacket((uint *)&InfoPacket, sizeof(InfoPacket) / sizeof(uint));
//          break;
//        case SEND_CONTROLLER_ALL_INFO:
//          SendPacket((uint *)&AllInfoPacket, sizeof(AllInfoPacket) / sizeof(uint));
//          break;
//        case SEND_CONTROLLER_STATUS:
//          if (VMUCycle) {
//            if (VMUCycleCount < 5) {
//              ControllerPacket.Header.Origin = ADDRESS_CONTROLLER;
//              VMUCycleCount++;
//            } else {
//              VMUCycleCount = 0;
//              VMUCycle = false;
//            }
//          } else {
//            ControllerPacket.Header.Origin = ADDRESS_CONTROLLER_AND_SUBS;
//          }
//          SendControllerStatus();
//
//          // Doing flash writes on controller status as likely got a frame
//          // until next message and unlikely to be in middle of doing rapid
//          // flash operations like a format or reading a large file. Ideally
//          // this would be asynchronous but doesn't seem possible :( We delay
//          // writes as flash reprogramming too slow to keep up with Dreamcast.
//          // Also has side benefit of amalgamating flash writes thus reducing
//          // wear.
//          if (SectorDirty && !multicore_fifo_rvalid() && MessagesSinceWrite >= FLASH_WRITE_DELAY) {
//            uint Sector = 31 - __builtin_clz(SectorDirty);
//            SectorDirty &= ~(1 << Sector);
//            uint SectorOffset = Sector * FLASH_SECTOR_SIZE;
//
//            uint Interrupts = save_and_disable_interrupts();
//            flash_range_erase((FLASH_OFFSET * currentPage) + SectorOffset, FLASH_SECTOR_SIZE);
//            flash_range_program((FLASH_OFFSET * currentPage) + SectorOffset, &MemoryCard[SectorOffset], FLASH_SECTOR_SIZE);
//            restore_interrupts(Interrupts);
//          } else if (!SectorDirty && MessagesSinceWrite >= FLASH_WRITE_DELAY && PageCycle) {
//            readFlash();
//            PageCycle = false;
//            VMUCycle = true;
//          } else if (MessagesSinceWrite < FLASH_WRITE_DELAY) {
//            MessagesSinceWrite++;
//          }
//          if (LCDUpdated) {
//            if (!oledType && endSplash){ // clear SSD1306 128x64 splashscreen
//              clearDisplay();
//              endSplash = false;
//            }
//
//            // thanks, gpt-4! :D
//            int x, y, pixel, bb;
//            for (int fb = 0; fb < LCDFramebufferSize; fb++) {
//                y = (fb / LCD_NumCols) * 2;
//                int mod = (fb % LCD_NumCols) * 16;
//                for (bb = 0; bb <= 7; bb++) {
//                    x = mod + (14 - bb * 2);
//                    pixel = ((LCDFramebuffer[fb] >> bb) & 0x01) * palette[currentPage - 1];
//                    if (LCD_Width == 48 && LCD_Height == 32) {
//                        setPixel(x, y, pixel);
//                        setPixel(x + 1, y, pixel);
//                        setPixel(x, y + 1, pixel);
//                        setPixel(x + 1, y + 1, pixel);
//                    }
//                }
//            }
//
//            updateDisplay();
//            LCDUpdated = false;
//          }
//          break;
//        case SEND_PURUPURU_STATUS:
//          SendPacket((uint *)&InfoPacket, sizeof(InfoPacket) / sizeof(uint));
//        case SEND_VMU_INFO:
//          SendPacket((uint *)&SubPeripheral0InfoPacket, sizeof(SubPeripheral0InfoPacket) / sizeof(uint));
//          break;
//        case SEND_VMU_ALL_INFO:
//          SendPacket((uint *)&SubPeripheral0AllInfoPacket, sizeof(SubPeripheral0AllInfoPacket) / sizeof(uint));
//          break;
//        case SEND_PURUPURU_INFO:
//          SendPacket((uint *)&SubPeripheral1InfoPacket, sizeof(SubPeripheral1InfoPacket) / sizeof(uint));
//          break;
//        case SEND_PURUPURU_ALL_INFO:
//          SendPacket((uint *)&SubPeripheral1AllInfoPacket, sizeof(SubPeripheral1AllInfoPacket) / sizeof(uint));
//          break;
//        case SEND_PURUPURU_MEDIA_INFO:
//          SendPacket((uint *)&PuruPuruInfoPacket, sizeof(PuruPuruInfoPacket) / sizeof(uint));
//          break;
//        case SEND_MEMORY_INFO:
//          SendPacket((uint *)&MemoryInfoPacket, sizeof(MemoryInfoPacket) / sizeof(uint));
//          break;
//        case SEND_LCD_INFO:
//          SendPacket((uint *)&LCDInfoPacket, sizeof(LCDInfoPacket) / sizeof(uint));
//          break;
//        case SEND_ACK:
//          SendPacket((uint *)&ACKPacket, sizeof(ACKPacket) / sizeof(uint));
//          break;
//        case SEND_DATA:
//          SendBlockReadResponsePacket(FUNC_MEMORY_CARD);
//          break;
//        case SEND_PURUPURU_DATA:
//          SendBlockReadResponsePacket(FUNC_VIBRATION);
//          break;
//        case SEND_PURUPURU_CONDITION:
//          SendPacket((uint *)&PuruPuruConditionPacket, sizeof(PuruPuruConditionPacket) / sizeof(uint));
//          break;
//        case SEND_TIMER_CONDITION:
//          SendPacket((uint *)&TimerConditionPacket, sizeof(TimerConditionPacket) / sizeof(uint));
//          break;
//        case SEND_TIMER_DATA:
//          SendBlockReadResponsePacket(FUNC_TIMER);
//          break;
//        }
//      }
//#endif
//      NextPacketSend = SEND_NOTHING;
//    }
//  }
}