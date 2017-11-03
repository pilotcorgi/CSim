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

#include "SimNode.hpp"
#include "SimTransistor.hpp"
#include "SolverFactory.hpp"

class Simulator {
public:
    void DoSimulation();
    void ParseNetlist(std::string netlist_string);
    std::string getSimulationResult(int precision_level);
    std::list<std::string> getInputList();
    std::list<std::string> getOutputList();
    int getCellMaxLevel();
    void ResetSimulator();
    
    std::list<SimTransistorPtr> sim_transistor_list_;
    std::vector<SimNodePtr> input_list_;
    std::vector<SimNodePtr> output_list_;
    std::vector<SimNodePtr> supply_list_;
    std::vector<SimNodePtr> node_list_;
    
private:
    std::list<SimNodePtr> changed_list1_, changed_list2_, changed_list3_;
    
    std::string result_bsf_;
    std::string result_bsf_weak_;
    int deadlock_cnt_;
    static std::mutex m_cout_;
    int cell_max_level_;
    
    // supporting one time ratioed case
    int ratioed_test_flag_;
    SimNodePtr ratioed_node_;
    SolverFactory solver_factory_;
    
    bool GetBin(int num, char *str);
    void SetStimuli(int stimuli);
    NodeState ComputeNewValue(std::list<SimNodePtr> *node_list);
    bool LevelizeCell();
    void InitStimuli(std::list<SimNodePtr> *changed_list);
    void ResetTransistorList();
    void ResetNodeList();
    void ClearVisitNodeList();
    void RunSim(int precision_level);
    std::vector<std::string> split(const std::string &s, char delim);
    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
};

#endif /* Simulator_hpp */
