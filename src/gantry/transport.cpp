#include "transport.hpp"

#include <iostream>

#include "../errors.h"

using namespace APEXDirectSDK::Gantry;

int ResponseHandle::read(std::string& out) {
  if (!isReady()) return -127;
  std::unique_lock<std::mutex> lock(_rw_mtx);
	out = _response;
  return 0;
}

void ResponseHandle::write(std::string data) {
  std::unique_lock<std::mutex> lock(_rw_mtx);
  _response = data;
  _ready = true;
}



PriorityCommand::PriorityCommand(
  std::string command, int priority,
  std::optional<ResponseHandle*> responseHandle) {
  this->command = command;
  this->priority = priority;
  this->responseHandle = responseHandle.has_value() ? *responseHandle : nullptr;
}

bool PriorityCommand::operator<(const PriorityCommand &rhs) const {
  return this->priority < rhs.priority;
}



int Transport::_executeRW() {
  _exec_thread_running = true;
	
  while (_exec_thread_running) {
    std::unique_lock<std::mutex> lock(_command_mtx);
		_command_cv.wait(lock, [this]{
			return !this->_command_queue.empty();
		});
    PriorityCommand next = _command_queue.top();
    _command_queue.pop();
    std::string command = next.command;
    if (command.size() < 2 || (command.substr(command.size()-2) != "\r\n"))
      command += "\r\n";
    ResponseHandle* rh = next.responseHandle;
    lock.unlock();
		_command_cv.notify_all();
    
    GenericTransport::_send(command);
    
    if (!_exec_thread_running) continue;

    std::string response;
    GenericTransport::_recv_until(response, '>');
    if (rh) rh->write(response);
  }
  return 0;
}

int Transport::connect(std::string ip, std::string service) {
  int ret = GenericTransport::connect(ip, service);
  if (ret) return ret;
  ret = _send("\r\n");
  std::string leading;
  GenericTransport::_recv_until(leading, '>');
  _exec_thread = std::thread(&Transport::_executeRW, this);
  _exec_thread->detach();
  return ret;
}

int Transport::addCommand(PriorityCommand pc) {
	std::unique_lock<std::mutex> lock(_command_mtx);
	_command_cv.wait(lock, []{return true;});
	_command_queue.push(pc);
	lock.unlock();
	_command_cv.notify_all();
  return 0;
}

Transport::~Transport() {
  _exec_thread_running = false;
  _command_queue = {};
  GenericTransport::disconnect();
}
