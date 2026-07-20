#include "transport.hpp"

#include <iostream>

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
  std::optional<ResponseHandle*> responseHandle)
{
  this->command = command;
  this->priority = priority;
  this->responseHandle = responseHandle.value_or(new ResponseHandle());
}

bool PriorityCommand::operator<(const PriorityCommand &rhs) const
{
  return this->priority < rhs.priority;
}



int Transport::_executeRW(Transport* parent)
{
  if (!parent) return 1;
  parent->_exec_thread_running = true;
	
  while (parent->_exec_thread_running) {
    std::unique_lock<std::mutex> lock(parent->_command_mtx);
		parent->_command_cv.wait(lock, [parent]{
			return !parent->_command_queue.empty();
		});
    PriorityCommand next = parent->_command_queue.top();
    parent->_command_queue.pop();
    std::string command = next.command;
    if (command.size() < 2 || (command.substr(command.size()-2) != "\r\n"))
      command += "\r\n";
    ResponseHandle* rh = next.responseHandle;
    lock.unlock();
		parent->_command_cv.notify_all();
    
    parent->GenericTransport::_send(command);
    
    if (!parent->_exec_thread_running) continue;

    std::string response;
    while (parent->_exec_thread_running &&
      (response.size() < 1 || response[response.size()-1] != '>')) {
      std::string packet;
      int ec = parent->GenericTransport::_recv(packet);
      if (!ec) response += packet;
    }
    rh->write(response);
  }
  return 0;
}

int Transport::connect(std::string ip, std::string service) {
  int ret = GenericTransport::connect(ip, service);
  if (ret) return ret;
  ret = _send("\r\n");
  std::string leading;
  while (leading.size() < 1 || leading[leading.size()-1] != '>')
    GenericTransport::_recv(leading);
  _exec_thread = std::thread(_executeRW, this);
  _exec_thread->detach();
  return ret;
}

int Transport::addCommand(PriorityCommand pc)
{
	std::unique_lock<std::mutex> lock(_command_mtx);
	_command_cv.wait(lock, []{return true;});
	_command_queue.push(pc);
	lock.unlock();
	_command_cv.notify_all();
  return 0;
}

Transport::~Transport()
{
  _exec_thread_running = false;
  _command_queue = {};
  GenericTransport::disconnect();
}
