#include "generictransport.hpp"

using namespace APEXDirectSDK::Gantry;

bool PriorityCommand::operator<(const PriorityCommand &rhs) const {
    return this->priority < rhs.priority;
}

bool ResponseHandle::isReady() const { return _ready; }

std::string ResponseHandle::read() {
    std::lock_guard<std::mutex> lock(_mtx);
    if (!_ready) return nullptr;
    return _response;
}

void ResponseHandle::write(std::string data) {
    std::lock_guard<std::mutex> lock(_mtx);
    _response = data;
    _ready = true;
}
