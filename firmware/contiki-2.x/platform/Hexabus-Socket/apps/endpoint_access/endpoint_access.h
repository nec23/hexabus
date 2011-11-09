#ifndef ENDPOINT_ACCESS_H_
#define ENDPOINT_ACCESS_H_

#include <stdint.h>
#include "../../../../../../shared/hexabus_packet.h"

uint8_t endpoint_get_datatype(uint8_t eid);
uint8_t endpoint_write(uint8_t eid, struct hxb_value* value);
void read(uint8_t eid, struct hxb_value* value);

#endif
