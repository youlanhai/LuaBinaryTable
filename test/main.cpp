//
//  main.cpp
//  LuaBinaryTable
//
//  Created by youlanhai on 15/12/27.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#include <iostream>
#include <cmath>
#include <climits>

int main(int argc, char **argv)
{
    double value = -1.2;
    std::cout << value << " " << std::floor(value) << " " << std::round(value) << std::endl;
    
    std::cout << "int max" << std::numeric_limits<int>::max() << std::endl;
    std::cout << "int min" << std::numeric_limits<int>::min() << std::endl;
    
    std::cout << "float max:" << std::numeric_limits<float>::max() << std::endl;
    std::cout << "float min:" << std::numeric_limits<float>::min() << std::endl;
    
    std::cout << "double max:" << std::numeric_limits<double>::max() << std::endl;
    std::cout << "double min:" << std::numeric_limits<double>::min() << std::endl;
    return 0;
}
