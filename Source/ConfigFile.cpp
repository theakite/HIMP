#include "./ConfigFile.hpp"
#include <fstream>
#include <iostream>

using namespace std;

namespace Configuration {

    // Constructor that populates properties based on the config file passed as input
    ConfigFile::ConfigFile(string fileName) {
        fstream infile(fileName);

        string temp_versionNumber;
        getline(infile, temp_versionNumber);
        //TODO: convert this to the enum efficiently
        int lineEnd = temp_versionNumber.length();
        temp_versionNumber = temp_versionNumber.substr(9,lineEnd);

        string temp_xmlPath;
        getline(infile, temp_xmlPath);
        lineEnd = temp_xmlPath.length();
        this->xmlPath = temp_xmlPath.substr(10, lineEnd);
        clog << "Path for the XML file is: " << this->xmlPath << endl;

        string temp_noDispText;
        getline(infile, temp_noDispText);
        lineEnd = temp_noDispText.length();
        this->noDisplayText = temp_noDispText.substr(17, lineEnd);
        clog << "The no-display keyword is: " << this->noDisplayText << endl;

        string temp_getTopNode;
        getline(infile, temp_getTopNode);
        lineEnd = temp_getTopNode.length();
        this->topNodeTitle = temp_getTopNode.substr(25, lineEnd);
        clog << "The top node title is: " << this->topNodeTitle << endl;
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

    /*int main() { // for testing

        ConfigFile mainFile = ConfigFile("./HIMP.config");

        cout << mainFile.getXmlPath() << " is the XML path" << endl;
        cout << mainFile.getNoDisplayText() << " is the Non-Display text" << endl;

        return 0;
    }*/
}