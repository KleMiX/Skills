//
//  main.cpp
//  Skills
//
//  Created by KleMiX on 19/01/14.
//  Copyright (c) 2014 Vsevolod Klemetjev. All rights reserved.
//

#include <iostream>
#include "RatingCalculator.h"
#include "Rating.h"
#include "GaussianDistribution.h"

int main(int argc, const char * argv[])
{
    Rating r1 = Rating(25.0, 25.0 / 3.0);
    Rating r2 = Rating(25.0, 25.0 / 3.0);
    
    RatingCalculator::CalculateNewRatings(r1, r2, 1, 0);

    std::cout << "Rating a: " << r1.mean << ", " << r1.standardDeviation << std::endl;
    std::cout << "Rating b: " << r2.mean << ", " << r2.standardDeviation << std::endl;
    
    return 0;
}

