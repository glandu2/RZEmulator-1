#ifndef PACKETS_TS_SC_DISCONNECT_DESC_H
#define PACKETS_TS_SC_DISCONNECT_DESC_H

#include "Packet/PacketDeclaration.h"

enum TS_SC_DISCONNECT_TYPE : uint8_t
{
	DISCONNECT_TYPE_ANOTHER_LOGIN = 0,
	DISCONNECT_TYPE_DUPLICATED_LOGIN = 1,
	DISCONNECT_TYPE_BILLING_EXPIRED = 2,
	DISCONNECT_TYPE_GAME_ADDICTION = 3,
	DISCONNECT_TYPE_DB_ERROR = 100,
	DISCONNECT_TYPE_ANTI_HACK = 101,
	DISCONNECT_TYPE_SCRIPT = 102
};


#define TS_SC_DISCONNECT_DESC_DEF(_) \
	_(simple)(TS_SC_DISCONNECT_TYPE, desc_id)

CREATE_PACKET(TS_SC_DISCONNECT_DESC, 28);

#endif // PACKETS_TS_SC_DISCONNECT_DESC_H