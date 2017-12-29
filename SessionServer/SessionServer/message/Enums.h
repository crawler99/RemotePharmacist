#pragma once

#include "Poco/Foundation.h"

namespace sserver { namespace message { 

    const static Poco::UInt16 VERSION_NUM = 1;

    enum PKT_TYPE
    {
        // login process 
        PKT_TYPE_LOGIN                              = 1,
        PKT_TYPE_LOGIN_ACCEPT                          ,
        PKT_TYPE_LOGIN_REJECT                          ,
		PKT_TYPE_REFRESH_FINGERPRINT                   ,

        // pharmacist certificate management
        PKT_TYPE_ADD_PHARMACIST_CERT_TYPE              ,
        PKT_TYPE_ADD_PHARMACIST_CERT_TYPE_SUCC         ,
        PKT_TYPE_ADD_PHARMACIST_CERT_TYPE_FAIL         ,

        PKT_TYPE_LIST_PHARMACIST_CERT_TYPES            ,
        PKT_TYPE_PHARMACIST_CERT_TYPES                 ,

		// store management
		PKT_TYPE_ADD_STORE                             ,
		PKT_TYPE_ADD_STORE_SUCC                        ,
		PKT_TYPE_ADD_STORE_FAIL                        ,
		PKT_TYPE_DELETE_STORES                         ,
		PKT_TYPE_DELETE_STORES_SUCC                    ,
		PKT_TYPE_DELETE_STORES_FAIL                    ,

		PKT_TYPE_LIST_STORES                           ,
		PKT_TYPE_STORES                                ,

		// user management
		PKT_TYPE_PHARMACIST_ADD                        ,
		PKT_TYPE_PHARMACIST_ADD_SUCC                   ,
		PKT_TYPE_PHARMACIST_ADD_FAIL                   ,
        PKT_TYPE_DELETE_PHARMACISTS                    ,
        PKT_TYPE_DELETE_PHARMACISTS_SUCC               ,
        PKT_TYPE_DELETE_PHARMACISTS_FAIL               ,

		PKT_TYPE_PHARMACISTS_BRIEF_LIST                ,
		PKT_TYPE_PHARMACISTS_BRIEF                     ,

        PKT_TYPE_PHARMACISTS_DETAIL_LIST               ,
        PKT_TYPE_PHARMACISTS_DETAIL                    ,

		PKT_TYPE_PATIENT_ADD                           ,
		PKT_TYPE_PATIENT_ADD_SUCC                      ,
		PKT_TYPE_PATIENT_ADD_FAIL                      ,

        // patient query
        PKT_TYPE_GET_PHARMACISTS_DETAIL_OF_CUR_STORE   ,
        PKT_TYPE_PHARMACISTS_DETAIL_OF_CUR_STORE       ,

		// conversation
		PKT_TYPE_PHARMACIST_REQUEST                    ,
		PKT_TYPE_PHARMACIST_REQUEST_SUCC               ,
        PKT_TYPE_PHARMACIST_REQUEST_FAIL               ,

		PKT_TYPE_PATIENT_CONNECTED                     ,
		
		PKT_TYPE_STOP_CONVERSATION                     ,
		PKT_TYPE_PATIENT_QUIT_CONVERSATION             ,
		PKT_TYPE_PHARMACIST_QUIT_CONVERSATION          ,

		PKT_TYPE_DATA_TRANSFER_PRESCRIPTION            ,
		PKT_TYPE_DATA_TRANSFER_VIDEO                   ,
        PKT_TYPE_DATA_TRANSFER_AUDIO                   ,

        // deals of drug
        PKT_TYPE_ADD_DEAL_OF_PRESCRIPTION_DRUG         ,
        PKT_TYPE_ADD_DEAL_OF_PRESCRIPTION_DRUG_SUCC    ,
        PKT_TYPE_ADD_DEAL_OF_PRESCRIPTION_DRUG_FAIL    ,
        PKT_TYPE_LIST_DEALS_OF_PRESCRIPTION_DRUG       ,
        PKT_TYPE_DEALS_OF_PRESCRIPTION_DRUG_DETAIL     ,

        PKT_TYPE_ADD_DEAL_OF_SPECIAL_DRUG              ,
        PKT_TYPE_ADD_DEAL_OF_SPECIAL_DRUG_SUCC         ,
        PKT_TYPE_ADD_DEAL_OF_SPECIAL_DRUG_FAIL         ,
        PKT_TYPE_LIST_DEALS_OF_SPECIAL_DRUG            ,
        PKT_TYPE_DEALS_OF_SPECIAL_DRUG_DETAIL          ,

        // general queries
        PKT_TYPE_LIST_CONSULTING_DETAILS               ,
        PKT_TYPE_CONSULTING_DETAILS                    ,
		PKT_TYPE_LIST_PHARMACIST_ACTIVITY              ,
		PKT_TYPE_PHARMACIST_ACTIVITY                   ,

		// heartbeats
		PKT_TYPE_CLIENT_HEARTBEAT					   ,

        // logout process
        PKT_TYPE_LOGOUT                                ,

        // shut down server
        PKT_TYPE_SHUTDOWN_SERVER                       ,
    };

} }