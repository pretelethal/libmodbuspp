/* Copyright © 2018-2019 Pascal JEAN, All rights reserved.
 * This file is part of the libmodbuspp Library.
 *
 * The libmodbuspp Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The libmodbuspp Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the libmodbuspp Library; if not, see <http://www.gnu.org/licenses/>.
 */
#ifdef _WIN32
# include <winsock2.h>
#else
# include <sys/socket.h>
#endif

#include <modbuspp/message.h>
#include "enclayer_p.h"
#include "config.h"

#if !defined(MSG_NOSIGNAL)
#define MSG_NOSIGNAL 0
#endif

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                         EncLayer Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  EncLayer::EncLayer (EncLayer::Private &dd) : NetLayer (dd) {}

  // ---------------------------------------------------------------------------
  EncLayer::EncLayer (const std::string & host, const std::string & service) :
    NetLayer (*new Private (host, service)) {}

  // ---------------------------------------------------------------------------
  const std::string & EncLayer::node() const {

    return connection();
  }

  // ---------------------------------------------------------------------------
  const std::string & EncLayer::service() const {

    return settings();
  }

  // ---------------------------------------------------------------------------
  int EncLayer::sendRawMessage (const Message * msg) {
    PIMP_D (EncLayer);

    return send (modbus_get_socket (d->ctx), msg->adu(), msg->aduSize(),
                 MSG_NOSIGNAL);
  }

  // ---------------------------------------------------------------------------
  bool EncLayer::prepareToSend (Message & msg) {

    if (msg.net() == Enc && msg.size() >= 1) {
      size_t aduSize = msg.aduSize();
      uint8_t * adu = msg.adu();

      uint16_t crc = RtuCrc::crc16 (adu, aduSize);
      adu[aduSize++] = crc >> 8;
      adu[aduSize++] = crc & 0xFF;
      msg.setAduSize (aduSize);
      return true;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  bool EncLayer::checkMessage (const Message & msg) {

    return RtuCrc::crc16 (msg.adu(), msg.aduSize() - 2) == msg.crc ();
  }

  // ---------------------------------------------------------------------------
  //
  //                         EncLayer::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  EncLayer::Private::Private (const std::string & host, const std::string & service) :
    NetLayer::Private (Enc, host, service, MODBUS_TCP_MAX_ADU_LENGTH),
    transactionId (1) {
    const char * node = NULL;

    if (host != "*") {
      node = host.c_str();
    }

    ctx = modbus_new_rtutcp_pi (node, service.c_str());
    if (! ctx) {

      throw std::invalid_argument (
        "Unable to create ENC RTU over TCP Modbus Backend(" +
        host + ":" + service + ")\n" + lastError());
    }
  }

}

/* ========================================================================== */
