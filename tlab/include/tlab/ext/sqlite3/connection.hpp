/**
 * @file connection.hpp
 * @author ghtak
 * @brief 
 * @version 0.1
 * @date 2019-01-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef __tlab_ext_sqlite3_connection_h__
#define __tlab_ext_sqlite3_connection_h__

#include <tlab/internal.hpp>
#include <sqlite3/sqlite3.h>

namespace tlab::ext::sqlite3{

/**
 * @brief 
 * 
 */
class connection{
public:
    /**
     * @brief Construct a new connection object
     * 
     */
    connection(void);

    /**
     * @brief Destroy the connection object
     * 
     */
    ~connection(void);

    /**
     * @brief 
     * 
     * @param info 
     * @return true 
     * @return false 
     */
    bool open(const std::string& info);

    bool open(const std::string& info , std::error_code& ec);

    /**
     * @brief 
     * 
     */
    void close(void);

    ::sqlite3* handle(void);
private:
    ::sqlite3* _sqlite3;
};


}

#endif