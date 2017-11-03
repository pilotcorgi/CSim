//
//  SolverFactory.hpp
//  CSim
//
//  Created by Fangzhou Wang on 11/2/17.
//  Copyright © 2017 Computer Aided Discovery. All rights reserved.
//

#ifndef SolverFactory_hpp
#define SolverFactory_hpp

#include "Solver.hpp"

class SolverFactory {
public:
    Solver* getSolver(int precision_level);
};

#endif /* SolverFactory_hpp */
