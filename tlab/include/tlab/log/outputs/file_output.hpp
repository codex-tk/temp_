/**
 * @file file_output.hpp
 * @author ghtak
 * @brief 
 * @version 0.1
 * @date 2019-01-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef __tlab_log_file_output_h__
#define __tlab_log_file_output_h__

#include <tlab/log/output.hpp>

#include <fstream>

namespace tlab::log {

class file_output : public tlab::log::output {
public:
    explicit file_output(const std::string& path,
                const int max_days = 30,
                const int max_lines = 0xffff);

    virtual void put(const char *ptr, const std::size_t sz,
                     const tlab::log::record &r) override;
private:
    void delete_old_logs(void);    
private:
    const std::string _path;
    const std::string _prefix;
    const int _max_lines;
    std::ofstream _ofstream;       
    int _lines;             
    int	_max_days;
    int _seq;
};

} // namespace tlab::log

#endif