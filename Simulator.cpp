//
//  Simulator.cpp
//  CADisCMOS
//
//  Created by Fangzhou Wang on 10/31/17.
//  Copyright © 2017 Computer Aided Discovery. All rights reserved.
//

#include "Simulator.hpp"

#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

using namespace std;

mutex Simulator::m_cout_;

void Simulator::ResetSimulator() {
    sim_transistor_list_.clear();
    input_list_.clear();
    output_list_.clear();
    supply_list_.clear();
    node_list_.clear();
    changed_list1_.clear();
    changed_list2_.clear();
    
    ratioed_test_flag_ = -1;
}

void Simulator::DoSimulation() {
    sort(input_list_.begin(), input_list_.end(), [](SimNodePtr node_1, SimNodePtr node_2){
        return node_1->node_name_ < node_2->node_name_;
    });

    LevelizeCell();

    result_bsf_ = "";
    result_bsf_weak_ = "";
    for (int i=0; i<pow(2.0, input_list_.size()); i++) {
        SetStimuli(i);
        ResetNodeList();
        ResetTransistorList();

        RunSim(1);
        if (output_list_.size() == 0) {
            result_bsf_ += "-";
        } else {
            result_bsf_ = result_bsf_ + output_list_.front()->GetNodeValue();
        }

        SetStimuli(i);
        ResetNodeList();
        ResetTransistorList();

        RunSim(2);

        if (output_list_.size() == 0) {
            result_bsf_weak_ += '-';
        } else {
            result_bsf_weak_ = result_bsf_weak_ + output_list_.front()->GetNodeValue();
        }
    }
}

bool Simulator::LevelizeCell() {
    
    // all nodes in input and supply list are LEVEL 0
    list<SimNodePtr> perturb_node_list;
    
    // pass - 0
    for (auto node : input_list_) {
        node->node_level_ = 0;
        perturb_node_list.push_back(node);
    }
    for (auto node : supply_list_) {
        node->node_level_ = 0;
        perturb_node_list.push_back(node);
    }
    
    // pass - i
    while (perturb_node_list.empty() == false) {
        auto node = perturb_node_list.front();
        perturb_node_list.pop_front();
        
        // update fanout set of the node
        for (auto iter: node->fanout_set_) {
            auto temp_transistor = iter.lock();
            temp_transistor->gate_level_ = node->node_level_;
            if (temp_transistor->drain_->isInternal() || temp_transistor->drain_->isOutput()) {
                if (temp_transistor->drain_->node_level_ < node->node_level_ + 1) {
                    temp_transistor->drain_->node_level_ = node->node_level_ + 1;
                    perturb_node_list.push_back(temp_transistor->drain_);
                }
            }
            
            if (temp_transistor->source_->isInternal() || temp_transistor->source_->isOutput()) {
                if (temp_transistor->source_->node_level_ < node->node_level_ + 1) {
                    temp_transistor->source_->node_level_ = node->node_level_ + 1;
                    perturb_node_list.push_back(temp_transistor->source_);
                }
            }
        }
        
        // update storage set of the node
        for (auto iter : node->storage_set_) {
            auto temp_transistor = iter.lock();
            int drain_level = temp_transistor->drain_->node_level_;
            int source_level = temp_transistor->source_->node_level_;
            int max_level = max(drain_level, source_level);
            
            
            if (temp_transistor->drain_->isInternal() || temp_transistor->drain_->isOutput()) {
                if (temp_transistor->drain_->node_level_ < max_level) {
                    temp_transistor->drain_->node_level_ = max_level;
                    perturb_node_list.push_back(temp_transistor->drain_);
                }
            }
            
            if (temp_transistor->source_->isInternal() || temp_transistor->source_->isOutput()) {
                if (temp_transistor->source_->node_level_ < max_level) {
                    temp_transistor->source_->node_level_ = max_level;
                    perturb_node_list.push_back(temp_transistor->source_);
                }
            }
        }
    }
    
    cell_max_level_ = -1;
    int temp_max = -1;
    
    for (auto node : node_list_) {
        if (node->node_level_ == -1)
            return false;
        temp_max = max(temp_max, node->node_level_);
    }
    for (auto node : output_list_) {
        if (node->node_level_ == -1)
            return false;
        temp_max = max(temp_max, node->node_level_);
    }
    
    cell_max_level_ = temp_max;
    
    return true;
}

bool Simulator::GetBin(int num, char *str) {
    *(str + 30) = '\0';
    if (num > 2147483647) {
        /* maximum 30 inputs are supported */
        cerr << "Exceeded 30 inputs." << endl;
        exit(1);
    }
    unsigned int mask = 0x40000000 << 1;
    while (mask >>= 1)
        *str++ = !!(mask & num) + '0';
    return true;
}

void Simulator::SetStimuli(int stimuli) {
    char str[31];
    int i = 30;
    GetBin(stimuli, str);
    
    for (auto iter = input_list_.begin(); iter != input_list_.end(); iter++) {
        if ('0' == str[i]) {
            (*iter)->new_state_ = STRONG0;
        } else {
            (*iter)->new_state_ = STRONG1;
        }
        i--;
    }
}

void Simulator::InitStimuli(list<SimNodePtr> *changed_list) {
    for (auto temp_node : input_list_) {
        SET_SOURCE(temp_node);
        temp_node->current_state_ = temp_node->new_state_;
        changed_list->push_back(temp_node);
    }
    
    for (auto temp_node : supply_list_) {
        SET_SOURCE(temp_node);
        temp_node->current_state_ = temp_node->new_state_;
        changed_list->push_back(temp_node);
    }
}

void Simulator::ResetTransistorList() {
    for (auto temp_transistor : sim_transistor_list_) {
        temp_transistor->new_state_ = UNKNOWN;
        temp_transistor->state_ = UNKNOWN;
    }
}

void Simulator::ResetNodeList() {
    for (auto temp_node : node_list_) {
        CLEAR_FLAG(temp_node);
        temp_node->current_state_ = UNINITIALIZED;
        temp_node->new_state_ = UNINITIALIZED;
    }
    
    for (auto temp_node : output_list_) {
        CLEAR_FLAG(temp_node);
        temp_node->current_state_ = UNINITIALIZED;
        temp_node->new_state_ = UNINITIALIZED;
    }
}

void Simulator::ClearVisitNodeList() {
    for (auto temp_node : input_list_) {
        CLR_VISITED(temp_node);
    }
    for (auto temp_node : output_list_) {
        CLR_VISITED(temp_node);
    }
    for (auto temp_node : node_list_) {
        CLR_VISITED(temp_node);
    }
    for (auto temp_node : supply_list_) {
        CLR_VISITED(temp_node);
    }
}

void Simulator::RunSim(int precision_level) {
    list<SimNodePtr> *old_changed_list = &changed_list1_;
    list<SimNodePtr> *new_changed_list = &changed_list2_;
    list<SimNodePtr> *high_level_changed_list = &changed_list3_;
    list<SimNodePtr> *swap_temp;
    
    InitStimuli(new_changed_list);
    
    Solver *solver = solver_factory_.getSolver(precision_level);
    
    deadlock_cnt_ = 0;
    
    for (int level=0; level <= cell_max_level_; level++) {
        while (!new_changed_list->empty()) {
            old_changed_list->clear();
            swap_temp = old_changed_list;
            old_changed_list = new_changed_list;
            new_changed_list = swap_temp;
            
            for (auto node_temp : *old_changed_list) {
                if (node_temp->node_level_ > level) {
                    high_level_changed_list->push_back(node_temp);
                    continue;
                }
                ClearVisitNodeList();
                solver->UpdateValue(node_temp->BFSSearch(), new_changed_list);
                solver->UpdateTransistor(node_temp, new_changed_list);
            }
            
            deadlock_cnt_++;
            if (deadlock_cnt_ == 50) {
                m_cout_.lock();
                cerr << "ERROR: deadlock detected in CSim." << endl;
                m_cout_.unlock();
            }
        }
        new_changed_list->splice(new_changed_list->end(), *high_level_changed_list);
    }
    
    delete solver;
    
    old_changed_list->clear();
    new_changed_list->clear();
}

string Simulator::getSimulationResult(int precision_level) {
    if (precision_level == 1) {
        return result_bsf_;
    } else {
        return result_bsf_weak_;
    }
}

int Simulator::getCellMaxLevel() {
    return cell_max_level_;
}

std::vector<std::string> &Simulator::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> Simulator::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void Simulator::ParseNetlist(string netlist_string) {
    this->ResetSimulator();
    
    SimTransistorPtr temp_transistor;
    SimNodePtr terminal_nodes[CMOS_TRANSISTOR_PORT_CNT];
    string terminal_net_name;
    
    vector<string> transistors = split(netlist_string, '\n');
    
    for (auto transistor : transistors) {
        vector<string> elements = split(transistor, ' ');
        if (elements[5] == "PMOS") {
            temp_transistor = make_shared<SimTransistor>(stoi(elements[0].substr(1)), pmos);
        } else {
            temp_transistor = make_shared<SimTransistor>(stoi(elements[0].substr(1)), nmos);
        }
        
        this->sim_transistor_list_.push_back(temp_transistor);
        
        for (int i=0; i<CMOS_TRANSISTOR_PORT_CNT; i++) {
            terminal_net_name = elements[i+1];
            if (terminal_net_name[0] == 'I') {
                terminal_nodes[i] = SimNode::GetNode(terminal_net_name, &input_list_);
            } else if (terminal_net_name[0] == 'O') {
                terminal_nodes[i] = SimNode::GetNode(terminal_net_name, &output_list_);
            } else if (terminal_net_name[0] == 'N') {
                terminal_nodes[i] = SimNode::GetNode(terminal_net_name, &node_list_);
            } else {
                terminal_nodes[i] = SimNode::GetNode(terminal_net_name, &supply_list_);
            }
            
        }
        temp_transistor->drain_ = terminal_nodes[0];
        temp_transistor->source_ = terminal_nodes[2];
        
        terminal_nodes[1]->fanout_set_.push_back(temp_transistor);
        if (temp_transistor->drain_->isInternal() || temp_transistor->drain_->isOutput()) {
            temp_transistor->source_->storage_set_.push_back(temp_transistor);
        } else {
            temp_transistor->source_->input_set_.push_back(temp_transistor);
        }
        
        if (temp_transistor->source_->isInternal() || temp_transistor->source_->isOutput()) {
            temp_transistor->drain_->storage_set_.push_back(temp_transistor);
        } else {
            temp_transistor->drain_->input_set_.push_back(temp_transistor);
        }
    }
}

list<string> Simulator::getInputList() {
    list<string> ret;
    for (auto input : input_list_) {
        ret.push_back(input->node_name_);
    }
    return ret;
}

list<string> Simulator::getOutputList() {
    list<string> ret;
    for (auto output : output_list_) {
        ret.push_back(output->node_name_);
    }
    return ret;
}
