/*
 * nRF51 CLOCK, POWER, and MPU emulation
 *
 * Copyright 2023 Chris Laplante <chris@laplante.io>
 *
 * This code is licensed under the GPL version 2 or later.  See
 * the COPYING file in the top-level directory.
 *
 */

#ifndef NRF51_CPM_H
#define NRF51_CPM_H

#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "qemu/timer.h"
#include "qom/object.h"
#define TYPE_NRF51_CPM "nrf51_soc.cpm"
OBJECT_DECLARE_SIMPLE_TYPE(NRF51CPMState, NRF51_CPM)

#define NRF51_CPM_SIZE                    0x1000
#define NRF51_CLOCK_TASK_HFCLKSTART       0x000
#define NRF51_CLOCK_TASK_HFCLKSTOP        0x004
#define NRF51_CLOCK_EVENT_HFCLKSTARTED    0x100

#define NRF51_POWER_SYSTEMOFF             0x500

struct NRF51CPMState {
    SysBusDevice parent_obj;

    MemoryRegion mmio;
//    qemu_irq irq;
//
//    /* Event End Points */
//    qemu_irq eep_valrdy;
//
    QEMUTimer hfclk_timer;

    uint32_t event_hfclkstarted;
//
//    /* Time between generation of successive unfiltered values in us */
//    uint16_t period_unfiltered_us;
//    /* Time between generation of successive filtered values in us */
//    uint16_t period_filtered_us;
//
//    uint8_t value;
//
//    uint32_t active;
//    uint32_t event_valrdy;
//    uint32_t shortcut_stop_on_valrdy;
//    uint32_t interrupt_enabled;
//    uint32_t filter_enabled;

};


#endif /* NRF51_CPM_H */