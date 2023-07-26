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
#define NRF51_CLOCK_TASK_LFCLKSTART       0x008
#define NRF51_CLOCK_TASK_LFCLKSTOP        0x00C
#define NRF51_CLOCK_TASK_CAL              0x010
#define NRF51_CLOCK_TASK_CALSTART         0x014
#define NRF51_CLOCK_TASK_CALSTOP          0x018


#define NRF51_CLOCK_EVENT_HFCLKSTARTED    0x100


#define NRF51_POWER_SYSTEMOFF             0x500

struct NRF51CPMState {
    SysBusDevice parent_obj;

    MemoryRegion mmio;

    QEMUTimer hfclk_timer;

    uint32_t event_hfclkstarted;


    uint8_t xtalfreq;

//    /* Time between generation of successive unfiltered values in us */
//    uint16_t period_unfiltered_us;
//    /* Time between generation of successive filtered values in us */
//    uint16_t period_filtered_us;
};


#endif /* NRF51_CPM_H */
