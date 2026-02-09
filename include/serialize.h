#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <stdint.h>
#include "system.h"

#define CPU_FIELDS 12
#define MEM_FIELDS 2
#define SYSTEM_STATS_BUFFER_SIZE ((CPU_FIELDS + MEM_FIELDS) * 8 + 2*4)

/* Server usage */
void serialize_system_stats(struct system_stats *stats, uint8_t *buffer);

/* Client usage */
void deserialize_system_stats(struct system_stats *stats, const uint8_t *buffer);

#endif