#pragma once

#include <string>
#include "libpq-fe.h"

namespace sserver { namespace server { 

	const static std::string DB_AGGREGATE_SEPARATOR = ",";

    class DBCmd
    {
    public:
        // return value of this function only reflects the execution status through the database's client API
        // for semantic check of the command result, you need to add extra functions in the sub-classes
        virtual bool execute(PGconn *pConn) = 0;

        // if user call this function then he/she should safely call execute again
        virtual void reset() = 0;
    };

} }