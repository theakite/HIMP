#ifndef ConfigFile_HPP
#define ConfigFile_HPP

#include <string>

using namespace std;

namespace Configuration {
    enum class config_version {
        v_0_1
    };

    class ConfigFile {

        private: 

            config_version versionNumber; //version of the config file
            string xmlPath; //currently test.xml
            string noDisplayText; //currently __false
            string topNodeTitle;

        public:

            ConfigFile(string fileName);
            config_version getVersionNumber();
            string getXmlPath();
            string getNoDisplayText();
            string getTopNodeTitle();

    };
}

#endif