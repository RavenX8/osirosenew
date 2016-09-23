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

#include "cloginserver.h"
#include "cloginclient.h"
#include "cloginisc.h"
#include "epackettype.h"
#include "config.h"
#include "database.h"

CLoginServer::CLoginServer(bool _isc) : CRoseServer(_isc), client_count_(0), server_count_(0) {
}

CLoginServer::~CLoginServer() { Shutdown(); }

void CLoginServer::OnAccepted(tcp::socket _sock) {
  if (_sock.is_open()) {
    // Do Something?
    std::string _address = _sock.remote_endpoint().address().to_string();
    if (IsISCServer() == false) {
      std::lock_guard<std::mutex> lock(client_list_mutex_);
      CLoginClient* nClient = new CLoginClient(std::move(_sock));
      nClient->SetId(client_count_++);
      nClient->SetLastUpdateTime( Core::Time::GetTickCount() );
      logger_->notice( "[{}] Client connected from: {}", nClient->GetId(),
                    _address.c_str());
      client_list_.push_front(nClient);
    } else {
      std::lock_guard<std::mutex> lock(isc_list_mutex_);
      CLoginISC* nClient = new CLoginISC(std::move(_sock));
      nClient->SetId(server_count_++);
      nClient->SetLastUpdateTime(Core::Time::GetTickCount());
      logger_->notice("Server connected from: {}", _address.c_str());
      isc_list_.push_front(nClient);
    }
  }
}
