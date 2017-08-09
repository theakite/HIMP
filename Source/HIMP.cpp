#include "./HIMP.hpp"
#include "./ConfigFile.hpp"

using namespace rapidxml;
using namespace std;
using namespace Configuration;

int countChildren(xml_node<> parentNode);

int main(int argc, char *argv[]) {

    ConfigFile config = ConfigFile("./HIMP.config");

    ofstream logFile("logFile.txt");
    int counter101 = 0;
    int counter217 = 0;
    int max101, min101, max217, min217;
    while(1) {
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
            //cerr << "Unexpected top node: " << topNode->name() <<  endl;

            return 0; //TODO: handle this quit more gracefully
        }

        xml_node<> *topChildren[topChildrenCount];
        xml_node<> *modulesTop;
        xml_node<> *modulesAll[10];
        xml_node<> *crowd, *rules, *room101, *room217;

        topChildren[0] = topNode->first_node(0);
        for (int i = 1; i < topChildrenCount; i++) {
            topChildren[i] = topChildren[i-1]->next_sibling(0);
        }

        // for testing
        //for (int i = 0; i < topChildrenCount; i++) {
        //    cout << "Child " << i << " is " << topChildren[i]->name() << endl;
        //}

        modulesTop = topNode->first_node(config.getModuleTop().c_str());
        modulesAll[0] = modulesTop->first_node(0);
        for (int i = 1; i < config.getNumModules(); i++) {
            //have a check based on info from config file
            modulesAll[i] = modulesAll[i-1]->next_sibling(0);
        }
        crowd = modulesTop->first_node("crowdsourcingModule");
        rules = crowd->first_node("processingRules");
        room101 = rules->first_node("roomRule");
        room217 = room101->next_sibling("roomRule");
        //cout << room101->first_attribute("hotCount")->name() << endl;
        char *temp = room101->first_attribute("hotCount")->value();
        max101 = atoi(temp);
        cout << max101 << endl;

        temp = room101->first_attribute("coldCount")->value();
        min101 = -1 * atoi(temp);
        cout << min101 << endl;

        temp = room217->first_attribute("hotCount")->value();
        max217 = atoi(temp);
        cout << max217 << endl;

        temp = room217->first_attribute("coldCount")->value();
        min217 = -1 * atoi(temp);
        cout << min217 << endl;


        
        cout << "Enter room number followed by input" << endl;
        string room;
        cin >> room;
        string input;
        cin >> input;

        //cout << "Room is " << room << " and input is " << input << endl;
        if (atoi(room.c_str()) == 101) {
            //do things
            if (input == "th") {
                counter101++;
            }
            else if (input == "tc") {
                counter101--;
            }
            else {
                cout << "unexpected command" << endl;
            }
            if (counter101 >= max101) {
                counter101 = 0;
                cout << "Send BacNet message to increase temp in Room 101" << endl;
            }
            else if (counter101 <= min101) {
                counter101 = 0;
                cout << "Send BacNet message to decrease temp in Room 101" << endl;
            }
            else {
                cout << "No change yet. Current value is " << counter101 << endl;
            }
        }
        else if (atoi(room.c_str()) == 217) {
            //do things
            if (input == "th") {
                counter217++;
            }
            else if (input == "tc") {
                counter217--;
            }
            else {
                cout << "unexpected command" << endl;
            }
            if (counter217 >= max217) {
                counter217 = 0;
                cout << "Send BacNet message to increase temp in Room 217" << endl;
            }
            else if (counter217 <= min217) {
                counter217 = 0;
                cout << "Send BacNet message to decrease temp in Room 217" << endl;
            }
            else {
                cout << "No change yet. Current value is " << counter217 << endl;
            }
        }
        else {
            cout << "Unexpected Room Number" << endl;
        }
        
    }

    return 0;
}