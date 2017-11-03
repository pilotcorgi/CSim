//
//  WeakPreciseSolver.cpp
//  CSim
//
//  Created by Fangzhou Wang on 11/2/17.
//  Copyright © 2017 Computer Aided Discovery. All rights reserved.
//

#include "WeakPreciseSolver.hpp"

#include <iostream>

using namespace std;

WeakPreciseSolver::WeakPreciseSolver() {
}

WeakPreciseSolver::~WeakPreciseSolver() {
}

int WeakPreciseSolver::UpdateValue(std::list<SimNodePtr> *node_list, std::list<SimNodePtr> *changed_list) {
    int changed_node_cnt = 0;
    
    NodeState new_value = ComputeNewValue(node_list);
    
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

