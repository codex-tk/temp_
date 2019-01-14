/**
 * @file sqlite3_connection.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-14
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __tlab_db_sqlite3_connection_h__
#define __tlab_db_sqlite3_connection_h__

#ifdef TLAB_ENABLE_SQLITE3_DB

#include <tlab/internal.hpp>
#include <sqlite3/sqlite3.h>

namespace tlab::db {

/**
 * @brief
 *
 */
class sqlite3_connection {
public:
    /**
     * @brief Construct a new sqlite3 connection object
     *
     */
    sqlite3_connection(void);

    /**
     * @brief Destroy the sqlite3 connection object
     *
     */
    ~sqlite3_connection(void);

    bool open(const std::string &conn_str, std::error_code &ec);

    void close(void);
private:
    sqlite3 *_sqlite3_ptr;
};

} // namespace tlab::db

#endif
#endif