#include "pch.h"
#include "GameLogic\Game.h"

using namespace LongHaulTestApp;
using namespace xbox::services;
using namespace xbox::services::system;

void Game::InitializeTestFramework()
{
    Platform::String^ localfolder = Windows::Storage::ApplicationData::Current->LocalFolder->Path;
    std::wstring localFolderW(localfolder->Begin());
    std::string localFolderA(localFolderW.begin(), localFolderW.end());
    m_logFileName = localFolderA + "\\long_haul_log.txt";

    Windows::System::Launcher::LaunchFolderAsync(Windows::Storage::ApplicationData::Current->LocalFolder);
    
    Game* pThis = this;
    auto settings = xbox_live_services_settings::get_singleton_instance(true);
    settings->set_diagnostics_trace_level(xbox_services_diagnostics_trace_level::off);
    settings->add_logging_handler([pThis](
        xbox_services_diagnostics_trace_level traceLevel,
        const std::string& category,
        const std::string& message
        ) 
    {
        pThis->Log("[" + pThis->TaceLevelToString(traceLevel) + "][" + category + "] " + message);
    });

    PrintMemoryUsage();
    Log("Starting Tests");
}

void Game::InitializeTests()
{

}

void Game::HandleTests()
{
    if (!m_testing && (time(NULL) - m_time) > m_testDelay)
    {
        BeginTest();
    }

    if (m_testing)
    {
        switch (m_testArea)
        {
            case TestArea::Achievements:
            case TestArea::Profile:
            case TestArea::Social:
                break;
        
            case TestArea::SocialManger: SocialManagerIntegrationUpdate(); break;
        }
    }
}

 void Game::BeginTest()
 {
     m_testing = true;

     switch (m_testArea)
     {
         case TestArea::Achievements: TestAchievementsFlow(); break;
         case TestArea::Profile: TestProfileFlow(); break;
         case TestArea::Social: TestSocialFlow(); break;
         case TestArea::SocialManger: TestSocialManagerFlow(); break;
     }
 }

void Game::EndTest()
{
    m_testing = false;

    int newTestArea = m_testArea + 1;
    if (newTestArea >= NUM_OF_TEST_AREAS)
    {
        m_testsRun++;
        m_time = time(NULL);
        newTestArea = 0;

        if (m_testsRun % 1 == 0)
        {
            PrintMemoryUsage();
        }
    }
    m_testArea = (TestArea)newTestArea;
}

void Game::Log(std::wstring log, bool showOnUI)
{
    std::lock_guard<std::mutex> guard(m_displayEventQueueLock);
    if (showOnUI)
    {
        m_displayEventQueue.push_back(log);
        if (m_displayEventQueue.size() > 30)
        {
            m_displayEventQueue.erase(m_displayEventQueue.begin());
        }
    }

    if (!m_logFileName.empty())
    {
        m_logFile.open(m_logFileName, std::ofstream::out | std::ofstream::app);
        m_logFile << utility::conversions::to_utf8string(log) << "\n";
        m_logFile.flush();
        m_logFile.close();
    }
}

void Game::Log(std::string log, bool showOnUI)
{
    Log(utility::conversions::to_utf16string(log), showOnUI);
}

void Game::PrintMemoryUsage()
{
    auto process = Windows::System::Diagnostics::ProcessDiagnosticInfo::GetForCurrentProcess();
    auto report = process->MemoryUsage->GetReport();
    if (!m_gotInitMemReport)
    {
        m_initMemReport = report;
        m_curMemReport = report;
        m_gotInitMemReport = true;
    }
    else
    {
        m_curMemReport = report;
        g_sampleInstance->m_lastDeltaMem = g_sampleInstance->m_curDeltaMem;
        g_sampleInstance->m_curDeltaMem = m_curMemReport->PeakVirtualMemorySizeInBytes - m_initMemReport->PeakVirtualMemorySizeInBytes;
    }

    stringstream_t stream;

    stream
        << "\n\n\n"
        << "=========================\n"
        << "===   Memory Ussage   ===\n"
        << "=========================\n\n\n"
        << "Tests Run: " << m_testsRun << "\n"
        << "NonPagedPoolSizeInBytes: " << report->NonPagedPoolSizeInBytes << "\n"
        << "PagedPoolSizeInBytes: " << report->PagedPoolSizeInBytes << "\n"
        << "PageFaultCount: " << report->PageFaultCount << "\n"
        << "PageFileSizeInBytes: " << report->PageFileSizeInBytes << "\n"
        << "PeakNonPagedPoolSizeInBytes: " << report->PeakNonPagedPoolSizeInBytes << "\n"
        << "PeakPagedPoolSizeInBytes: " << report->PeakPagedPoolSizeInBytes << "\n"
        << "PeakPageFileSizeInBytes: " << report->PeakPageFileSizeInBytes << "\n"
        << "PeakVirtualMemorySizeInBytes: " << report->PeakVirtualMemorySizeInBytes << "\n"
        << "PeakWorkingSetSizeInBytes: " << report->PeakWorkingSetSizeInBytes << "\n"
        << "PrivatePageCount: " << report->PrivatePageCount << "\n"
        << "VirtualMemorySizeInBytes: " << report->VirtualMemorySizeInBytes << "\n"
        << "WorkingSetSizeInBytes: " << report->WorkingSetSizeInBytes
        << "\n\n\n";

    Log(stream.str(), false);
}

string Game::TaceLevelToString(xbox_services_diagnostics_trace_level traceLevel)
{
    string level = "Unknown";

    switch (traceLevel)
    {
    case xbox_services_diagnostics_trace_level::error: level = "Error"; break;
    case xbox_services_diagnostics_trace_level::info: level = "Info"; break;
    case xbox_services_diagnostics_trace_level::verbose: level = "Verbose"; break;
    case xbox_services_diagnostics_trace_level::warning: level = "Warning"; break;
    }

    return level;
}