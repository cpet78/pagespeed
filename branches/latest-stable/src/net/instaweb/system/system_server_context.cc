// Copyright 2013 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: jefftk@google.com (Jeff Kaufman)

#include "net/instaweb/system/public/system_server_context.h"

#include "net/instaweb/system/public/system_rewrite_driver_factory.h"

namespace net_instaweb {

SystemServerContext::SystemServerContext(
    SystemRewriteDriverFactory* driver_factory)
    : ServerContext(driver_factory) {
}

}  // namespace net_instaweb