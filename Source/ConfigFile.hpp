#ifndef ConfigFile_HPP
#define ConfigFile_HPP

#include <string>
#include <iostream>

using namespace std;

namespace Configuration {
    enum class config_version {
        v_0_1
    };

    struct installedModule {
        string node;
        string id;
    };

    class ConfigFile {

        private: 

            config_version versionNumber; //version of the config file
            string xmlPath; //currently test.xml
            string noDisplayText; //currently __false
            string topNodeTitle; //currently paramaterFile
            int numModules;
            installedModule moduleList[10];
            string moduleTop;

        public:

            ConfigFile(string fileName);
            config_version getVersionNumber();
            string getXmlPath();
            string getNoDisplayText();
            string getTopNodeTitle();
            int getNumModules();
            string getModuleTop();
    };
}

#endif