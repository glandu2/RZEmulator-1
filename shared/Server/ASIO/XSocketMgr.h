#pragma once
/*
 * Copyright (C) 2017-2018 NGemity <https://ngemity.org/>
 * Copyright (C) 2008-2018 TrinityCore <https://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
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

/** \addtogroup u2w User to World Communication
 *  @{
 *  \file WorldSocketMgr.h
 *  \author Derex <derex101@gmail.com>
*/
#include "SocketMgr.h"
#include "XSocketThread.h"
#include "Config.h"
/// Manages all sockets connected to peers and network threads

template<class T>
static void OnSocketAccept(tcp::socket&& sock, uint32 threadIndex);

template<class T>
class XSocketMgr : public SocketMgr<XSocket>
{
        typedef SocketMgr<XSocket> BaseSocketMgr;

    public:
        ~XSocketMgr() = default;

        static XSocketMgr &Instance()
        {
            static XSocketMgr instance;
            return instance;
        }

        /// Start network, listen at address:port .
        bool StartWorldNetwork(NGemity::Asio::IoContext &ioContext, std::string const &bindIp, uint16 port, int threadCount)
        {
            _tcpNoDelay = sConfigMgr->GetBoolDefault("Network.TcpNodelay", true);

            int const max_connections = NGEMITY_MAX_LISTEN_CONNECTIONS;
            NG_LOG_DEBUG("misc", "Max allowed socket connections %d", max_connections);

            // -1 means use default
            _socketSystemSendBufferSize = sConfigMgr->GetIntDefault("Network.OutKBuff", -1);

            _socketApplicationSendBufferSize = sConfigMgr->GetIntDefault("Network.OutUBuff", 65536);

            if (_socketApplicationSendBufferSize <= 0)
            {
                NG_LOG_ERROR("misc", "Network.OutUBuff is wrong in your config file");
                return false;
            }

            if (!BaseSocketMgr::StartNetwork(ioContext, bindIp, port, threadCount))
                return false;

            _acceptor->SetSocketFactory(std::bind(&BaseSocketMgr::GetSocketForAccept, this));
            _acceptor->AsyncAcceptWithCallback<&OnSocketAccept<T>>();

            return true;
        }

/// Stops all network threads, It will wait for all running threads .
        void StopNetwork() override
        {
            BaseSocketMgr::StopNetwork();
        }

        void OnSocketOpen(tcp::socket &&sock, uint32 threadIndex) override
        {
            // set some options here
            if (_socketSystemSendBufferSize >= 0)
            {
                boost::system::error_code err;
                sock.set_option(boost::asio::socket_base::send_buffer_size(_socketSystemSendBufferSize), err);
                if (err && err != boost::system::errc::not_supported)
                {
                    NG_LOG_ERROR("misc", "WorldSocketMgr::OnSocketOpen sock.set_option(boost::asio::socket_base::send_buffer_size) err = %s", err.message().c_str());
                    return;
                }
            }

            // Set TCP_NODELAY.
            if (_tcpNoDelay)
            {
                boost::system::error_code err;
                sock.set_option(boost::asio::ip::tcp::no_delay(true), err);
                if (err)
                {
                    NG_LOG_ERROR("misc", "WorldSocketMgr::OnSocketOpen sock.set_option(boost::asio::ip::tcp::no_delay) err = %s", err.message().c_str());
                    return;
                }
            }

            //sock->m_OutBufferSize = static_cast<size_t> (m_SockOutUBuff);

            BaseSocketMgr::OnSocketOpen(std::forward<tcp::socket>(sock), threadIndex);
        }

        std::size_t GetApplicationSendBufferSize() const { return _socketApplicationSendBufferSize; }

    protected:
        XSocketMgr(): BaseSocketMgr(), _socketSystemSendBufferSize(-1), _socketApplicationSendBufferSize(65536), _tcpNoDelay(true)
        {
        }

        NetworkThread<XSocket> *CreateThreads() const override
        {
            return new XSocketThread<T>[GetNetworkThreadCount()];
        }

    private:
// private, must not be called directly
        bool StartNetwork(NGemity::Asio::IoContext &ioContext, std::string const &bindIp, uint16 port, int threadCount) override
        {
            return BaseSocketMgr::StartNetwork(ioContext, bindIp, port, threadCount);
        }

        int32 _socketSystemSendBufferSize;
        int32 _socketApplicationSendBufferSize;
        bool  _tcpNoDelay;
};

template<class T>
static void OnSocketAccept(tcp::socket&& sock, uint32 threadIndex)
{
    XSocketMgr<T>::Instance().OnSocketOpen(std::forward<tcp::socket>(sock), threadIndex);
}