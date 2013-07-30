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

#ifndef NET_INSTAWEB_SYSTEM_PUBLIC_SYSTEM_REWRITE_DRIVER_FACTORY_H_
#define NET_INSTAWEB_SYSTEM_PUBLIC_SYSTEM_REWRITE_DRIVER_FACTORY_H_

#include <set>

#include "net/instaweb/rewriter/public/rewrite_driver_factory.h"

#include "net/instaweb/util/public/basictypes.h"
#include "pagespeed/kernel/base/scoped_ptr.h"
#include "pagespeed/kernel/base/string.h"
#include "pagespeed/kernel/base/string_util.h"

namespace net_instaweb {

class AbstractSharedMem;
class NonceGenerator;
class ServerContext;
class SharedCircularBuffer;
class SharedMemStatistics;
class Statistics;
class SystemCaches;
class SystemRewriteOptions;
class SystemServerContext;
class ThreadSystem;

// A server context with features specific to a psol port on a unix system.
class SystemRewriteDriverFactory : public RewriteDriverFactory {
 public:
  // Takes ownership of thread_system.
  SystemRewriteDriverFactory(ThreadSystem* thread_system,
                             StringPiece hostname,
                             int port);
  virtual ~SystemRewriteDriverFactory();

  // Build global shared-memory statistics.  This is invoked if at least
  // one server context (global or VirtualHost) enables statistics.
  Statistics* MakeGlobalSharedMemStatistics(
      const SystemRewriteOptions& options);

  AbstractSharedMem* shared_mem_runtime() const {
    return shared_mem_runtime_.get();
  }

  // Creates and ::Initializes a shared memory statistics object.
  SharedMemStatistics* AllocateAndInitSharedMemStatistics(
      bool local, const StringPiece& name, const SystemRewriteOptions& options);

  // Hook for subclasses to init their stats and call
  // SystemRewriteDriverFactory::InitStats().
  virtual void NonStaticInitStats(Statistics* statistics) = 0;

  // Creates a HashedNonceGenerator initialized with data from /dev/random.
  NonceGenerator* DefaultNonceGenerator();

  // For shared memory resources the general setup we follow is to have the
  // first running process (aka the root) create the necessary segments and
  // fill in their shared data structures, while processes created to actually
  // handle requests attach to already existing shared data structures.
  //
  // During normal server startup[1], RootInit() must be called from the
  // root process and ChildInit() in every child process.
  //
  // Keep in mind, however, that when fork() is involved a process may
  // effectively see both calls, in which case the 'ChildInit' call would
  // come second and override the previous root status. Both calls are also
  // invoked in the debug single-process mode (In Apache, "httpd -X".).
  //
  // Note that these are not static methods -- they are invoked on every
  // SystemRewriteDriverFactory instance, which exist for the global
  // configuration as well as all the virtual hosts.
  //
  // Implementations should override RootInit and ChildInit for their setup.
  // See ApacheRewriteDriverFactory for an example.
  //
  // [1] Besides normal startup, Apache also uses a temporary process to
  // syntax check the config file. That basically looks like a complete
  // normal startup and shutdown to the code.
  bool is_root_process() const { return is_root_process_; }
  virtual void RootInit();
  virtual void ChildInit();

  // This helper method contains init procedures invoked by both RootInit()
  // and ChildInit()
  virtual void ParentOrChildInit();

  // Initialize SharedCircularBuffer and pass it to SystemMessageHandler and
  // SystemHtmlParseMessageHandler. is_root is true if this is invoked from
  // root (ie. parent) process.
  void SharedCircularBufferInit(bool is_root);

  // Hook for implementations to shut down their fetchers.
  virtual void ShutDownFetchers() {}

  // Hook so implementations may disable the property cache.
  virtual bool enable_property_cache() const {
    return true;
  }

  GoogleString hostname_identifier() { return hostname_identifier_; }

  // Release all the resources. It also calls the base class ShutDown to release
  // the base class resources.
  virtual void ShutDown();

  virtual void StopCacheActivity();

  SystemCaches* caches() { return caches_.get(); }

  virtual void set_message_buffer_size(int x) {
    message_buffer_size_ = x;
  }

 protected:
  // Initializes all the statistics objects created transitively by
  // SystemRewriteDriverFactory.  Only subclasses should call this.
  static void InitStats(Statistics* statistics);

  virtual void SetupCaches(ServerContext* server_context);

  // TODO(jefftk): create SystemMessageHandler and get rid of these hooks.
  virtual void SetupMessageHandlers() {}
  virtual void ShutDownMessageHandlers() {}
  virtual void SetCircularBuffer(SharedCircularBuffer* buffer) {}

  // Once ServerContexts are initialized via
  // RewriteDriverFactory::InitServerContext, they will be
  // managed by the RewriteDriverFactory.  But in the root process
  // the ServerContexts will never be initialized.  We track these here
  // so that SystemRewriteDriverFactory::ChildInit can iterate over all
  // the server contexts that need to be ChildInit'd, and so that we can free
  // them in the Root process that does not run ChildInit.
  typedef std::set<SystemServerContext*> SystemServerContextSet;
  SystemServerContextSet uninitialized_server_contexts_;

 private:
  scoped_ptr<SharedMemStatistics> shared_mem_statistics_;
  // While split statistics in the ServerContext cleans up the actual objects,
  // we do the segment cleanup for local stats here.
  StringVector local_shm_stats_segment_names_;
  scoped_ptr<AbstractSharedMem> shared_mem_runtime_;
  scoped_ptr<SharedCircularBuffer> shared_circular_buffer_;

  bool statistics_frozen_;
  bool is_root_process_;

  // hostname_identifier_ equals to "server_hostname:port" of the webserver.
  // It's used to distinguish the name of shared memory, so that each virtual
  // host has its own SharedCircularBuffer.
  const GoogleString hostname_identifier_;

  // Size of shared circular buffer for displaying Info messages in
  // /pagespeed_messages (or /mod_pagespeed_messages, /ngx_pagespeed_messages)
  int message_buffer_size_;

  // Manages all our caches & lock managers.
  scoped_ptr<SystemCaches> caches_;

  DISALLOW_COPY_AND_ASSIGN(SystemRewriteDriverFactory);
};

}  // namespace net_instaweb

#endif  // NET_INSTAWEB_SYSTEM_PUBLIC_SYSTEM_REWRITE_DRIVER_FACTORY_H_