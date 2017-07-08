#include "../Libraries/rapidxml-1.13/rapidxml.hpp"
#include "../Libraries/rapidxml-1.13/rapidxml_utils.hpp"

#include <iostream>
#include <cstring>

using namespace rapidxml;
using namespace std;

int countChildren(xml_node<> parentNode);

int main() {

    file<> xmlFile("test.xml"); //TODO: needs to have test.xml as a variable in a config file
    clog << "XML Loaded" << endl;

    xml_document<> doc;
    doc.parse<0>(xmlFile.data());
    clog << "XML Parsed" << endl;

    xml_node<> *topNode=doc.first_node(0);
    int topChildrenCount = count_children(topNode);

    //TODO: have something for version checking
    
    if (!strcmp(topNode->name(), "parameterFile")) { //TODO: needs to have "parameterFile" as var in config
        clog << "Top Node Loaded at " << topNode << " and has " << topChildrenCount << " children" << endl;
    }

    else {
        clog << "Unexpected top node: " << topNode->name() <<  endl;
        cerr << "Unexpected top node: " << topNode->name() <<  endl;

        return 0; //TODO: handle this quit more gracefully
    }

    xml_node<> topChildren[topChildrenCount] = new xml_node<>[];
    topChildren[0] = topNode->first_node(0);
    for (int i = 1; i < topChildrenCount; i++) {
        topChildren[i] = topChildren[i-1].next_sibling(0);
    }

    cout << "we did it" << endl;

    return 0;
}