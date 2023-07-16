/*
 * Tempur-Pedic RF502A remote control
 *
 * Copyright 2023 Chris Laplante <chris@laplante.io>
 *
 * This code is licensed under the GPL version 2 or later.  See
 * the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/boards.h"
#include "hw/arm/boot.h"
#include "sysemu/sysemu.h"
#include "exec/address-spaces.h"

#include "hw/arm/nrf51_soc.h"
#include "hw/qdev-properties.h"
#include "qom/object.h"
#include "qemu/log-for-trace.h"

struct RF502AMiscGPIOState {
    SysBusDevice parent_obj;

    qemu_irq test_irq;
};

#define TYPE_RF502A_MISC_GPIO "rf502a-misc-gpio"
OBJECT_DECLARE_SIMPLE_TYPE(RF502AMiscGPIOState, RF502A_MISC_GPIO)

struct RF502AMachineState {
    MachineState parent;

    NRF51State nrf51;
    RF502AMiscGPIOState misc_gpio;
};

#define TYPE_RF502A_MACHINE MACHINE_TYPE_NAME("rf502a")
OBJECT_DECLARE_SIMPLE_TYPE(RF502AMachineState, RF502A_MACHINE)

static void rf502a_init(MachineState *machine)
{
    RF502AMachineState *s = RF502A_MACHINE(machine);
    MemoryRegion *system_memory = get_system_memory();

    object_initialize_child(OBJECT(machine), "nrf51", &s->nrf51,
                            TYPE_NRF51_SOC);
    qdev_prop_set_chr(DEVICE(&s->nrf51), "serial0", serial_hd(0));
    object_property_set_link(OBJECT(&s->nrf51), "memory",
                             OBJECT(system_memory), &error_fatal);
    sysbus_realize(SYS_BUS_DEVICE(&s->nrf51), &error_fatal);

    object_initialize_child(OBJECT(machine), "misc-gpio", &s->misc_gpio, TYPE_RF502A_MISC_GPIO);
    sysbus_realize(SYS_BUS_DEVICE(&s->misc_gpio), &error_fatal);

    qdev_connect_gpio_out_named(DEVICE(&s->misc_gpio), "pin8", 0, qdev_get_gpio_in(DEVICE(&s->nrf51), 8));

    armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename,
                       0, s->nrf51.flash_size);
}

static void rf502a_machine_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "Tempur-Pedic RF502A remote control (Cortex-M0)";
    mc->init = rf502a_init;
    mc->max_cpus = 1;
}

static void rf502a_misc_gpio_init(Object *obj)
{
    RF502AMiscGPIOState *s = RF502A_MISC_GPIO(obj);
    DeviceState *dev = DEVICE(obj);

    qdev_init_gpio_out_named(dev, &s->test_irq, "pin8", 1);
}

static const TypeInfo rf502a_misc_gpio_info = {
    .name = TYPE_RF502A_MISC_GPIO,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RF502AMiscGPIOState),
    .instance_init = rf502a_misc_gpio_init,
};

static const TypeInfo rf502a_info = {
        .name = TYPE_RF502A_MACHINE,
        .parent = TYPE_MACHINE,
        .instance_size = sizeof(RF502AMachineState),
        .class_init = rf502a_machine_class_init,
};

static void rf502a_machine_init(void)
{
    type_register_static(&rf502a_info);
    type_register_static(&rf502a_misc_gpio_info);
}

type_init(rf502a_machine_init);
