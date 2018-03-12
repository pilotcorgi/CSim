//
//  Solver.cpp
//  CSim
//
//  Created by Fangzhou Wang on 11/2/17.
//  Copyright © 2017 Computer Aided Discovery. All rights reserved.
//

#include "Solver.hpp"

#include <iostream>

using namespace std;

Solver::Solver() {
}

Solver::~Solver() {
}

NodeState Solver::ComputeNewValue(list<SimNodePtr> *node_list) {
    /*
     value
     |  weak0   |   weak1   |   ratioed |   strong0 |   strong1 |   invalid |
     */
    int value = 0;
    
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
        } else if (node_temp->current_state_ == RATIOED_IMP) {
            value |= 0x1000000;
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
    } else if (value == 0x1000000) {
        new_value = RATIOED_IMP;
    } else {
        new_value = RATIOED;
    }
    
    if ((value & 0x000001) == 0x000001) {
        new_value = INVALID;
    }
    
    return new_value;
}

int Solver::UpdateTransistor(SimNodePtr node, set<SimNodePtr> &changed_list) {
    int update_cnt = 0;
    SimTransistorPtr temp_transistor;
    bool is_changed = false;
    
    for (auto iter: node->fanout_set_) {
        temp_transistor = iter.lock();
        if (
            ((STRONG0 == node->current_state_) && (nmos == temp_transistor->type_))
            || ((STRONG1 == node->current_state_) && (pmos == temp_transistor->type_))
            ) {
            temp_transistor->new_state_ = STRONG_OFF;
            if (temp_transistor->new_state_ != temp_transistor->state_) {
                temp_transistor->state_ = temp_transistor->new_state_;
                if (!IS_SOURCE(temp_transistor->drain_)) {
                    changed_list.insert(temp_transistor->drain_);
                    update_cnt++;
                }
                if (!IS_SOURCE(temp_transistor->source_)) {
                    changed_list.insert(temp_transistor->source_);
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
                    changed_list.insert(temp_transistor->drain_);
                    update_cnt++;
                }
                if (!IS_SOURCE(temp_transistor->source_)) {
                    changed_list.insert(temp_transistor->source_);
                    update_cnt++;
                }
            }
        }
        
        if (RATIOED == node->current_state_ || RATIOED_IMP == node->current_state_) {
            temp_transistor->new_state_ = RATIOED_STATE;
            if (temp_transistor->new_state_ != temp_transistor->state_) {
                temp_transistor->state_ = temp_transistor->new_state_;
                
                if (!IS_SOURCE(temp_transistor->drain_)) {
                    if (temp_transistor->drain_->current_state_ != RATIOED_IMP) {
                        temp_transistor->drain_->current_state_ = RATIOED_IMP;
                        changed_list.insert(temp_transistor->drain_);
                    }
                }
                
                if (!IS_SOURCE(temp_transistor->source_)) {
                    if (temp_transistor->source_->current_state_ != RATIOED_IMP) {
                        temp_transistor->source_->current_state_ = RATIOED_IMP;
                        changed_list.insert(temp_transistor->source_);
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
                        changed_list.insert(temp_transistor->drain_);
                    }
                }
                
                if (!IS_SOURCE(temp_transistor->source_)) {
                    if (temp_transistor->source_->current_state_ != INVALID) {
                        temp_transistor->source_->current_state_ = INVALID;
                        changed_list.insert(temp_transistor->source_);
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
                    changed_list.insert(temp_transistor->drain_);
                    update_cnt++;
                }
                if (!IS_SOURCE(temp_transistor->source_)) {
                    changed_list.insert(temp_transistor->source_);
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
