#pragma once

#include <string>

namespace sserver { namespace server {

    const static std::string SERVER_VERSION("1.0.0");
    
    const static int DEFAULT_DB_PORT = 5432;
    const static int DEFAULT_DB_CONN_TIMEOUT = 10;

	const static int MILLISECONDS_PER_SECOND = 1000;
	const static int MICROSECONDS_PER_SECOND = 1000 * 1000;

    // Notice: don't assign a value larger than 256 !!!!!
    // Because these roles will be used to generate different sets of session ids.
    enum USER_ROLE
    {
        USER_ADMIN                   = 1,
        USER_PHARMACIST              = 2,
        USER_PATIENT                 = 3
    };

	enum USER_STATUS
	{
		USER_STATUS_OFFLINE          = 1,
		USER_STATUS_IDLE             = 2,
		USER_STATUS_BUSY             = 3
	};

	enum MSG_DIRECTION
	{
		MSG_DIRECTION_PHARMACIST_TO_PATIENT = 1,
		MSG_DIRECTION_PATIENT_TO_PHARMACIST = 2
	};

} }