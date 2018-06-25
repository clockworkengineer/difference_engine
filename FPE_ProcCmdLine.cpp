#include "HOST.hpp"
/*
 * File:   FPE_ProcCmdLine.cpp
 * 
 * Author: Robert Tizzard
 *
 * Created on October 24, 2016, 2:34 PM
 * 
 * Copyright 2016.
 * 
 */

//
// Module: FPE_ProcCmdLine
//
// Description: Command line option processing functionality.
// 
// Dependencies:
// 
// C11++        : Use of C11++ features.
// Linux        : Target platform
// Boost        : File system, program options.
//

// =============
// INCLUDE FILES
// =============

//
// C++ STL
//

#include <iostream>

//
// Program components.
//

#include "FPE.hpp"
#include "FPE_ProcCmdLine.hpp"

//
// Boost  file system library & program options processing
//

#include "boost/program_options.hpp" 
#include <boost/filesystem.hpp>

// =========
// NAMESPACE
// =========

namespace FPE_ProcCmdLine {

    // =======
    // IMPORTS
    // =======

    using namespace std;

    using namespace FPE;
    using namespace FPE_Actions;

    namespace po = boost::program_options;
    namespace fs = boost::filesystem;

    // ===============
    // LOCAL FUNCTIONS
    // ===============

    //
    // Add options common to both command line and config file
    //

    static void addCommonOptions(po::options_description& commonOptions, FPEOptions& optionData) {

        commonOptions.add_options()
                ("watch,w", po::value<string>(&optionData.optionsMap[kWatchOption])->required(), "Watch folder")
                ("destination,d", po::value<string>(&optionData.optionsMap[kDestinationOption]), "Destination folder")
                ("task,t", po::value<string>(&optionData.optionsMap[kTaskOption])->required(), "Task number")
                ("command", po::value<string>(&optionData.optionsMap[kCommandOption]), "Shell command to run")
                ("maxdepth", po::value<string>(&optionData.optionsMap[kMaxDepthOption])->default_value("-1"), "Maximum watch depth")
                ("extension,e", po::value<string>(&optionData.optionsMap[kExtensionOption]), "Override destination file extension")
                ("quiet,q", "Quiet mode (no trace output)")
                ("delete", "Delete source file")
                ("log,l", po::value<string>(&optionData.optionsMap[kLogOption]), "Log file")
                ("single,s", "Run task in main thread")
                ("killcount,k", po::value<string>(&optionData.optionsMap[kKillCountOption])->default_value("0"), "Files to process before closedown")
                ("server,s", po::value<string>(&optionData.optionsMap[kServerOption]), "SMTP/IMAP/MongoDB server URL and port")
                ("user,u", po::value<string>(&optionData.optionsMap[kUserOption]), "Account username")
                ("password,p", po::value<string>(&optionData.optionsMap[kPasswordOption]), "Account username password")
                ("recipient,r", po::value<string>(&optionData.optionsMap[kRecipientOption]), "Recipients(s) for email with attached file")
                ("mailbox,m", po::value<string>(&optionData.optionsMap[kMailBoxOption]), "IMAP Mailbox name for drop box")
                ("archive,a", po::value<string>(&optionData.optionsMap[kArchiveOption]), "ZIP destination archive")
                ("database,b", po::value<string>(&optionData.optionsMap[kDatabaseOption]), "Database name")
                ("collection,c", po::value<string>(&optionData.optionsMap[kCollectionOption]), "Collection/Table name")
                ("list", "Display a list of supported tasks.");
                

    }
    
    //
    // If a task option is not present throw an exception.
    //

    static void checkTaskOptions(const vector<string>& options, const po::variables_map& configVarMap) {

        for (auto opt : options) {
            if (!configVarMap.count(opt) || configVarMap[opt].as<string>().empty()) {
                throw po::error("Task option '" + opt + "' missing.");
            }
        }

    }
   
    //
    // If an option is not a valid int throw an exception.For the moment just try to convert to an
    // integer with stoi() (throws an error if the conversion fails). Note stoi() will convert up and to
    // the first non-numeric character so a string like "89ttt" will be converted to 89. 
    //
    
    static void checkIntegerOptions(const vector<string>& options, const po::variables_map& configVarMap) {

        for (auto opt : options) {
            if (configVarMap.count(opt)) {
                try {
                    stoi(configVarMap[opt].as<string>());
                } catch (const std::exception& e) {
                    throw po::error(opt + " is not a valid integer.");
                }
            }
        }

    }
     
    //
    // Display option name and its string value
    //

    static void displayOption(const string& nameStr, const string& valueStr) {
        if (!valueStr.empty()) {
            cout << "*** "  << nameStr << " = [" << valueStr << "] ***" << endl;
        }
    }
    
    //
    // Display description string if boolean option true
    //
    
    static void displayOption(bool bOption, const string& descStr) {
        if (bOption) {
            cout << "*** " << descStr << " ***" << endl;
        }
    }

    //
    // Process program option data and display run options. All options specified 
    // are displayed even though they may be ignored for the task (this is the 
    // simplest solution rather displaying dependant upon task).
    //

    static void processOptionData(FPEOptions& optionData) {
        
        // Make watch/destination paths absolute

        optionData.optionsMap[kWatchOption] = fs::absolute(optionData.optionsMap[kWatchOption]).lexically_normal().string();
        if (optionData.optionsMap[kWatchOption].back() == '.') optionData.optionsMap[kWatchOption].pop_back();
        if (!optionData.optionsMap[kDestinationOption].empty()) {
            optionData.optionsMap[kDestinationOption] = fs::absolute(optionData.optionsMap[kDestinationOption]).lexically_normal().string();
            if (optionData.optionsMap[kDestinationOption].back() == '.') optionData.optionsMap[kDestinationOption].pop_back();
        }
        
        // Display options

        displayOption(static_cast<string>(kConfigOption), optionData.optionsMap[kConfigOption]);
        displayOption(static_cast<string>(kTaskOption), optionData.action->getName());
        displayOption(static_cast<string>(kWatchOption), optionData.optionsMap[kWatchOption]);
        displayOption(static_cast<string>(kDestinationOption), optionData.optionsMap[kDestinationOption]);
        displayOption(static_cast<string>(kCommandOption), optionData.optionsMap[kCommandOption]);
        displayOption(static_cast<string>(kServerOption), optionData.optionsMap[kServerOption]);
        displayOption(static_cast<string>(kMailBoxOption), optionData.optionsMap[kMailBoxOption]);
        displayOption(static_cast<string>(kArchiveOption), optionData.optionsMap[kArchiveOption]);
        displayOption(static_cast<string>(kLogOption), optionData.optionsMap[kLogOption]);
        displayOption(static_cast<string>(kExtensionOption), optionData.optionsMap[kExtensionOption]);
        displayOption(static_cast<string>(kKillCountOption), optionData.optionsMap[kKillCountOption]);
        displayOption(static_cast<string>(kDatabaseOption), optionData.optionsMap[kDatabaseOption]);
        displayOption(static_cast<string>(kCollectionOption), optionData.optionsMap[kCollectionOption]);
        displayOption(getOption<bool>(optionData,kQuietOption), kQuietOption);
        displayOption(getOption<bool>(optionData,kDeleteOption), kDeleteOption);
        displayOption(getOption<bool>(optionData,kSingleOption),kSingleOption);
     
        // Create watch folder for task if necessary 

        if (!fs::exists(optionData.optionsMap[kWatchOption])) {
            fs::create_directories(optionData.optionsMap[kWatchOption]);
        }

        // Create destination folder for task if necessary 

        if (!optionData.optionsMap[kDestinationOption].empty() && !fs::exists(optionData.optionsMap[kDestinationOption])) {
            fs::create_directories(optionData.optionsMap[kDestinationOption]);
        }

    }

    // ================
    // PUBLIC FUNCTIONS
    // ================
    
    //
    // Read in and process command line options using boost.
    //

    FPEOptions fetchCommandLineOptionData(int argc, char** argv) {

        FPEOptions optionData{};

        // Define and parse the program options

        po::options_description commandLine("Command Line Options");

        // Command line (first unique then add those shared with config file

        commandLine.add_options()
                ("help", "Display help message")
                (kConfigOption, po::value<string>(&optionData.optionsMap[kConfigOption]), "Configuration file name");

        addCommonOptions(commandLine, optionData);

        // Config file options

        po::options_description configFile("Configuration File Options");

        addCommonOptions(configFile, optionData);

        po::variables_map configVarMap;

        try {

            // Process command line options

            po::store(po::parse_command_line(argc, argv, commandLine), configVarMap);

            // Display options and exit with success

            if (configVarMap.count("help")) {
                cout << "File Processing Engine Application" << endl << commandLine << endl;
                exit(EXIT_SUCCESS);
            }
            
            // Display list of available tasks
            
            if (configVarMap.count("list")) {
                cout << "File Processing Engine Application Tasks\n\n";
                int taskNo=0;
                std::shared_ptr<TaskAction> taskFunc;
                taskFunc = createTaskAction(taskNo);
                while (!taskFunc->getName().empty()){
                    cout << taskNo << "\t" << taskFunc->getName() << "\n";
                    taskFunc = createTaskAction(++taskNo);
                }
                exit(EXIT_SUCCESS);
            }

            // Load config file specified

            if (configVarMap.count(kConfigOption)) {
                if (fs::exists(configVarMap[kConfigOption].as<string>().c_str())) {
                    ifstream configFileStream{configVarMap[kConfigOption].as<string>().c_str()};
                    if (configFileStream) {
                        po::store(po::parse_config_file(configFileStream, configFile), configVarMap);
                    } else {
                        throw po::error("Error opening config file.");
                    }
                } else {
                    throw po::error("Specified config file [" + configVarMap[kConfigOption].as<string>() + "] does not exist.");
                }
            }
            
            // Check common integer options
            
            checkIntegerOptions({kTaskOption, kKillCountOption, kMaxDepthOption}, configVarMap);
                 
            // Task option validation. Options  valid to the task being
            // run are checked for and if not present an exception is thrown to
            // produce a relevant error message.Any extra options not required 
            // for a task are just ignored.

            if (configVarMap.count(kTaskOption)) {
                optionData.action = createTaskAction(stoi(configVarMap[kTaskOption].as<string>()));
                if (optionData.action) {
                    checkTaskOptions(optionData.action->getParameters(), configVarMap);
                } else {
                    throw po::error("Error invalid task number.");                 
                }
            }
  
            //
            // Set any boolean flags
            //
            
            // Delete source file

            if (configVarMap.count(kDeleteOption)) {
                optionData.optionsMap[kDeleteOption] = "1";  // true
            }

            // No trace output

            if (configVarMap.count(kQuietOption)) {
                optionData.optionsMap[kQuietOption] = "1"; // true
            }

            // Use main thread for task.

            if (configVarMap.count(kSingleOption)) {
                optionData.optionsMap[kSingleOption] = "1"; // true
            }

            po::notify(configVarMap);

        } catch (po::error& e) {
            cerr << "FPE Error: " << e.what() << endl;
            exit(EXIT_FAILURE);
        }

        // Process program option data

        processOptionData(optionData);

        return (optionData);

    }

} // FPE_ProcCmdLine
