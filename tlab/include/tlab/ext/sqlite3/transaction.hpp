/**
 * @file transaction.hpp
 * @author ghtak
 * @brief 
 * @version 0.1
 * @date 2019-01-19
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <tlab/ext/sqlite3/statement.hpp>

#ifndef __tlab_ext_sqlite3_transaction_h__
#define __tlab_ext_sqlite3_transaction_h__

namespace tlab::ext::sqlite3 {

class transaction{
public:
    transaction(tlab::ext::sqlite3::connection& conn) noexcept;

    bool begin(void){
        std::error_code ec;
        return begin(ec);
    }

    bool commit(void){
        std::error_code ec;
        return commit(ec);
    }

    bool rollback(void){
        std::error_code ec;
        return rollback(ec);
    }

    bool begin(std::error_code& ec);

    bool commit(std::error_code& ec);
    
    bool rollback(std::error_code& ec);
private:
    tlab::ext::sqlite3::statement _statement;
};

}

#endif