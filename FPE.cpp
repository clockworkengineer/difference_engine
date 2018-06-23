#include "HOST.hpp"
/*
 * File: FPE.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on October 24, 2016, 2:34 PM
 *
 * Copyright 2016.
 *
 */

//
// Program: File Processing Engine
//
// Description: This is a generic file processing engine that sets up a
// watch folder and waits for files/directories to be copied to it. Any 
// added directories are also watched (ie.this is recursive) but any added files
// are be processed using one of its built in task action functions
// 
// 1) File copy
// 2) Video file conversion (using Handbrake)
// 3) Run shell command
// 4) Email file as a attachment ( or add file to mailbox for IMAP server)
// 5) File append to ZIP archive
// 
// All of this can be setup by using options  passed to the program from
// command line (FPE --help for a full list).
// 
// Dependencies:
// 
// C11++              : Use of C11++ features.
// Antikythera Classes: CTask, CSMTP, CIMAP, CIMAPParse, CZIP, CMIME, CLogger
// Linux              : Target platform
// Boost              : File system, program option.
//

// =============
// INCLUDE FILES
// =============

//
// Program components.
//

#include "FPE.hpp"
#include "FPE_ProcCmdLine.hpp"
#include "FPE_Actions.hpp"

//
// Antikythera Classes
//

#include "CRedirect.hpp"

//
// Boost file system library
//

#include <boost/filesystem.hpp>

// =========
// NAMESPACE
// =========

namespace FPE {

    // =======
    // IMPORTS
    // =======

    using namespace std;

    using namespace Antik::File;

    using namespace FPE_ProcCmdLine;
    using namespace FPE_Actions;

    // ===============
    // LOCAL FUNCTIONS
    // ===============

    //
    // Exit with error message/status
    //

    static void exitWithError(const string& errmsgStr) {

        // Closedown action functions, display error and exit.

        std::cerr << errmsgStr << std::endl;
        exit(EXIT_FAILURE);

    }


    //
    // Create task and run in thread.
    //

    static void createTaskAndRun(FPEOptions& optionData) {

        optionData.action->setActionData(optionData.optionsMap);
        
        // Create task object

        CTask task(optionData.optionsMap[kWatchOption], 
                   optionData.action, 
                   getOption<int>(optionData, kMaxDepthOption),
                   getOption<int>(optionData, kKillCountOption));

        // Create task object thread and start to watch else use FPE thread.

        if (getOption<bool>(optionData,kSingleOption)) {
            unique_ptr<thread> taskThread;
            taskThread.reset(new thread(&CTask::monitor, &task));
            taskThread->join();
        } else {
            task.monitor();
        }

        // If an exception occurred re-throw (end of chain)

        if (task.getThrownException()) {
            rethrow_exception(task.getThrownException());
        }

    }

    // ================
    // PUBLIC FUNCTIONS
    // ================

    void FileProcessingEngine(int argc, char** argv) {

        FPEOptions optionData; // Command line options  

        try {

            // cout to logfile if option specified.

            CRedirect logFile{cout};

            // Get FPE command line options.

            optionData = fetchCommandLineOptionData(argc, argv);

            // Display BOOST version

            std::cout << "*** boost version = [" << 
                to_string(BOOST_VERSION / 100000) << "." <<
                to_string(BOOST_VERSION / 100 % 1000) <<"." <<
                to_string(BOOST_VERSION % 100) << "] ***" << std::endl;

            // FPE up and running

            std::cout << "FPE Running..." << std::endl;

            // Output to log file ( CRedirect(cout) is the simplest solution). 
            // Once the try is exited CRedirect object will be destroyed and 
            // cout restored.

            if (!optionData.optionsMap[kLogOption].empty()) {
                logFile.change(optionData.optionsMap[kLogOption], ios_base::out | ios_base::app);
                std::cout << string(100, '=') << std::endl;
            }

            // Create task object

            createTaskAndRun(optionData);

        //
        // Catch any errors
        //    

        } catch (const boost::filesystem::filesystem_error & e) {
            exitWithError(e.what());
        } catch (const system_error &e) {
            exitWithError(e.what());
        } catch (const exception & e) {
            exitWithError(e.what());
        }

        std::cout << "FPE Exiting." << std::endl;


    }

} // namespace FPE

// ============================
// ===== MAIN ENTRY POINT =====
// ============================

int main(int argc, char** argv) {

    FPE::FileProcessingEngine(argc, argv);
    exit(EXIT_SUCCESS);

}
