//
//  Simulator.hpp
//  CADisCMOS
//
//  Created by Fangzhou Wang on 10/31/17.
//  Copyright © 2017 Computer Aided Discovery. All rights reserved.
//

#ifndef Simulator_hpp
#define Simulator_hpp

#include <list>
#include <string>
#include <mutex>

#include "TaskPool.hpp"
#include "SimNode.hpp"
#include "SimTransistor.hpp"

class Simulator {
public:
    void DoSimulation();
    void ParseNetlist(std::string netlist_string);
    std::string getSimulationResult(int precision_level);
    std::list<std::string> getInputList();
    std::list<std::string> getOutputList();
    int getCellMaxLevel();
    void ResetSimulator();
    
//private:
    std::list<SimTransistorPtr> sim_transistor_list_;
    std::list<SimNodePtr> input_list_;
    std::list<SimNodePtr> output_list_;
    std::list<SimNodePtr> supply_list_;
    std::list<SimNodePtr> node_list_;
    std::list<SimNodePtr> changed_list1_, changed_list2_, changed_list3_;
    
    std::string result_bsf_;
    std::string result_bsf_weak_;
    int deadlock_cnt_;
    static std::mutex m_cout_;
    int cell_max_level_;
    
    // supporting one time ratioed case
    int ratioed_test_flag_;
    SimNodePtr ratioed_node_;
    
    bool GetBin(int num, char *str);
    void SetStimuli(int stimuli);
    bool LevelizeCell();
    int UpdateValue(std::list<SimNodePtr> *node_list, std::list<SimNodePtr> *changed_list);
    int UpdateValueWeak(std::list<SimNodePtr> *node_list, std::list<SimNodePtr> *changed_list);
    int UpdateTransistor(SimNodePtr node, std::list<SimNodePtr> *changed_list);
    void InitStimuli(std::list<SimNodePtr> *changed_list);
    void ResetTransistorList();
    void ResetNodeList();
    void ClearVisitNodeList();
    void RunSim();
    void RunSimWeak();
    std::vector<std::string> split(const std::string &s, char delim);
    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
};

#endif /* Simulator_hpp */
