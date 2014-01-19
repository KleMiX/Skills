//
//  TruncatedGaussianCorrectionFunctions.h
//  Skills
//
//  Created by KleMiX on 19/01/14.
//  Copyright (c) 2014 Vsevolod Klemetjev. All rights reserved.
//

#pragma once

namespace TruncatedGaussianCorrectionFunctions
{
    double VExceedsMargin(double teamPerformanceDifference, double drawMargin);
    double VExceedsMargin(double teamPerformanceDifference, double drawMargin, double c);
    
    double WExceedsMargin(double teamPerformanceDifference, double drawMargin);
    double WExceedsMargin(double teamPerformanceDifference, double drawMargin, double c);
    
    double VWithinMargin(double teamPerformanceDifference, double drawMargin);
    double VWithinMargin(double teamPerformanceDifference, double drawMargin, double c);
    
    double WWithinMargin(double teamPerformanceDifference, double drawMargin);
    double WWithinMargin(double teamPerformanceDifference, double drawMargin, double c);
}