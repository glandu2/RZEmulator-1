#ifndef PACKETS_TS_CS_AUCTION_INSTANT_PURCHASE_H
#define PACKETS_TS_CS_AUCTION_INSTANT_PURCHASE_H

#include "Server/Packets/PacketDeclaration.h"

#define TS_CS_AUCTION_INSTANT_PURCHASE_DEF(_) \
	_(simple)(int32_t, auction_uid)

CREATE_PACKET(TS_CS_AUCTION_INSTANT_PURCHASE, 1308);

#endif // PACKETS_TS_CS_AUCTION_INSTANT_PURCHASE_H