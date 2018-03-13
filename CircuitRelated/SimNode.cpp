//
//  SimNode.cpp
//  CADisCMOS
//
//  Created by Fangzhou Wang on 10/31/17.
//  Copyright © 2017 Computer Aided Discovery. All rights reserved.
//

#include "SimNode.hpp"

using namespace std;

SimNode::SimNode(string name) :
node_name_(name) {
    
}

SimNode::~SimNode() {
    
}

char SimNode::GetNodeValue() {
    char value_temp = '-';
    if (current_state_ == UNINITIALIZED) {
        value_temp = 'Z';
    }
    else if (current_state_ == INVALID) {
        value_temp = 'X';
    }
    else if (current_state_ == STRONG1) {
        value_temp = '1';
    }
    else if (current_state_ == STRONG0) {
        value_temp = '0';
    }
    else if (current_state_ == RATIOED_IMP) {
        value_temp = 'r';
    }
    else if (current_state_ == RATIOED) {
        value_temp = 'R';
    }
    else if (current_state_ == WEAK1) {
        value_temp = 'i';
    }
    else if (current_state_ == WEAK0) {
        value_temp = 'o';
    }
    else {
        value_temp = 'E';
    }
    return value_temp;
}

list<SimNodePtr> *SimNode::BFSSearch() {
    list<SimNodePtr> *list_temp = new list<SimNodePtr>();
    list<SimNodePtr> bfs_q;
    
    SimTransistorPtr temp_transistor;
    SimNodePtr other_node;
    SimNodePtr current_node = shared_from_this();
    
    SET_VISITED(current_node);
    list_temp->push_back(current_node);
    do {
        for (auto iter=current_node->storage_set_.begin(); iter!=current_node->storage_set_.end(); iter++) {
            temp_transistor = (*iter).lock();
            if (temp_transistor->state_ != STRONG_ON && temp_transistor->state_ != WEAK_ON) {
                continue;
            }
            
            other_node = temp_transistor->GetOtherNode(current_node);
            if (IS_VISITED(other_node)) {
                continue;
            }
            
            bfs_q.push_back(other_node);
            list_temp->push_back(other_node);
            SET_VISITED(other_node);
        }
        
        for (auto iter=current_node->input_set_.begin(); iter!=current_node->input_set_.end(); iter++) {
            temp_transistor = (*iter).lock();
            if (temp_transistor->state_ == STRONG_ON || temp_transistor->state_ == WEAK_ON) {
                list_temp->push_back(temp_transistor->GetOtherNode(current_node));
            }
        }
        
        if (bfs_q.empty()) {
            break;
        }
        
        current_node = bfs_q.front();
        bfs_q.pop_front();
        
    } while (true);
    
    return list_temp;
}

bool SimNode::isVDD() {
    return this->node_name_[0] == 'V';
}

bool SimNode::isGND() {
    return this->node_name_[0] == 'G';
}

bool SimNode::isInput() {
    return this->node_name_[0] == 'I';
}

bool SimNode::isOutput() {
    return this->node_name_[0] == 'O';
}

bool SimNode::isInternal() {
    return this->node_name_[0] == 'N';
}

SimNodePtr SimNode::GetNode(string node_name, vector<SimNodePtr> *list) {
    for (auto iter=list->begin(); iter!=list->end(); iter++) {
        if ((*iter)->node_name_ == node_name) {
            return (*iter);
        }
    }
    
    SimNodePtr temp_node = make_shared<SimNode>(node_name);
    temp_node->current_state_ = UNINITIALIZED;
    
    if (node_name[0] == 'V') {
        temp_node->new_state_ = STRONG1;
    } else if (node_name[0] == 'G') {
        temp_node->new_state_ = STRONG0;
    } else {
        temp_node->new_state_ = UNINITIALIZED;
    }
    
    list->push_back(temp_node);
    
    CLEAR_FLAG(temp_node);
    temp_node->node_level_ = -1;
    
    return temp_node;
}
