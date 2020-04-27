#ifndef PACKETS_TS_SC_HUNTAHOLIC_UPDATE_SCORE_H
#define PACKETS_TS_SC_HUNTAHOLIC_UPDATE_SCORE_H

#include "Server/Packets/PacketDeclaration.h"

#define TS_SC_HUNTAHOLIC_UPDATE_SCORE_DEF(_) \
	_(simple)(int32_t, kill_count) \
	_(simple)(int32_t, score)

CREATE_PACKET(TS_SC_HUNTAHOLIC_UPDATE_SCORE, 4007);

#endif // PACKETS_TS_SC_HUNTAHOLIC_UPDATE_SCORE_H
