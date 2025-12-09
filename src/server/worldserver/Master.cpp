

/** \file
    \ingroup Trinityd
*/

#include "Common.h"
#include "World.h"
#include "WorldSocket.h"
#include "WorldSocketMgr.h"
#include "Configuration/Config.h"
#include "Database/DatabaseEnv.h"
#include "Database/DatabaseWorkerPool.h"

#include "Log.h"
#include "Master.h"
#include "RASession.h"
#include "Timer.h"
#include "Util.h"
#include "ServerThreads.h"

#include "BigNumber.h"
#include "DatabaseLoader.h"
#include "GitRevision.h"
#include "MySQLThreading.h"
#include "Threading/BackgroundWorker.hpp"

#ifdef _WIN32
#include "ServiceWin32.h"
extern int m_ServiceStatus;
#endif

#ifdef __linux__
#include <sched.h>
#include <sys/resource.h>
#define PROCESS_HIGH_PRIORITY -15 // [-20, 19], default is 0
#endif

#include <boost/asio/signal_set.hpp>
#include <thread>
#include <chrono>

AsyncAcceptor* StartRaSocketAcceptor()
{
    uint16 raPort = uint16(sConfigMgr->GetIntDefault("Ra.Port", 3443));
    std::string raListener = sConfigMgr->GetStringDefault("Ra.IP", "0.0.0.0");

    AsyncAcceptor* acceptor = new AsyncAcceptor(raListener, raPort);
    if (!acceptor->Bind())
    {
        sLog->outError("Failed to bind RA socket acceptor");
        delete acceptor;
        return nullptr;
    }

    acceptor->AsyncAccept<RASession>();
    return acceptor;
}

/// Main function
int Master::Run()
{
    BigNumber seed1;
    seed1.SetRand(16 * 8);

    sLog->outString("%s (worldserver-daemon)", GitRevision::GetFullVersion());
    sLog->outString("<Ctrl-C> to stop.\n");

    /// worldserver PID file creation
    std::string pidFile = sConfigMgr->GetStringDefault("PidFile", "");
    if (!pidFile.empty())
    {
        if (uint32 pid = CreatePIDFile(pidFile))
            sLog->outString("Daemon PID: %u\n", pid);
        else
        {
            sLog->outString("Cannot create PID file %s.\n", pidFile.c_str());
            return 1;
        }
    }

    sBackgroundWorker->Initialize(sConfigMgr->GetIntDefault("BackgroundWorker.NumThreads", 4));
    sBackgroundWorker->Run([](){sLog->outBasic("Background Worker initialized");}, Threading::DetachedTask);

    ///- Start the databases
    if (!_StartDB())
        return 1;

    // set server offline (not connectable)
    LoginDatabase.DirectPExecute("UPDATE realmlist SET flag = (flag & ~%u) | %u WHERE id = '%d'", REALM_FLAG_OFFLINE, REALM_FLAG_INVALID, realmID);

    ///- Initialize the World
    sWorld->SetInitialWorldSettings();

    IoContext ioContext; // event loop that will be handled by main thread after completing init
    // Initialize the signal handlers
    boost::asio::signal_set signals(ioContext, SIGINT, SIGTERM);
#ifdef _WIN32
    signals.add(SIGBREAK);
#endif
    signals.async_wait([&](const boost::system::error_code& error, int signal)
            { if (error)
                  return;
              if (signal == SIGINT)
              {World::StopNow(RESTART_EXIT_CODE); ioContext.stop();}
#ifdef _WIN32
              else if (m_ServiceStatus != 1)
#else
              else
#endif
              {World::StopNow(SHUTDOWN_EXIT_CODE); ioContext.stop();}
            });


    ///- Launch WorldRunnable thread

    std::thread worldThread(&Sunwell::WorldThread);

    std::thread cliThread;

#ifdef _WIN32
    if (sConfigMgr->GetBoolDefault("Console.Enable", true) && (m_ServiceStatus == -1)/* need disable console in service mode*/)
#else
    if (sConfigMgr->GetBoolDefault("Console.Enable", true))
#endif
    {
        cliThread = std::thread(&Sunwell::CommandLineThread);
    }

	// pussywizard:
    std::thread auctionLising_thread(&Sunwell::AuctionListingThread);
    std::unique_ptr<AsyncAcceptor> raAcceptor(StartRaSocketAcceptor());

#if defined(_WIN32) || defined(__linux__)


    ///- Handle affinity for multiple processors and process priority
    uint32 affinity = sConfigMgr->GetIntDefault("UseProcessors", 0);
    bool highPriority = sConfigMgr->GetBoolDefault("ProcessPriority", false);

#ifdef _WIN32 // Windows

    HANDLE hProcess = GetCurrentProcess();

    if (affinity > 0)
    {
        ULONG_PTR appAff;
        ULONG_PTR sysAff;

        if (GetProcessAffinityMask(hProcess, &appAff, &sysAff))
        {
            ULONG_PTR currentAffinity = affinity & appAff;            // remove non accessible processors

            if (!currentAffinity)
                sLog->outError("Processors marked in UseProcessors bitmask (hex) %x are not accessible for the worldserver. Accessible processors bitmask (hex): %x", affinity, appAff);
            else if (SetProcessAffinityMask(hProcess, currentAffinity))
                sLog->outString("Using processors (bitmask, hex): %x", currentAffinity);
            else
                sLog->outError("Can't set used processors (hex): %x", currentAffinity);
        }
    }

    if (highPriority)
    {
        if (SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS))
            sLog->outString("worldserver process priority class set to HIGH");
        else
            sLog->outError("Can't set worldserver process priority class.");
    }

#else // Linux

    if (affinity > 0)
    {
        cpu_set_t mask;
        CPU_ZERO(&mask);

        for (unsigned int i = 0; i < sizeof(affinity) * 8; ++i)
            if (affinity & (1 << i))
                CPU_SET(i, &mask);

        if (sched_setaffinity(0, sizeof(mask), &mask))
            sLog->outError("Can't set used processors (hex): %x, error: %s", affinity, strerror(errno));
        else
        {
            CPU_ZERO(&mask);
            sched_getaffinity(0, sizeof(mask), &mask);
            sLog->outString("Using processors (bitmask, hex): %lx", *(__cpu_mask*)(&mask));
        }
    }

    if (highPriority)
    {
        if (setpriority(PRIO_PROCESS, 0, PROCESS_HIGH_PRIORITY))
            sLog->outError("Can't set worldserver process priority class, error: %s", strerror(errno));
        else
            sLog->outString("worldserver process priority class set to %i", getpriority(PRIO_PROCESS, 0));
    }

#endif
#endif

    // world thread is already running, but we may want to give it some extra time
    uint32 networkDelay = sConfigMgr->GetIntDefault("Network.DelayedStart", 0);
    std::this_thread::sleep_for( std::chrono::milliseconds( networkDelay ) );


    // Start up freeze catcher thread
    std::thread freezeThread;
    if (uint32 freezeDelay = sConfigMgr->GetIntDefault("MaxCoreStuckTime", 0))
    {
        freezeThread = std::thread(&Sunwell::FreezeDetectorThread, freezeDelay);
    }

    ///- Launch the world listener socket
    uint16 worldPort = uint16(sWorld->getIntConfig(CONFIG_PORT_WORLD));
    std::vector< uint16 > ports;
    ports.push_back( worldPort );

    std::string additionalPorts = sConfigMgr->GetStringDefault( "WorldServerPorts2", "" );
    if ( !additionalPorts.empty() )
    {
        Tokenizer tokens( additionalPorts, ';' );
        for ( auto && token : tokens )
        {
            ports.push_back( atoi( token ) );
        }
    }

    std::string bindIp = sConfigMgr->GetStringDefault("BindIP", "0.0.0.0");

    int num_threads = sConfigMgr->GetIntDefault("Network.Threads", 1);
    if (num_threads <= 0)
    {
        sLog->outError("Network.Threads is wrong in your config file");
        return 1;
    }

    if (!sWorldSocketMgr.StartNetwork(bindIp, worldPort, num_threads))
    {
        sLog->outError("Failed to start network");
        World::StopNow(ERROR_EXIT_CODE);
        // go down and shutdown the server
    }

    // set server online (allow connecting now)
    LoginDatabase.DirectPExecute("UPDATE realmlist SET flag = flag & ~%u, population = 0 WHERE id = '%u'", REALM_FLAG_INVALID, realmID);

    sLog->outString("%s (worldserver-daemon) ready...", GitRevision::GetFullVersion());
    while (!World::IsStopped())
        ioContext.run_for(std::chrono::milliseconds(100)); // run main event loop (wait for signals)
    // when the main thread closes the singletons get unloaded
    // since worldrunnable uses them, it will crash if unloaded after master
    worldThread.join();
    auctionLising_thread.join();
    sBackgroundWorker->Shutdown();

    sWorldSocketMgr.StopNetwork();

    if (freezeThread.joinable())
        freezeThread.join();

    // set server offline
    LoginDatabase.DirectPExecute("UPDATE realmlist SET flag = flag | %u WHERE id = '%d'", REALM_FLAG_OFFLINE, realmID);

    ///- Clean database before leaving
    ClearOnlineAccounts();

    _StopDB();

    sLog->outString("Halting process...");

    if (cliThread.joinable())
    {
        #ifdef _WIN32

        // this only way to terminate CLI thread exist at Win32 (alt. way exist only in Windows Vista API)
        //_exit(1);
        // send keyboard input to safely unblock the CLI thread
        INPUT_RECORD b[4];
        HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
        b[0].EventType = KEY_EVENT;
        b[0].Event.KeyEvent.bKeyDown = TRUE;
        b[0].Event.KeyEvent.uChar.AsciiChar = 'X';
        b[0].Event.KeyEvent.wVirtualKeyCode = 'X';
        b[0].Event.KeyEvent.wRepeatCount = 1;

        b[1].EventType = KEY_EVENT;
        b[1].Event.KeyEvent.bKeyDown = FALSE;
        b[1].Event.KeyEvent.uChar.AsciiChar = 'X';
        b[1].Event.KeyEvent.wVirtualKeyCode = 'X';
        b[1].Event.KeyEvent.wRepeatCount = 1;

        b[2].EventType = KEY_EVENT;
        b[2].Event.KeyEvent.bKeyDown = TRUE;
        b[2].Event.KeyEvent.dwControlKeyState = 0;
        b[2].Event.KeyEvent.uChar.AsciiChar = '\r';
        b[2].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        b[2].Event.KeyEvent.wRepeatCount = 1;
        b[2].Event.KeyEvent.wVirtualScanCode = 0x1c;

        b[3].EventType = KEY_EVENT;
        b[3].Event.KeyEvent.bKeyDown = FALSE;
        b[3].Event.KeyEvent.dwControlKeyState = 0;
        b[3].Event.KeyEvent.uChar.AsciiChar = '\r';
        b[3].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        b[3].Event.KeyEvent.wVirtualScanCode = 0x1c;
        b[3].Event.KeyEvent.wRepeatCount = 1;
        DWORD numb;
        WriteConsoleInput(hStdIn, b, 4, &numb);
        #endif
        cliThread.join();
    }

    // for some unknown reason, unloading scripts here and not in worldrunnable
    // fixes a memory leak related to detaching threads from the module
    //UnloadScriptingModule();

    // Exit the process with specified return value
    return World::GetExitCode();
}

template<typename DatabaseConnection>
bool StartDatabaseConnection(DatabaseWorkerPool<DatabaseConnection>& connection, std::string const& databaseName, uint8 const asyncThreads, uint8 const syncThreads, uint32 const maxPingTime)
{
    if (databaseName.empty())
    {
        sLog->outError("World database not specified in configuration file");
        return false;
    }

    if (asyncThreads == 0 || asyncThreads > 32)
    {
        sLog->outError("World database: invalid number of worker threads specified. "
                       "Please pick a value between 1 and 32.");
        return false;
    }

    if (!connection.Open(databaseName, asyncThreads, syncThreads, maxPingTime))
    {
        sLog->outError("Cannot connect to database %s", databaseName.c_str());
        return false;
    }

    return true;
}

/// Initialize connection to the databases
bool Master::_StartDB()
{
    MySQL::Library_Init();
    sLog->SetLogDB(false);

    // Load databases
    DatabaseLoader loader("server.worldserver", DatabaseType::None);
    loader
        .AddDatabase(LoginDatabase, "Login")
        .AddDatabase(CharacterDatabase, "Character")
        .AddDatabase(WorldDatabase, "World")
        .AddDatabase(LogDatabase, "Log")
        .AddDatabase(CustomDatabase, "Custom");

    if (!loader.Load())
        return false;

    ///- Get the realm Id from the configuration file
    realmID = sConfigMgr->GetIntDefault("RealmID", 0);
    if (!realmID /*|| realmID > 8*/) // pussywizard: above 8 spoils 8-bit online mask for the accounts
    {
        sLog->outError("Realm ID not defined in configuration file");
        return false;
    }
    sLog->outString("Realm running as realm ID %d", realmID);

    ///- Initialize the DB logging system
    sLog->SetRealmID(realmID);

    ///- Clean the database before starting
    ClearOnlineAccounts();

    ///- Insert version info into DB
    WorldDatabase.PExecute("UPDATE version SET core_version = '%s', core_revision = '%s'", GitRevision::GetFullVersion(), GitRevision::GetHash());        // One-time query

    sWorld->LoadDBVersion();

    sLog->outString("Using World DB: %s", sWorld->GetDBVersion());
    return true;
}

void Master::_StopDB()
{
    CharacterDatabase.Close();
    WorldDatabase.Close();
    LoginDatabase.Close();
    CustomDatabase.Close();
    LogDatabase.Close();

    MySQL::Library_End();
}

/// Clear 'online' status for all accounts with characters in this realm
void Master::ClearOnlineAccounts()
{
    // Reset online status for all accounts with characters on the current realm
    // pussywizard: tc query would set online=0 even if logged in on another realm >_>
    LoginDatabase.DirectExecute("UPDATE account SET online = 0 WHERE online <> 0");

    // Reset online status for all characters
    CharacterDatabase.DirectExecute("UPDATE characters SET online = 0 WHERE online <> 0");
}

