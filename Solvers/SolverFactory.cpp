//
//  SolverFactory.cpp
//  CSim
//
//  Created by Fangzhou Wang on 11/2/17.
//  Copyright © 2017 Computer Aided Discovery. All rights reserved.
//

#include "SolverFactory.hpp"

#include <iostream>

#include "AccurateSolver.hpp"
#include "WeakPreciseSolver.hpp"

using namespace std;

Solver* SolverFactory::getSolver(int precision_level) {
    if (precision_level == 1) {
        return new AccurateSolver();
    } else if (precision_level == 2) {
        return new WeakPreciseSolver();
    } else {
        cerr << "ERROR: Precision level " << precision_level << " is not defined." << endl;
        exit(1);
    }
}
