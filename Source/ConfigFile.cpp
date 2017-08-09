#include "./ConfigFile.hpp"
#include <fstream>
#include <iostream>

using namespace std;

namespace Configuration {

    // Constructor that populates properties based on the config file passed as input
    ConfigFile::ConfigFile(string fileName) {
        fstream infile(fileName);
        ofstream logFile("configLog.txt");

        string temp_versionNumber;
        getline(infile, temp_versionNumber);
        //TODO: convert this to the enum efficiently
        int lineEnd = temp_versionNumber.length();
        temp_versionNumber = temp_versionNumber.substr(9,lineEnd);

        string temp_xmlPath;
        getline(infile, temp_xmlPath);
        lineEnd = temp_xmlPath.length();
        this->xmlPath = temp_xmlPath.substr(10, lineEnd);
        logFile << "Path for the XML file is: " << this->xmlPath << endl;

        string temp_noDispText;
        getline(infile, temp_noDispText);
        lineEnd = temp_noDispText.length();
        this->noDisplayText = temp_noDispText.substr(17, lineEnd);
        logFile << "The no-display keyword is: " << this->noDisplayText << endl;

        string temp_getTopNode;
        getline(infile, temp_getTopNode);
        lineEnd = temp_getTopNode.length();
        this->topNodeTitle = temp_getTopNode.substr(25, lineEnd);
        logFile << "The top node title is: " << this->topNodeTitle << endl;

        string temp_getModuleTop;
        getline(infile, temp_getModuleTop);
        lineEnd = temp_getModuleTop.length();
        this->moduleTop = temp_getModuleTop.substr(18, lineEnd);
        logFile << "The top module node is: " << this->moduleTop << endl;

        string temp_getNumModules;
        getline(infile, temp_getNumModules);
        lineEnd = temp_getNumModules.length();
        temp_getNumModules = temp_getNumModules.substr(19, lineEnd);
        if (atoi(temp_getNumModules.c_str()) < 10 && atoi(temp_getNumModules.c_str()) > 0) {
            this->numModules = atoi(temp_getNumModules.c_str());
            //cout << atoi(temp_getNumModules.c_str()) << endl;
            logFile << "The number of modules is: " << to_string(this->numModules) << endl;
        }
        else {
            logFile << "Max module number is 9." << endl;
            cout << "Error: number of modules is out of bounds (must be between 1 and 9)" << endl;
        }

        string tempNodeName;
        string tempId;
        for (int i = 0; i < this->numModules; i++) {
            getline(infile, tempNodeName);
            tempNodeName = tempNodeName.substr(8, tempNodeName.length());
            getline(infile, tempId);
            tempId = tempId.substr(4, tempId.length());
            this->moduleList[i].node = tempNodeName;
            this->moduleList[i].id = tempId;
            //cout << "Node name " << i+1 << " is " << this->moduleList[i].node << endl;
            //cout << "Node id " << i+1 << " is " << this->moduleList[i].id << endl;
        }
    }

    config_version ConfigFile::getVersionNumber() {
        return this->versionNumber;
    }

    string ConfigFile::getXmlPath() {
        return this->xmlPath;
    }

    string ConfigFile::getNoDisplayText() {
        return this->noDisplayText;
    }

    string ConfigFile::getTopNodeTitle() {
        return this-> topNodeTitle;
    }

    int ConfigFile::getNumModules() {
        return this-> numModules;
    }

    string ConfigFile::getModuleTop() {
        return this-> moduleTop;
    }

    /*int main() { // for testing

        ConfigFile mainFile = ConfigFile("./HIMP.config");

        cout << mainFile.getXmlPath() << " is the XML path" << endl;
        cout << mainFile.getNoDisplayText() << " is the Non-Display text" << endl;

        return 0;
    }*/
}