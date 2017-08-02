#include "./HIMP.hpp"
#include "./ConfigFile.hpp"

using namespace rapidxml;
using namespace std;
using namespace Configuration;

int countChildren(xml_node<> parentNode);

int main() {

    ConfigFile config = ConfigFile("./HIMP.config");

    ofstream logFile("logFile.txt");

    file<> xmlFile(config.getXmlPath().c_str());//OPTI: maybe start as a cstring instead of having to convert to it?
    logFile << "XML Loaded" << endl;

    xml_document<> doc;
    doc.parse<0>(xmlFile.data());
    logFile << "XML Parsed" << endl;

    xml_node<> *topNode=doc.first_node(0);
    int topChildrenCount = count_children(topNode);

    //TODO: have something for version checking ** handle in config file
    
    if (!strcmp(topNode->name(), config.getTopNodeTitle().c_str())) { //OPTI: cstring conversion can't be the best way to do this
        logFile << "Top Node Loaded at " << topNode << " and has " << topChildrenCount << " children" << endl;
    }

    else {
        logFile << "Unexpected top node: " << topNode->name() <<  endl;
        cerr << "Unexpected top node: " << topNode->name() <<  endl;

        return 0; //TODO: handle this quit more gracefully
    }

    xml_node<> *topChildren[topChildrenCount];
    topChildren[0] = topNode->first_node(0);
    for (int i = 1; i < topChildrenCount; i++) {
        topChildren[i] = topChildren[i-1]->next_sibling(0);
    }

    for (int i = 0; i < topChildrenCount; i++) {
        cout << "Child " << i << " is " << topChildren[i]->name() << endl;
    }

    return 0;
}