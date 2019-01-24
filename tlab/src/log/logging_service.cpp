/**
 * @file logging_service.cpp
 * @author ghtak
 * @brief 
 * @version 0.1
 * @date 2019-01-24
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <tlab/log/logging_service.hpp>

namespace tlab::log {

void logging_service::add_output(const std::shared_ptr<tlab::log::output>& optr){
    _outputs.push_back(optr);
}

} // namespace tlab::log
