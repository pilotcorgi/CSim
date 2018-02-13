//
//  main.cpp
//  CSim
//
//  Created by Fangzhou Wang on 10/31/17.
//  Copyright © 2017 Computer Aided Discovery. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <algorithm>

#include "Simulator.hpp"

using namespace std;

static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

string ReadNetlistFromFile(string file_name) {
    string netlist;
    
    ifstream netlist_file(file_name);
    
    if(!netlist_file) {
        cerr << "Cannot open netlist file: " << file_name << endl;
        exit(1);
    }
    
    string line;
    while (getline(netlist_file, line)) {
        line = line.substr(0, line.find_first_of(';'));
        trim(line);
        if (line.empty()) continue;
        if (line[0] == '*') continue;
        netlist += line + "\n";
    }
    
    return netlist;
}

void PrintTruthTable(list<string> inputs, list<string> outputs, string results) {
    cout << "|";
    for (auto item : inputs) {
        cout << item << "|";
    }
    cout << "|";
    for (auto item : outputs) {
        cout << item << "|";
    }
    cout << endl;
    
    int n = 0;
    for (auto one_bit : results) {
        cout << "|";
        int temp = n;
        size_t width;
        int j;
        for (auto input : inputs) {
            width = input.size();
            for (j=0; j<width/2; j++) {
                cout << " ";
            }
            cout << temp%2;
            for (; j<width-1; j++) {
                cout << " ";
            }
            cout << "|";
            temp /= 2;
        }
        cout << "|";
        width = outputs.back().size();
        for (j=0; j<width/2; j++) {
        cout << " ";
        }
        cout << one_bit;
        for (; j<width-1; j++) {
        cout << " ";
        }
        cout << "|" << endl;
        temp /= 2;
        n++;
    }
    
    cout << endl;
}

int main(int argc, const char * argv[]) {
    if (argc != 2 && argc != 3) {
        cerr << "Usage: CSim <netlist_file_name>" << endl;
        cerr << "\tCSim -s <netlist_string>" << endl;
        exit(1);
    }
    
    Simulator *simulator = new Simulator();
    //string netlist = "M0001 IN003 IN002 OUT01 VDD PMOS\nM0002 OUT01 IN001 GND GND NMOS\n";
    //string netlist = ReadNetlistFromFile("5t_bug.csim");
    M0001 VDD VDD OUT01 GND NMOS M0002 OUT01 VDD IN001 GND NMOS
    M0001 VDD VDD N0001 GND NMOS M0002 OUT01 IN001 N0001 GND NMOS
    string netlist = "";
    
    if (argc == 2) {
        netlist = ReadNetlistFromFile(argv[1]);
        cout << "--- netlist ---" << endl;
        cout << netlist << endl;
        simulator->ParseNetlist(netlist);
        simulator->DoSimulation();
        cout << "--- simulation results ---" << endl;
        cout << "--- precision level 1 ---" << endl;
        PrintTruthTable(simulator->getInputList(), simulator->getOutputList(), simulator->getSimulationResult(1));
        cout << "--- precision level 2 ---" << endl;
        PrintTruthTable(simulator->getInputList(), simulator->getOutputList(), simulator->getSimulationResult(2));
    } else {
        netlist = string(argv[2]);
        //netlist ="M0001 GND IN001 N0001 GND NMOS\nM0002 VDD IN001 N0001 VDD PMOS\nM0003 VDD N0001 OUT01 VDD PMOS\nM0004 IN002 IN001 OUT01 VDD PMOS\n";
        simulator->ParseNetlist(netlist);
        simulator->DoSimulation();
        cout << simulator->getSimulationResult(1) << "," << simulator->getSimulationResult(2) << endl;
    }

    return 0;
}
