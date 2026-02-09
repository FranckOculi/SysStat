#include "serialize.h"
#include "system.h"

#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>

static uint64_t htonll(uint64_t val) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return ((uint64_t)htonl(val & 0xFFFFFFFF) << 32) | htonl(val >> 32);
#else
    return val;
#endif
}

static uint64_t ntohll(uint64_t val) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return ((uint64_t)ntohl(val & 0xFFFFFFFF) << 32) | ntohl(val >> 32);
#else
    return val;
#endif
}

// TODO: wip
void serialize_system_stats(struct system_stats *stats, uint8_t *buffer) {
    int offset = 0;

    /* cpu_stat */

    uint64_t *cpu_fields = (uint64_t *)&stats->cpu;
    for (int i = 0; i < CPU_FIELDS; i++) {
        uint64_t val = htonll(cpu_fields[i]);
        memcpy(buffer + offset, &val, sizeof(val));
        offset += 8;
    }

    /* mem_stat */

    uint64_t mem_total = htonll(stats->mem.mem_total);
    memcpy(buffer + offset, &mem_total, 8); offset += 8;

    uint64_t mem_available = htonll(stats->mem.mem_available);
    memcpy(buffer + offset, &mem_available, 8); offset += 8;

    /* uptime_stat */

    uint32_t uptime_hours = htonl(stats->uptime_hours);
    memcpy(buffer + offset, &uptime_hours, 4); offset += 4;

    uint32_t uptime_minutes = htonl(stats->uptime_minutes);
    memcpy(buffer + offset, &uptime_minutes, 4); offset += 4;
}


// TODO: wip
void deserialize_system_stats(struct system_stats *stats, const uint8_t *buffer) {
    int offset = 0;

    /* cpu_stat */
    
    uint64_t *cpu_fields = (uint64_t *)&stats->cpu;
    for (int i = 0; i < CPU_FIELDS; i++) {
        uint64_t val;
        memcpy(&val, buffer + offset, sizeof(val));
        cpu_fields[i] = ntohll(val);
        offset += 8;
    }

    /* mem_stat */

    uint64_t mem_total, mem_available;
    memcpy(&mem_total, buffer + offset, 8); offset += 8;
    memcpy(&mem_available, buffer + offset, 8); offset += 8;
    stats->mem.mem_total = ntohll(mem_total);
    stats->mem.mem_available = ntohll(mem_available);

    /* uptime_stat */

    uint32_t uptime_hours, uptime_minutes;
    memcpy(&uptime_hours, buffer + offset, 4); offset += 4;
    memcpy(&uptime_minutes, buffer + offset, 4); offset += 4;
    stats->uptime_hours = ntohl(uptime_hours);
    stats->uptime_minutes = ntohl(uptime_minutes);
}
