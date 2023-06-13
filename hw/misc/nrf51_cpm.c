/*
 * nRF51 CLOCK, POWER, and MPU emulation
 *
 * Reference Manual: http://infocenter.nordicsemi.com/pdf/nRF51_RM_v3.0.1.pdf
 *
 * Copyright 2023 Chris Laplante <chris@laplante.io>
 *
 * This code is licensed under the GPL version 2 or later.  See
 * the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "hw/qdev-properties.h"
#include "migration/vmstate.h"

#include "hw/arm/nrf51.h"
#include "hw/misc/nrf51_cpm.h"
#include "target/arm/arm-powerctl.h"

static uint64_t cpm_read(void *opaque, hwaddr offset, unsigned int size)
{
    NRF51CPMState *s = NRF51_CPM(opaque);
    uint64_t r = 0;

    switch (offset) {
    case NRF51_CLOCK_EVENT_HFCLKSTARTED:
        r = s->event_hfclkstarted;
        break;
    case NRF51_POWER_SYSTEMOFF:
        r = 1;
        break;
    }

    qemu_log("%s: read offset 0x%" HWADDR_PRIx "\n",
             __func__, offset);

    return r;
}

static void cpm_write(void *opaque, hwaddr offset, uint64_t value, unsigned int size)
{
    NRF51CPMState *s = NRF51_CPM(opaque);

    switch (offset) {
    case NRF51_CLOCK_TASK_HFCLKSTART:
        if (value == NRF51_TRIGGER_TASK) {
            int64_t timeout = qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
            timeout += 800;
            timer_mod(&s->hfclk_timer, timeout);
        }
        break;
    case NRF51_CLOCK_TASK_HFCLKSTOP:
        if (value == NRF51_TRIGGER_TASK) {
            if (timer_pending(&s->hfclk_timer)) {
                timer_del(&s->hfclk_timer);
            }

            s->event_hfclkstarted = 0;
        }
        break;
    case NRF51_CLOCK_EVENT_HFCLKSTARTED:
        if (value == NRF51_EVENT_CLEAR) {
            s->event_hfclkstarted = 0;
        }
        break;
    case NRF51_POWER_SYSTEMOFF:
        qemu_log(
                "%s: SYSTEMOFF!!!",
                __func__);
        if (value == 1) {
            arm_set_cpu_off(0);
        }
        break;
    default:
        break;

    }
    qemu_log(
            "%s: write offset 0x%" HWADDR_PRIx ", value = %lu \n",
            __func__, offset, value);
}

static const MemoryRegionOps cpm_ops = {
        .read =  cpm_read,
        .write = cpm_write,
        .endianness = DEVICE_LITTLE_ENDIAN,
        .impl.min_access_size = 4,
        .impl.max_access_size = 4
};

static void nrf51_cpm_hfclk_timer_expire(void *opaque)
{
    NRF51CPMState *s = NRF51_CPM(opaque);
    s->event_hfclkstarted = 1;
    timer_del(&s->hfclk_timer);
}

static void nrf51_cpm_init(Object *obj)
{
    NRF51CPMState *s = NRF51_CPM(obj);
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);

    memory_region_init_io(&s->mmio, obj, &cpm_ops, s,
                          TYPE_NRF51_CPM, NRF51_CPM_SIZE);
    sysbus_init_mmio(sbd, &s->mmio);

    timer_init_us(&s->hfclk_timer, QEMU_CLOCK_VIRTUAL, nrf51_cpm_hfclk_timer_expire, s);
}

static const VMStateDescription vmstate_cpm = {
        .name = "nrf51_soc.cpm",
        .version_id = 1,
        .minimum_version_id = 1,
        .fields = (VMStateField[]) {
//                VMSTATE_UINT32(active, NRF51RNGState),
//                VMSTATE_UINT32(event_valrdy, NRF51RNGState),
//                VMSTATE_UINT32(shortcut_stop_on_valrdy, NRF51RNGState),
//                VMSTATE_UINT32(interrupt_enabled, NRF51RNGState),
//                VMSTATE_UINT32(filter_enabled, NRF51RNGState),
                VMSTATE_END_OF_LIST()
        }
};

static void nrf51_cpm_reset(DeviceState *dev)
{
    NRF51CPMState *s = NRF51_CPM(dev);

    s->event_hfclkstarted = 0;

    timer_del(&s->hfclk_timer);
}

static void nrf51_cpm_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    //device_class_set_props(dc, nrf51_rng_properties);
    dc->vmsd = &vmstate_cpm;
    dc->reset = nrf51_cpm_reset;
}

static const TypeInfo nrf51_cpm_info = {
        .name = TYPE_NRF51_CPM,
        .parent = TYPE_SYS_BUS_DEVICE,
        .instance_size = sizeof(NRF51CPMState),
        .instance_init = nrf51_cpm_init,
        .class_init = nrf51_cpm_class_init
};

static void nrf51_cpm_register_types(void)
{
    type_register_static(&nrf51_cpm_info);
}

type_init(nrf51_cpm_register_types)
