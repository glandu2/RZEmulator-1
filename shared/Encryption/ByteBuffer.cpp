/*
 * Copyright (C) 2011-2017 Project SkyFire <http://www.projectskyfire.org/>
 * Copyright (C) 2008-2017 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2017 MaNGOS <https://www.getmangos.eu/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ByteBuffer.h"

#include <sstream>

#include "Common.h"
#include "Log.h"

ByteBuffer::ByteBuffer(MessageBuffer &&buffer)
    : _rpos(0)
    , _wpos(0)
    , _curbitval(0)
    , _storage(buffer.Move())
{
}

ByteBufferPositionException::ByteBufferPositionException(bool /*add*/, size_t pos, size_t size, size_t valueSize)
{
    std::ostringstream ss;

    ss << "Attempted to get value with size: " << valueSize << " in ByteBuffer (pos: " << pos << " size: " << size << ")";

    message().assign(ss.str());
}

ByteBufferSourceException::ByteBufferSourceException(size_t pos, size_t size, size_t valueSize)
{
    std::ostringstream ss;

    ss << "Attempted to put a " << (valueSize > 0 ? "NULL-pointer" : "zero-sized value") << " in ByteBuffer (pos: " << pos << " size: " << size << ")\n\n";

    message().assign(ss.str());
}

void ByteBuffer::print_storage() const
{
    if (!sLog->ShouldLog("network",
            LOG_LEVEL_TRACE)) // optimize disabled trace output
        return;

    std::ostringstream o;
    o << "STORAGE_SIZE: " << size();
    for (uint32_t i = 0; i < size(); ++i)
        o << read<uint8_t>(i) << " - ";
    o << " ";

    NG_LOG_TRACE("network", "%s", o.str().c_str());
}

void ByteBuffer::textlike() const
{
    if (!sLog->ShouldLog("network",
            LOG_LEVEL_TRACE)) // optimize disabled trace output
        return;

    std::ostringstream o;
    o << "STORAGE_SIZE: " << size();
    for (uint32_t i = 0; i < size(); ++i)
    {
        char buf[1];
        snprintf(buf, 1, "%c", read<uint8_t>(i));
        o << buf;
    }
    o << " ";
    NG_LOG_DEBUG("network", "%s", o.str().c_str());
}

void ByteBuffer::hexlike() const
{
    if (!sLog->ShouldLog("network",
            LOG_LEVEL_TRACE)) // optimize disabled trace output
        return;

    uint32_t j = 1, k = 1;

    std::ostringstream o;
    o << "STORAGE_SIZE: " << size();

    for (uint32_t i = 0; i < size(); ++i)
    {
        char buf[3];
        snprintf(buf, 1, "%2X ", read<uint8_t>(i));
        if ((i == (j * 8)) && ((i != (k * 16))))
        {
            o << "| ";
            ++j;
        }
        else if (i == (k * 16))
        {
            o << "\n";
            ++k;
            ++j;
        }

        o << buf;
    }
    o << " ";
    NG_LOG_DEBUG("network", "%s", o.str().c_str());
}
