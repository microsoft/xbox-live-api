// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"jasonsa"
#define TEST_CLASS_AREA L"ServiceCallLogger"
#include "UnitTestIncludes.h"
#include "Utils.h"
#include "service_call_logger.h"
#include "service_call_logger_data.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

DEFINE_TEST_CLASS(ServiceCallLoggerTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(ServiceCallLoggerTests)

    bool LoggerFileExists(std::shared_ptr<xbox::services::service_call_logger> logger)
    {
        char buff[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buff);
        xsapi_internal_string filePath(buff);
        xsapi_internal_string fileName = logger->file_location();
        if (fileName.empty())
        {
            return false;
        }
        filePath += "\\" + fileName;
        std::ifstream stream(filePath.data());
        if (!stream)
        {
            return false;
        }
        return true;
    }

    void DeleteLoggerFiles() 
    {
        string_t prefix(_T("callHistoryJson-"));
        char_t buff[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, buff);
        string_t filePath(buff);

        Platform::String^ fileLocation = ref new Platform::String(filePath.c_str());
        Windows::Storage::StorageFolder^ appFolder = pplx::create_task(Windows::Storage::StorageFolder::GetFolderFromPathAsync(fileLocation)).get();

        // Get the items in the current folder; 
        IVectorView<Windows::Storage::IStorageItem^>^ itemsInFolder = pplx::create_task(appFolder->GetItemsAsync()).get();
        for (auto it = itemsInFolder->First(); it->HasCurrent; it->MoveNext())
        {
            Windows::Storage::IStorageItem^ item = it->Current;
            auto fileNameSubstr = STRING_T_FROM_PLATFORM_STRING(item->Name).substr(0, 16);
            if (item->IsOfType(Windows::Storage::StorageItemTypes::File) && utils::str_icmp(fileNameSubstr, prefix) == 0)
            {
                pplx::create_task(item->DeleteAsync()).get();
            }
        }
    }
    
    DEFINE_TEST_CASE(TestEnabledStates)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestEnabledStates);
        std::shared_ptr<xbox::services::service_call_logger> logger = xbox::services::service_call_logger::get_singleton_instance();
        VERIFY_IS_NOT_NULL(logger.get());
        VERIFY_NO_THROW(
            //enabling when enabled
            logger->enable();
            logger->enable();
            VERIFY_ARE_EQUAL(true, LoggerFileExists(logger));

            //enabling when disabled
            logger->disable();
            logger->enable();
            VERIFY_ARE_EQUAL(true, LoggerFileExists(logger));

            //disabling when enabled
            logger->disable();
            VERIFY_ARE_EQUAL(false, LoggerFileExists(logger));

            //disabling when disabled
            logger->disable();
            VERIFY_ARE_EQUAL(false, LoggerFileExists(logger));
            );
        DeleteLoggerFiles();
    }

    DEFINE_TEST_CASE(TestLogging)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestLogging);
        std::shared_ptr<xbox::services::service_call_logger> logger = xbox::services::service_call_logger::get_singleton_instance();
        VERIFY_IS_NOT_NULL(logger.get());

        //create mock data
        xbox::services::service_call_logger_data data(
            "fake.endpoint.com",
            "",
            "",
            false,
            0,
            "",
            "",
            "",
            "",
            std::chrono::milliseconds::zero(),
            chrono_clock_t::now(),
            "");

        //check logging when disabled
        logger->disable();
        logger->log(data.to_string());
        VERIFY_ARE_EQUAL(false, LoggerFileExists(logger));

        //check logging when enabled
        logger->enable();
        logger->log(data.to_string());
        VERIFY_ARE_EQUAL(true, LoggerFileExists(logger));
        logger->disable();
        
        DeleteLoggerFiles();
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

