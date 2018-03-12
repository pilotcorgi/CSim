//
//  Solver.hpp
//  CSim
//
//  Created by Fangzhou Wang on 11/2/17.
//  Copyright © 2017 Computer Aided Discovery. All rights reserved.
//

#ifndef Solver_hpp
#define Solver_hpp

#include <list>
#include <set>
#include "SimNode.hpp"

class Solver {
public:
    Solver();
    virtual ~Solver();
    
    virtual int UpdateValue(std::list<SimNodePtr> *node_list, std::set<SimNodePtr> &changed_list) = 0;
    int UpdateTransistor(SimNodePtr node, std::set<SimNodePtr> &changed_list);
    NodeState ComputeNewValue(std::list<SimNodePtr> *node_list);
};

#endif /* Solver_hpp */
