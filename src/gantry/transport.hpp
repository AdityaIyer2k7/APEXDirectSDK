#pragma once

#include "generictransport.hpp"

#include <optional>
#include <thread>
#include <mutex>
#include <queue>

namespace APEXDirectSDK::Gantry {
  class ResponseHandle {
    public:
      ResponseHandle() = default;
      bool isReady() const
      { return _ready; };
      int read(std::string& out);
      void write(std::string data);
    private:
      std::atomic<bool> _ready = false;
      std::mutex _rw_mtx;
      std::string _response;
  };

  typedef struct PriorityCommand {
    std::string command;
    int priority;
    ResponseHandle* responseHandle;
    PriorityCommand(std::string command, int priority, ResponseHandle* responseHandle = nullptr);
    bool operator<(const PriorityCommand &rhs) const;
  } PriorityCommand;
  
  class Transport : public GenericTransport {
    public:
      int connect(std::string ip, std::string service);
      int connect(std::string ip, int service)
      { return connect(ip, std::to_string(service)); }

      int addCommand(PriorityCommand pc);
      int addCommand(std::string command, int priority = 0)
      { return addCommand(PriorityCommand(command, priority)); }
      
      ~Transport();
    private:
      int recv(std::string& out) { return GenericTransport::_recv(out); }
      std::atomic<bool> _exec_thread_running = false;
      std::mutex _command_mtx;
      std::condition_variable _command_cv;
      std::priority_queue<PriorityCommand> _command_queue;
      int _executeRW();
      std::optional<std::thread> _exec_thread;
  };
}
