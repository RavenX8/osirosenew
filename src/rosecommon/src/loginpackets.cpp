// Copyright 2016 Chirstopher Torres (Raven), L3nn0x
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "loginpackets.h"

RoseCommon::CliLoginReq::CliLoginReq(const std::string &user,
                                     const std::string &pass)
    : CRosePacket(ePacketType::PAKCS_LOGIN_REQ),
      password_(pass),
      username_(user) {}

RoseCommon::CliLoginReq::CliLoginReq(uint8_t buffer[MAX_PACKET_SIZE])
    : CRosePacket(buffer) {
  if (type() != ePacketType::PAKCS_LOGIN_REQ)
    throw std::runtime_error("Not the right packet!");
  char pass[32];
  *this >> pass >> username_;
  password_ = std::string(pass, 32);
}

RoseCommon::CliLoginReq::~CliLoginReq() {}

std::string RoseCommon::CliLoginReq::password() const { return password_; }

std::string RoseCommon::CliLoginReq::username() const { return username_; }

void RoseCommon::CliLoginReq::pack() {
  *this << password_.c_str() << username_;
}

RoseCommon::SrvLoginReply::info::info(const std::string &name, uint32_t id,
                                      bool isTest /*= false*/)
    : name_(name), channel_id_(id), test_(isTest) {}

RoseCommon::SrvLoginReply::SrvLoginReply(uint8_t result, uint16_t right,
                                         uint16_t type)
    : CRosePacket(ePacketType::PAKLC_LOGIN_REPLY),
      right_(right),
      type_(type),
      result_(result) {}

RoseCommon::SrvLoginReply::~SrvLoginReply() {}

uint8_t &RoseCommon::SrvLoginReply::result() { return result_; }

uint16_t &RoseCommon::SrvLoginReply::right() { return right_; }

uint16_t &RoseCommon::SrvLoginReply::type() { return type_; }

void RoseCommon::SrvLoginReply::setRight(uint16_t right) { right_ = right; }

void RoseCommon::SrvLoginReply::addServer(const std::string &name, uint32_t id,
                                          bool isTest /*= false*/) {
  info channel(name, id, isTest);
  channel_list_.push_back(channel);
}

void RoseCommon::SrvLoginReply::pack() {
  *this << result_ << right_ << type_;

  for (auto &server : channel_list_) {
    char pad = ' ';
    if (server.test_ == true) pad = '@';
    *this << pad << server.name_ << server.channel_id_;
  }
}

RoseCommon::CliChannelReq::CliChannelReq(uint32_t server_id)
    : CRosePacket(ePacketType::PAKCS_CHANNEL_LIST_REQ), server_id_(server_id) {}

RoseCommon::CliChannelReq::CliChannelReq(uint8_t buffer[MAX_PACKET_SIZE])
    : CRosePacket(buffer) {
  if (type() != ePacketType::PAKCS_CHANNEL_LIST_REQ)
    throw std::runtime_error("Not the right packet!");
  *this >> server_id_;
}

RoseCommon::CliChannelReq::~CliChannelReq() {}

uint32_t RoseCommon::CliChannelReq::server_id() const { return server_id_; }

void RoseCommon::CliChannelReq::pack() { *this << server_id_; }

RoseCommon::SrvChannelReply::info::info(const std::string &name, uint8_t id,
                                        uint16_t user_capacity_percentage,
                                        uint8_t low_age /*= 0*/,
                                        uint8_t high_age /*= 0*/)
    : name_(name),
      channel_id_(id),
      low_age_(low_age),
      high_age_(high_age),
      capacity_(user_capacity_percentage) {}

RoseCommon::SrvChannelReply::SrvChannelReply(uint32_t serverid)
    : CRosePacket(ePacketType::PAKLC_CHANNEL_LIST_REPLY),
      server_id_(serverid),
      channel_count_(0) {}

RoseCommon::SrvChannelReply::~SrvChannelReply() {}

uint8_t RoseCommon::SrvChannelReply::server_id() const { return server_id_; }

uint16_t RoseCommon::SrvChannelReply::channel_count() const {
  return channel_count_;
}

void RoseCommon::SrvChannelReply::addChannel(const std::string &name,
                                             uint8_t id,
                                             uint16_t user_capacity_percentage,
                                             uint8_t low_age /*= 0*/,
                                             uint8_t high_age /*= 0*/) {
  ++channel_count_;
  info channel(name, id, user_capacity_percentage, low_age, high_age);
  channel_list_.push_back(channel);
}

void RoseCommon::SrvChannelReply::pack() {
  *this << server_id_ << channel_count_;

  for (auto &server : channel_list_)
    *this << server.channel_id_ << server.low_age_ << server.high_age_
          << server.capacity_ << server.name_;
}

RoseCommon::CliServerSelectReq::CliServerSelectReq(uint32_t server_id,
                                                   uint8_t channel_id)
    : CRosePacket(ePacketType::PAKCS_SRV_SELECT_REQ),
      server_id_(server_id),
      channel_id_(channel_id) {}

RoseCommon::CliServerSelectReq::CliServerSelectReq(
    uint8_t buffer[MAX_PACKET_SIZE])
    : CRosePacket(buffer) {
  if (type() != ePacketType::PAKCS_SRV_SELECT_REQ)
    throw std::runtime_error("Not the right packet!");
  *this >> server_id_ >> channel_id_;
}

RoseCommon::CliServerSelectReq::~CliServerSelectReq() {}

uint32_t RoseCommon::CliServerSelectReq::server_id() const {
  return server_id_;
}

uint8_t RoseCommon::CliServerSelectReq::channel_id() const {
  return channel_id_;
}

void RoseCommon::CliServerSelectReq::pack() {
  *this << server_id_ << channel_id_;
}

RoseCommon::SrvServerSelectReply::SrvServerSelectReply(const std::string &ip,
                                                       uint32_t session_id,
                                                       uint32_t crypt_val,
                                                       uint16_t port)
    : CRosePacket(ePacketType::PAKLC_SRV_SELECT_REPLY),
      session_id_(session_id),
      crypt_val_(crypt_val),
      port_(port),
      result_(0),
      ip_(ip) {}

RoseCommon::SrvServerSelectReply::~SrvServerSelectReply() {}

uint32_t RoseCommon::SrvServerSelectReply::session_id() const {
  return session_id_;
}

uint32_t RoseCommon::SrvServerSelectReply::crypt_val() const {
  return crypt_val_;
}

uint16_t RoseCommon::SrvServerSelectReply::port() const { return port_; }

uint8_t RoseCommon::SrvServerSelectReply::result() const { return result_; }

std::string RoseCommon::SrvServerSelectReply::ip() const { return ip_; }

void RoseCommon::SrvServerSelectReply::pack() {
  *this << result_ << session_id_ << crypt_val_ << ip_ << port_;
}
