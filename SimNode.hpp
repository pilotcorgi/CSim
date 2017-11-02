//
//  SimNode.hpp
//  CADisCMOS
//
//  Created by Fangzhou Wang on 10/31/17.
//  Copyright © 2017 Computer Aided Discovery. All rights reserved.
//

#ifndef SimNode_hpp
#define SimNode_hpp

#include <vector>
#include <memory>
#include <list>
#include <string>

#include "SimTransistor.hpp"

#define     NO_FLAG         0x00000000
#define     NODE_DIRTY      0x00000001
#define     NODE_VISITED    0x00000002
#define     NODE_SOURCE     0x00000010

#define     CLEAR_FLAG(x)   (x->flags_ = NO_FLAG)

#define     SET_VISITED(x)  (x->flags_ |= NODE_VISITED)
#define     CLR_VISITED(x)  (x->flags_ &= ~NODE_VISITED)
#define     IS_VISITED(x)   ((x->flags_ & NODE_VISITED) == NODE_VISITED)

#define     SET_SOURCE(x)   (x->flags_ |= NODE_SOURCE)
#define     CLR_SOURCE(x)   (x->flags_ &= ~NODE_SOURCE)
#define     IS_SOURCE(x)    ((x->flags_ & NODE_SOURCE) == NODE_SOURCE)

typedef enum tagNodeState {
    UNINITIALIZED = 0,
    INVALID, STRONG1, WEAK1, STRONG0, WEAK0, RATIOED, DOUBLE_WEAK1, DOUBLE_WEAK0
} NodeState;

class SimNode : public std::enable_shared_from_this<SimNode> {
public:
    SimNode(std::string u_id);
    ~SimNode();
    
    char GetNodeValue();
    std::list<std::shared_ptr<SimNode>> *BFSSearch();
    
    static std::shared_ptr<SimNode> GetNode(std::string node_name, std::list<std::shared_ptr<SimNode>> *list);
    NodeState current_state_, new_state_;
    const std::string node_name_;
    std::vector<std::weak_ptr<SimTransistor>> fanout_set_;
    std::vector<std::weak_ptr<SimTransistor>> input_set_;
    std::vector<std::weak_ptr<SimTransistor>> storage_set_;
    int flags_;
    
    int node_level_;
};

typedef std::shared_ptr<SimNode> SimNodePtr;

#endif /* SimNode_hpp */
