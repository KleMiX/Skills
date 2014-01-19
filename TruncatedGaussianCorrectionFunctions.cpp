//
//  TruncatedGaussianCorrectionFunctions.cpp
//  Skills
//
//  Created by KleMiX on 19/01/14.
//  Copyright (c) 2014 Vsevolod Klemetjev. All rights reserved.
//

#include "TruncatedGaussianCorrectionFunctions.h"
#include "GaussianDistribution.h"

double TruncatedGaussianCorrectionFunctions::VExceedsMargin(double teamPerformanceDifference, double drawMargin)
{
    double denominator = GaussianDistribution::CumulativeTo(teamPerformanceDifference - drawMargin);
    
    if (denominator < 2.222758749e-162)
    {
        return -teamPerformanceDifference + drawMargin;
    }
    
    return GaussianDistribution::At(teamPerformanceDifference - drawMargin)/denominator;
}

double TruncatedGaussianCorrectionFunctions::VExceedsMargin(double teamPerformanceDifference, double drawMargin, double c)
{
    return VExceedsMargin(teamPerformanceDifference/c, drawMargin/c);
    //return GaussianDistribution::At((teamPerformanceDifference - drawMargin) / c) / GaussianDistribution.CumulativeTo((teamPerformanceDifference - drawMargin) / c);
}

double TruncatedGaussianCorrectionFunctions::WExceedsMargin(double teamPerformanceDifference, double drawMargin)
{
    double denominator = GaussianDistribution::CumulativeTo(teamPerformanceDifference - drawMargin);
    
    if (denominator < 2.222758749e-162)
    {
        if (teamPerformanceDifference < 0.0)
        {
            return 1.0;
        }
        return 0.0;
    }
    
    double vWin = VExceedsMargin(teamPerformanceDifference, drawMargin);
    return vWin*(vWin + teamPerformanceDifference - drawMargin);
}

double TruncatedGaussianCorrectionFunctions::WExceedsMargin(double teamPerformanceDifference, double drawMargin, double c)
{
    return WExceedsMargin(teamPerformanceDifference/c, drawMargin/c);
    //var vWin = VExceedsMargin(teamPerformanceDifference, drawMargin, c);
    //return vWin * (vWin + (teamPerformanceDifference - drawMargin) / c);
}

double TruncatedGaussianCorrectionFunctions::VWithinMargin(double teamPerformanceDifference, double drawMargin)
{
    double teamPerformanceDifferenceAbsoluteValue = fabs(teamPerformanceDifference);
    double denominator =
    GaussianDistribution::CumulativeTo(drawMargin - teamPerformanceDifferenceAbsoluteValue) -
    GaussianDistribution::CumulativeTo(-drawMargin - teamPerformanceDifferenceAbsoluteValue);
    if (denominator < 2.222758749e-162)
    {
        if (teamPerformanceDifference < 0.0)
        {
            return -teamPerformanceDifference - drawMargin;
        }
        
        return -teamPerformanceDifference + drawMargin;
    }
    
    double numerator = GaussianDistribution::At(-drawMargin - teamPerformanceDifferenceAbsoluteValue) -
    GaussianDistribution::At(drawMargin - teamPerformanceDifferenceAbsoluteValue);
    
    if (teamPerformanceDifference < 0.0)
    {
        return -numerator/denominator;
    }
    
    return numerator/denominator;
}

// the additive correction of a double-sided truncated Gaussian with unit variance
double TruncatedGaussianCorrectionFunctions::VWithinMargin(double teamPerformanceDifference, double drawMargin, double c)
{
    return VWithinMargin(teamPerformanceDifference/c, drawMargin/c);
    //var teamPerformanceDifferenceAbsoluteValue = Math.Abs(teamPerformanceDifference);
    //return (GaussianDistribution.At((-drawMargin - teamPerformanceDifferenceAbsoluteValue) / c) - GaussianDistribution.At((drawMargin - teamPerformanceDifferenceAbsoluteValue) / c))
    //       /
    //       (GaussianDistribution.CumulativeTo((drawMargin - teamPerformanceDifferenceAbsoluteValue) / c) - GaussianDistribution.CumulativeTo((-drawMargin - teamPerformanceDifferenceAbsoluteValue) / c));
}

double TruncatedGaussianCorrectionFunctions::WWithinMargin(double teamPerformanceDifference, double drawMargin)
{
    double teamPerformanceDifferenceAbsoluteValue = fabs(teamPerformanceDifference);
    double denominator = GaussianDistribution::CumulativeTo(drawMargin - teamPerformanceDifferenceAbsoluteValue)
    -
    GaussianDistribution::CumulativeTo(-drawMargin - teamPerformanceDifferenceAbsoluteValue);
    
    if (denominator < 2.222758749e-162)
    {
        return 1.0;
    }
    
    double vt = VWithinMargin(teamPerformanceDifferenceAbsoluteValue, drawMargin);
    
    return vt*vt +
    (
     (drawMargin - teamPerformanceDifferenceAbsoluteValue)
     *
     GaussianDistribution::At(
                              drawMargin - teamPerformanceDifferenceAbsoluteValue)
     - (-drawMargin - teamPerformanceDifferenceAbsoluteValue)
     *
     GaussianDistribution::At(-drawMargin - teamPerformanceDifferenceAbsoluteValue))/denominator;
}

// the multiplicative correction of a double-sided truncated Gaussian with unit variance
double TruncatedGaussianCorrectionFunctions::WWithinMargin(double teamPerformanceDifference, double drawMargin, double c)
{
    return WWithinMargin(teamPerformanceDifference/c, drawMargin/c);
}