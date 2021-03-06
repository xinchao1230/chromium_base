#ifndef CHILD_PROCESS_HOST_H_
#define CHILD_PROCESS_HOST_H_

#include <queue>
#include "../base/ipc/ipc_channel_proxy.h"
#include "../base/ipc/ipc_message.h"
#include "../base/id_map.h"
#include "../base/atomic_sequence_num.h"
#include "child_process_launcher.h"

class ChildLeonHost;

class ChildProcessHost : public IPC::Sender
                       , public IPC::Listener
                       , public ChildProcessLauncher::Observer {
public:
  explicit ChildProcessHost();
  virtual ~ChildProcessHost();

  bool CreateChildProcess();

  void ShutdownChildProcess();

  virtual int GetNextRoutingID();

  virtual bool Send(IPC::Message* msg) OVERRIDE;

  virtual bool OnMessageReceived(const IPC::Message& message) OVERRIDE;

  virtual void OnChannelConnected(int32 peer_pid) OVERRIDE;

  virtual void OnChannelError() OVERRIDE;

  virtual void OnProcessLaunched() OVERRIDE;

  virtual void Attach(ChildLeonHost* child_leon_host, int routing_id);

  virtual void Release(int routing_id);

  virtual ChildLeonHost* GetChildLeonHostByID(int routing_id);

  static int GenerateChildProcessUniqueId();

  static FilePath GetChildPath();

  static void RegisterHost(int host_id, ChildProcessHost* host);

  static void UnregisterHost(int host_id);

  static ChildProcessHost* FromID(int child_process_id);

  int GetID() const { return id_; }

  const std::string& channel_name() const { return channel_name_; }

  IDMap<ChildLeonHost>& ChildLeonHosts() { return child_leon_hosts_; }

  bool HasConnection() { return channel_.get() != NULL; }

  IPC::ChannelProxy* GetChannel() { return channel_.get(); }

  base::ProcessHandle GetHandle();

  virtual void Cleanup();

protected:
  scoped_ptr<IPC::ChannelProxy> channel_;

private:
  scoped_refptr<ChildProcessLauncher> child_process_launcher_;

  IDMap<ChildLeonHost> child_leon_hosts_;

  int id_;

  std::string channel_name_;

  bool delete_soon_;

  std::queue<IPC::Message*> queued_messages_;

  base::AtomicSequenceNumber next_routing_id_;

  DISALLOW_COPY_AND_ASSIGN(ChildProcessHost);
};
#endif