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
    list<SimNodePtr> input_list_temp = input_list_;
    input_list_.clear();
    for (auto node : input_list_temp) {
        if (node->node_name_ == "IN001") {
            input_list_.push_back(node);
            break;
        }
    }
    for (auto node : input_list_temp) {
        if (node->node_name_ == "IN002") {
            input_list_.push_back(node);
            break;
        }
    }
    for (auto node : input_list_temp) {
        if (node->node_name_ == "IN003") {
            input_list_.push_back(node);
            break;
        }
    }

    bool is_levelizable = LevelizeCell();
    is_levelizable = true;
    
    if (is_levelizable) {
        result_bsf_ = "";
        result_bsf_weak_ = "";
        for (int i=0; i<pow(2.0, input_list_.size()); i++) {
            SetStimuli(i);
            ResetNodeList();
            ResetTransistorList();
            
            RunSim();
            if (output_list_.size() == 0) {
                result_bsf_ += "-";
            } else {
                result_bsf_ = result_bsf_ + output_list_.front()->GetNodeValue();
            }

            SetStimuli(i);
            ResetNodeList();
            ResetTransistorList();
            
            RunSimWeak();
            
            if (output_list_.size() == 0) {
                result_bsf_weak_ += '-';
            } else {
                result_bsf_weak_ = result_bsf_weak_ + output_list_.front()->GetNodeValue();
            }
        }
    } else {
        result_bsf_ = "-";
        result_bsf_weak_ = "-";
    }

    if (result_bsf_.find("R") != std::string::npos) {
        string result_bsf_temp = result_bsf_;
        result_bsf_ = "";
        for (int i=0; i<pow(2.0, input_list_.size()); i++) {
            vector<string> temp_results;
            temp_results.push_back("-");
            temp_results.push_back("-");
            ratioed_node_.reset();
            for (int j=0; j<=1; j++) {
                ratioed_test_flag_ = j;
                SetStimuli(i);
                ResetNodeList();
                ResetTransistorList();
                
                RunSim();
                temp_results[j] = output_list_.front()->GetNodeValue();
            }
            if (temp_results[0] == temp_results[1]) {
                result_bsf_ = result_bsf_ + temp_results[0];
            } else {
                result_bsf_ = result_bsf_ + "R";
            }
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
        
        if (node->node_level_ > 10) {
            break;
        }
        
        // update fanout set of the node
        for (auto iter: node->fanout_set_) {
            auto temp_transistor = iter.lock();
            temp_transistor->gate_level_ = node->node_level_;
            if (temp_transistor->drain_->node_name_[0] == 'N' || temp_transistor->drain_->node_name_[0] == 'O') {
                if (temp_transistor->drain_->node_level_ < node->node_level_ + 1) {
                    temp_transistor->drain_->node_level_ = node->node_level_ + 1;
                    perturb_node_list.push_back(temp_transistor->drain_);
                }
            }
            
            if (temp_transistor->source_->node_name_[0] == 'N' || temp_transistor->source_->node_name_[0] == 'O') {
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
            int max_level = (drain_level > source_level) ? drain_level : source_level;
            
            
            if (temp_transistor->drain_->node_name_[0] == 'N' || temp_transistor->drain_->node_name_[0] == 'O') {
                if (temp_transistor->drain_->node_level_ < max_level) {
                    temp_transistor->drain_->node_level_ = max_level;
                    perturb_node_list.push_back(temp_transistor->drain_);
                }
            }
            
            if (temp_transistor->source_->node_name_[0] == 'N' || temp_transistor->source_->node_name_[0] == 'O') {
                if (temp_transistor->source_->node_level_ < max_level) {
                    temp_transistor->source_->node_level_ = max_level;
                    perturb_node_list.push_back(temp_transistor->source_);
                }
            }
        }
    }
    
    cell_max_level_ = -1;
    for (auto node : node_list_) {
        if (node->node_level_ == -1 || node->node_level_ > 10) {
            return false;
        }
    }
    for (auto node : output_list_) {
        if (node->node_level_ == -1 || node->node_level_ > 10) {
            return false;
        }
    }
    
    for (auto node : node_list_) {
        if (node->node_level_ > cell_max_level_) {
            cell_max_level_ = node->node_level_;
        }
    }
    for (auto node : output_list_) {
        if (node->node_level_ > cell_max_level_) {
            cell_max_level_ = node->node_level_;
        }
    }
    
    return true;
}

bool Simulator::GetBin(int num, char *str) {
    *(str + 30) = '\0';
    if (num > 2147483647) {
        /* maximum 30 inputs are supported */
        return false;
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

int Simulator::UpdateValueWeak(list<SimNodePtr> *node_list, list<SimNodePtr> *changed_list) {
    /*
     value
     |  weak0   |   weak1   |   ratioed |   strong0 |   strong1 |   invalid |
     */
    int value = 0;
    int changed_node_cnt = 0;
    
    for (auto node_temp : *node_list) {
        if (node_temp->current_state_ == UNINITIALIZED) {
            value |= 0x000000;
        } else if (node_temp->current_state_ == INVALID) {
            value |= 0x000001;
            //cout << "Error: invalid value found in update value: " << value << endl;
            //exit(1);
        } else if (node_temp->current_state_ == STRONG1) {
            value |= 0x000010;
        } else if (node_temp->current_state_ == STRONG0) {
            value |= 0x000100;
        } else if (node_temp->current_state_ == WEAK1) {
            value |= 0x010000;
        } else if (node_temp->current_state_ == WEAK0) {
            value |= 0x100000;
        } else if (node_temp->current_state_ == RATIOED) {
            value |= 0x001000;
        } else {
            cerr << "Error: Undefined value " << value << " found in update value" << endl;
        }
    }
    
    NodeState new_value;
    if (value == 0x000010 || value == 0x010010) {
        new_value = STRONG1;
    } else if (value == 0x000100 || value == 0x100100) {
        new_value = STRONG0;
    } else if (value == 0x010000) {
        new_value = WEAK1;
    } else if (value == 0x100000) {
        new_value = WEAK0;
    } else if (value == 0x000000) {
        new_value = UNINITIALIZED;
    } else {
        new_value = RATIOED;
    }
    
    if ((value & 0x000001) == 0x000001) {
        new_value = INVALID;
    }
    
    if (!(new_value == STRONG0 || new_value == STRONG1)) {
        for (auto node_temp : *node_list) {
            if (IS_SOURCE(node_temp)) {
                continue;
            }
            
            node_temp->new_state_ = new_value;
            if (node_temp->new_state_ != node_temp->current_state_) {
                node_temp->current_state_ = node_temp->new_state_;
                changed_list->push_back(node_temp);
                changed_node_cnt++;
            }
        }
    } else {
        list<SimNodePtr> list_temp;
        
        // 1.gather all root/source nodes
        for (auto node_temp : *node_list) {
            if (IS_SOURCE(node_temp)) {
                node_temp->new_state_ = node_temp->current_state_;
                list_temp.push_back(node_temp);
            }
        }
        
        if (list_temp.empty()) {
            cout << "weak NO SOURCE FOUND!" << endl;
        }
        
        while (!list_temp.empty()) {
            auto node = list_temp.front();
            list_temp.pop_front();
            
            
            
            for (auto temp_transistor : node->input_set_) {
                auto transistor = temp_transistor.lock();
                
                if (transistor->state_ != STRONG_ON && transistor->state_ != WEAK_ON) {
                    continue;
                }
                
                auto other_node = transistor->GetOtherNode(node);
                
                NodeState temp_value = node->new_state_;
                // Rule 1: skip source
                if (IS_SOURCE(other_node)) {
                    continue;
                }
                
                // strong on PMOS passing 0
                else if (transistor->type_ == pmos && temp_value == STRONG0 && transistor->state_ == STRONG_ON) {
                    // Rule 2: skip same value
                    //                    if (otherNode->current_state_ == weak0) {
                    if (other_node->new_state_ == WEAK0) {
                        continue;
                    }
                    //                    if (otherNode->current_state_ != strong0) {
                    if (other_node->new_state_ != STRONG0) {
                        other_node->new_state_ = WEAK0;
                        list_temp.push_back(other_node);
                    }
                }
                
                // weak on PMOS passing 0
                else if (transistor->type_ == pmos && temp_value == STRONG0 && transistor->state_ == WEAK_ON) {
                    // Rule 2: skip same value
                    //                    if (otherNode->current_state_ == invalid) {
                    if (other_node->new_state_ == INVALID) {
                        continue;
                    }
                    //                    if (otherNode->current_state_ != strong0 && otherNode->current_state_ != weak0) {
                    if (other_node->new_state_ != STRONG0 && other_node->new_state_ != WEAK0) {
                        other_node->new_state_ = INVALID;
                        list_temp.push_back(other_node);
                    }
                }
                
                // strong on NMOS passing 1
                else if (transistor->type_ == nmos && temp_value == STRONG1 && transistor->state_ == STRONG_ON) {
                    // Rule 2: skip same value
                    //                    if (otherNode->current_state_ == weak1) {
                    if (other_node->new_state_ == WEAK1) {
                        continue;
                    }
                    //                    if (otherNode->current_state_ != strong1) {
                    if (other_node->new_state_ != STRONG1) {
                        other_node->new_state_ = WEAK1;
                        list_temp.push_back(other_node);
                    }
                }
                
                // weak on NMOS passing 1
                else if (transistor->type_ == nmos && temp_value == STRONG1 && transistor->state_ == WEAK_ON) {
                    // Rule 2: skip same value
                    //                    if (otherNode->current_state_ == invalid) {
                    if (other_node->new_state_ == INVALID) {
                        continue;
                    }
                    //                    if (otherNode->current_state_ != strong1 && otherNode->current_state_ != weak1) {
                    if (other_node->new_state_ != STRONG1 && other_node->new_state_ != WEAK1) {
                        other_node->new_state_ = INVALID;
                        list_temp.push_back(other_node);
                    }
                }
                
                else {
                    if (other_node->new_state_ == temp_value) {
                        continue;
                    }
                    other_node->new_state_ = temp_value;
                    list_temp.push_back(other_node);
                }
                
            }
            
            for (auto temp_transistor : node->storage_set_) {
                auto transistor = temp_transistor.lock();
                
                if (transistor->state_ != STRONG_ON && transistor->state_ != WEAK_ON) {
                    continue;
                }
                auto other_node = transistor->GetOtherNode(node);
                
                NodeState temp_value = node->new_state_;
                // Rule 1: skip source
                if (IS_SOURCE(other_node)) {
                    continue;
                }
                
                // strong on PMOS passing 0
                else if (transistor->type_ == pmos && temp_value == STRONG0 && transistor->state_ == STRONG_ON) {
                    // Rule 2: skip same value
                    //                    if (otherNode->current_state_ == weak0) {
                    if (other_node->new_state_ == WEAK0) {
                        continue;
                    }
                    //                    if (otherNode->current_state_ != strong0) {
                    if (other_node->new_state_ != STRONG0) {
                        other_node->new_state_ = WEAK0;
                        list_temp.push_back(other_node);
                    }
                }
                // weak on PMOS passing 0
                else if (transistor->type_ == pmos && temp_value == STRONG0 && transistor->state_ == WEAK_ON) {
                    // Rule 2: skip same value
                    //                    if (otherNode->current_state_ == invalid) {
                    if (other_node->new_state_ == INVALID) {
                        continue;
                    }
                    //                    if (otherNode->current_state_ != strong0 && otherNode->current_state_ != weak0) {
                    if (other_node->new_state_ != STRONG0 && other_node->new_state_ != WEAK0) {
                        other_node->new_state_ = INVALID;
                        list_temp.push_back(other_node);
                    }
                }
                
                // strong on NMOS passing 1
                else if (transistor->type_ == nmos && temp_value == STRONG1 && transistor->state_ == STRONG_ON) {
                    // Rule 2: skip same value
                    //                    if (otherNode->current_state_ == weak1) {
                    if (other_node->new_state_ == WEAK1) {
                        continue;
                    }
                    //                    if (otherNode->current_state_ != strong1) {
                    if (other_node->new_state_ != STRONG1) {
                        other_node->new_state_ = WEAK1;
                        list_temp.push_back(other_node);
                    }
                }
                // weak on NMOS passing 1
                else if (transistor->type_ == nmos && temp_value == STRONG1 && transistor->state_ == WEAK_ON) {
                    // Rule 2: skip same value
                    //                    if (otherNode->current_state_ == invalid) {
                    if (other_node->new_state_ == INVALID) {
                        continue;
                    }
                    //                    if (otherNode->current_state_ != strong1 && otherNode->current_state_ != weak1) {
                    if (other_node->new_state_ != STRONG1 && other_node->new_state_ != WEAK1) {
                        other_node->new_state_ = INVALID;
                        list_temp.push_back(other_node);
                    }
                }
                
                else {
                    if (other_node->new_state_ == temp_value) {
                        continue;
                    }
                    other_node->new_state_ = temp_value;
                    list_temp.push_back(other_node);
                }
                
            }
        }
        
        
        for (auto node_temp : *node_list) {
            if (IS_SOURCE(node_temp)) {
                continue;
            }
            
            if (node_temp->new_state_ != node_temp->current_state_) {
                node_temp->current_state_ = node_temp->new_state_;
                changed_list->push_back(node_temp);
                changed_node_cnt++;
            }
        }
        
    }
    
    node_list->clear();
    delete node_list;
    
    return changed_node_cnt;
}

int Simulator::UpdateValue(list<SimNodePtr> *node_list, list<SimNodePtr> *changed_list) {
    /*
     value
     |  weak0   |   weak1   |   ratioed |   strong0 |   strong1 |   invalid |
     */
    int value = 0;
    int changed_node_cnt = 0;
    
    for (auto node_temp : *node_list) {
        if (node_temp->current_state_ == UNINITIALIZED) {
            value |= 0x000000;
        } else if (node_temp->current_state_ == INVALID) {
            value |= 0x000001;
            //cout << "Error: invalid value found in update value: " << value << endl;
            //exit(1);
        } else if (node_temp->current_state_ == STRONG1) {
            value |= 0x000010;
        } else if (node_temp->current_state_ == STRONG0) {
            value |= 0x000100;
        } else if (node_temp->current_state_ == WEAK1) {
            value |= 0x010000;
        } else if (node_temp->current_state_ == WEAK0) {
            value |= 0x100000;
        } else if (node_temp->current_state_ == RATIOED) {
            value |= 0x001000;
        } else {
            cerr << "Error: Undefined value " << value << " found in update value" << endl;
        }
    }
    
    NodeState new_value;
    if (value == 0x000010 || value == 0x010010) {
        new_value = STRONG1;
    } else if (value == 0x000100 || value == 0x100100) {
        new_value = STRONG0;
    } else if (value == 0x010000) {
        new_value = WEAK1;
    } else if (value == 0x100000) {
        new_value = WEAK0;
    } else if (value == 0x000000) {
        new_value = UNINITIALIZED;
    } else {
        new_value = RATIOED;
    }
    
    if ((value & 0x000001) == 0x000001) {
        new_value = INVALID;
    }
    
    for (auto node_temp : *node_list) {
        if (IS_SOURCE(node_temp)) {
            continue;
        }
        
        node_temp->new_state_ = new_value;
        if (node_temp->new_state_ != node_temp->current_state_) {
            node_temp->current_state_ = node_temp->new_state_;
            changed_list->push_back(node_temp);
            changed_node_cnt++;
        }
    }
    
    node_list->clear();
    delete node_list;
    
    return changed_node_cnt;
}

int Simulator::UpdateTransistor(SimNodePtr node, list<SimNodePtr> *changed_list) {
    int update_cnt = 0;
    SimTransistorPtr temp_transistor;
    bool is_changed = false;
    
    for (auto iter: node->fanout_set_) {
        if (ratioed_test_flag_ == 0 && node->current_state_ == RATIOED && ratioed_node_ == node) {
            node->current_state_ = STRONG0;
            is_changed = true;
        }
        if (ratioed_test_flag_ == 1 && node->current_state_ == RATIOED && ratioed_node_ == node) {
            node->current_state_ = STRONG1;
            is_changed = true;
        }
        if (ratioed_test_flag_ == 0 && node->current_state_ == RATIOED && !ratioed_node_) {
            node->current_state_ = STRONG0;
            ratioed_node_ = node;
            is_changed = true;
        }
        temp_transistor = iter.lock();
        if (
            ((STRONG0 == node->current_state_) && (nmos == temp_transistor->type_))
            || ((STRONG1 == node->current_state_) && (pmos == temp_transistor->type_))
            ) {
            temp_transistor->new_state_ = STRONG_OFF;
            if (temp_transistor->new_state_ != temp_transistor->state_) {
                temp_transistor->state_ = temp_transistor->new_state_;
                if (!IS_SOURCE(temp_transistor->drain_)) {
                    changed_list->push_back(temp_transistor->drain_);
                    update_cnt++;
                }
                if (!IS_SOURCE(temp_transistor->source_)) {
                    changed_list->push_back(temp_transistor->source_);
                    update_cnt++;
                }
            }
        }
        
        if (
            ((STRONG1 == node->current_state_) && (nmos == temp_transistor->type_))
            || ((STRONG0 == node->current_state_) && (pmos == temp_transistor->type_))
            ) {
            temp_transistor->new_state_ = STRONG_ON;
            if (temp_transistor->new_state_ != temp_transistor->state_) {
                temp_transistor->state_ = temp_transistor->new_state_;
                if (!IS_SOURCE(temp_transistor->drain_)) {
                    changed_list->push_back(temp_transistor->drain_);
                    update_cnt++;
                }
                if (!IS_SOURCE(temp_transistor->source_)) {
                    changed_list->push_back(temp_transistor->source_);
                    update_cnt++;
                }
            }
        }
        
        if (RATIOED == node->current_state_) {
            temp_transistor->new_state_ = RATIOED_STATE;
            if (temp_transistor->new_state_ != temp_transistor->state_) {
                temp_transistor->state_ = temp_transistor->new_state_;
                
                if (!IS_SOURCE(temp_transistor->drain_)) {
                    if (temp_transistor->drain_->current_state_ != RATIOED) {
                        temp_transistor->drain_->current_state_ = RATIOED;
                        changed_list->push_back(temp_transistor->drain_);
                    }
                }
                
                if (!IS_SOURCE(temp_transistor->source_)) {
                    if (temp_transistor->source_->current_state_ != RATIOED) {
                        temp_transistor->source_->current_state_ = RATIOED;
                        changed_list->push_back(temp_transistor->source_);
                    }
                }
            }
        }
        
        
        if (INVALID == node->current_state_ || UNINITIALIZED == node->current_state_) {
            temp_transistor->new_state_ = INVALID_STATE;
            if (temp_transistor->new_state_ != temp_transistor->state_) {
                temp_transistor->state_ = temp_transistor->new_state_;
                
                if (!IS_SOURCE(temp_transistor->drain_)) {
                    if (temp_transistor->drain_->current_state_ != INVALID) {
                        temp_transistor->drain_->current_state_ = INVALID;
                        changed_list->push_back(temp_transistor->drain_);
                    }
                }
                
                if (!IS_SOURCE(temp_transistor->source_)) {
                    if (temp_transistor->source_->current_state_ != INVALID) {
                        temp_transistor->source_->current_state_ = INVALID;
                        changed_list->push_back(temp_transistor->source_);
                    }
                }
            }
        }
        
        
        if (
            (WEAK0 == node->current_state_) || (WEAK1 == node->current_state_)
            ) {
            temp_transistor->new_state_ = WEAK_ON;
            if (temp_transistor->new_state_ != temp_transistor->state_) {
                temp_transistor->state_ = temp_transistor->new_state_;
                if (!IS_SOURCE(temp_transistor->drain_)) {
                    changed_list->push_back(temp_transistor->drain_);
                    update_cnt++;
                }
                if (!IS_SOURCE(temp_transistor->source_)) {
                    changed_list->push_back(temp_transistor->source_);
                    update_cnt++;
                }
            }
        }
        if (is_changed) {
            node->current_state_ = RATIOED;
            is_changed = false;
        }
    }
    
    return update_cnt;
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

void Simulator::RunSim() {
    list<SimNodePtr> *old_changed_list = &changed_list1_;
    list<SimNodePtr> *new_changed_list = &changed_list2_;
    list<SimNodePtr> *high_level_changed_list = &changed_list3_;
    list<SimNodePtr> *swap_temp;
    
    InitStimuli(new_changed_list);
    
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
                UpdateValue(node_temp->BFSSearch(), new_changed_list);
                UpdateTransistor(node_temp, new_changed_list);
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
    
    old_changed_list->clear();
    new_changed_list->clear();
}


void Simulator::RunSimWeak() {
    list<SimNodePtr> *old_changed_list = &changed_list1_;
    list<SimNodePtr> *new_changed_list = &changed_list2_;
    list<SimNodePtr> *high_level_list = &changed_list3_;
    list<SimNodePtr> *swap_temp;
    
    InitStimuli(new_changed_list);
    
    deadlock_cnt_ = 0;
    
    for (int level=0; level <= cell_max_level_; level++) {
        while (!new_changed_list->empty()) {
            old_changed_list->clear();
            swap_temp = old_changed_list;
            old_changed_list = new_changed_list;
            new_changed_list = swap_temp;
            
            for (auto node_temp : *old_changed_list) {
                if (node_temp->node_level_ > level) {
                    high_level_list->push_back(node_temp);
                    continue;
                }
                ClearVisitNodeList();
                UpdateValueWeak(node_temp->BFSSearch(), new_changed_list);
                UpdateTransistor(node_temp, new_changed_list);
            }
            
            deadlock_cnt_++;
            if (deadlock_cnt_ == 50) {
                m_cout_.lock();
                cerr << "ERROR: deadlock detected in CSim." << endl;
                m_cout_.unlock();
            }
        }
        new_changed_list->splice(new_changed_list->end(), *high_level_list);
    }
    
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
        if ('N' == temp_transistor->drain_->node_name_[0]) {
            temp_transistor->source_->storage_set_.push_back(temp_transistor);
        }
        else if ('O' == temp_transistor->drain_->node_name_[0]) {
            temp_transistor->source_->storage_set_.push_back(temp_transistor);
        }
        else {
            temp_transistor->source_->input_set_.push_back(temp_transistor);
        }
        
        if ('N' == temp_transistor->source_->node_name_[0]) {
            temp_transistor->drain_->storage_set_.push_back(temp_transistor);
        }
        else if ('O' == temp_transistor->drain_->node_name_[0]) {
            temp_transistor->drain_->storage_set_.push_back(temp_transistor);
        }
        else {
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
