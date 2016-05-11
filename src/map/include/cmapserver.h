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

#ifndef _CMAPSERVER_H_
#define _CMAPSERVER_H_

#include "croseserver.h"

class CMapServer : public RoseCommon::CRoseServer {
 public:
  CMapServer(bool _isc = false, int16_t mapidx = -1);
  virtual ~CMapServer();

  int32_t GetMapIDX() { return map_idx_; }

 protected:
  virtual void OnAccepted(tcp::socket _sock);

  enum class ServerType : int8_t { MASTER_NODE = -1, WORKER_THREAD };
  int32_t map_idx_;
  uint32_t client_count_;
  uint32_t server_count_;
};

#endif
