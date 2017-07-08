#include "../Libraries/rapidxml-1.13/rapidxml.hpp"
#include "../Libraries/rapidxml-1.13/rapidxml_utils.hpp"

#include <iostream>
#include <cstring>

using namespace rapidxml;
using namespace std;


int main() {

    file<> xmlFile("test.xml"); //TODO: needs to have test.xml as a variable in a config file
    clog << "XML Loaded" << endl;

    xml_document<> doc;
    doc.parse<0>(xmlFile.data());
    clog << "XML Parsed" << endl;

    xml_node<> *topNode=doc.first_node(0);

    //TODO: have something for version checking
    
    if (!strcmp(topNode->name(), "parameterFile")) { //TODO: needs to have "parameterFile" as var in config
        clog << "Top Node Loaded at " << topNode << endl;
    }

    else {
        clog << "Unexpected first node: " << topNode->name() <<  endl;
        cerr << "Unexpected first node: " << topNode->name() <<  endl;

        return 0;
    }

    //TODO: cycle thorough next-generation nodes and store in an array
    xml_node<> *firstChild = topNode->first_node(0);
    if (firstChild != 0) { //There's a node
        //count the nodes
        cout << "Child Node Found: " << firstChild << endl;
    }

    else { //There isn't a child node. This is unexpected
        clog << "No Child Node Found" << endl;
        cerr << "No Child Node Found" << endl;
    }



    return 0;
}