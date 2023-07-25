#include "qemu/osdep.h"
#include "monitor/hmp.h"
#include "monitor/monitor.h"

void hmp_rtt_start(Monitor *mon, const QDict *qdict)
{
    monitor_printf(mon, "whatever you say big boy\n");

}
