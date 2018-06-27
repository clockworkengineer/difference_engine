#include "HOST.hpp"
/*
 * File:   ImportCSVFile.cpp
 *
 * Author: Robert Tizzard
 * 
 * Created on October 24, 2016, 2:34 PM
 *
 * Copyright 2016.
 *
 */

//
// Module: ImportCSVFile
//
// Description: Take passed in CSV and import it into a MongoDB.
//
// Dependencies:
// 
// C11++              : Use of C11++ features.
// Linux              : Target platform
// Boost              : File system.
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
#include "FPE_Actions.hpp"

//
// Boost file system, format and tokenizer library
//

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>

//
// MongoDB C++ Driver
// Note: C++ Driver not easy to install so add define
//

#ifdef MONGO_DRIVER_INSTALLED
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#endif // MONGO_DRIVER_INSTALLED

namespace FPE_TaskActions {

    // =======
    // IMPORTS
    // =======

    using namespace std;

    using namespace FPE;

    namespace fs = boost::filesystem;

    // ===============
    // LOCAL VARIABLES
    // ===============

    // ===============
    // LOCAL FUNCTIONS
    // ===============

 
    // ================
    // PUBLIC FUNCTIONS
    // ================

    //
    // Import CSV File to MongoDB
    //

    bool ImportCSVFile::process(const string &file) {

        // ASSERT for any invalid options.

        assert(file.length() != 0);

        bool bSuccess = false;

        // Form source file path

#ifdef MONGO_DRIVER_INSTALLED

        fs::path sourceFile(file);

        ifstream csvFileStream(sourceFile.string());
        if (!csvFileStream.is_open()) {
            cout << "Error opening file " << sourceFile.string() << endl;
            return (false);
        }

        cout << "Importing CSV file [" << sourceFile.filename().string() << "] To MongoDB." << endl;

        mongocxx::instance driverInstance{};
        mongocxx::client mongoConnection{mongocxx::uri{this->m_actionData[kServerOption]}};
        auto csvCollection = mongoConnection[this->m_actionData[kDatabaseOption]][this->m_actionData[kCollectionOption]];
        vector<string> fieldNames;
        string csvLine;

        getline(csvFileStream, csvLine);
        if (csvLine.back() == '\r')csvLine.pop_back();

        fieldNames = getCSVTokens(csvLine);

        while (getline(csvFileStream, csvLine)) {
            vector< string > fieldValues;
            bsoncxx::builder::stream::document document{};
            if (csvLine.back() == '\r')csvLine.pop_back();
            fieldValues = getCSVTokens(csvLine);
            int i = 0;
            for (auto& field : fieldValues) {
                document << fieldNames[i++] << field;
            }
            csvCollection.insert_one(document.view());
        }
#endif // MONGO_DRIVER_INSTALLED

        return (bSuccess);

    }

} // namespace FPE_Actions