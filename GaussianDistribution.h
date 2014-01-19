//
//  GaussianDistribution.h
//  Skills
//
//  Created by KleMiX on 19/01/14.
//  Copyright (c) 2014 Vsevolod Klemetjev. All rights reserved.
//

#pragma once

#include <math.h>
#include <algorithm>

struct GaussianDistribution
{
    double mean;
    double standardDeviation;
    
    double precision;
    double precisionMean;
    double variance;
    
    GaussianDistribution() : mean(0), standardDeviation(0), precision(0), precisionMean(0), variance(0) { }
    
    GaussianDistribution(double mean, double standardDeviation) : mean(mean), standardDeviation(standardDeviation)
    {
        variance = standardDeviation * standardDeviation;
        precision = 1.0 / variance;
        precisionMean = precision * mean;
    }
    
    double NormalizationConstant()
    {
        return 1.0 / (sqrt(2.0 * M_PI) * standardDeviation);
    }
    
    static GaussianDistribution FromPrecisionMean(double precisionMean, double precision)
    {
        GaussianDistribution result = GaussianDistribution();
        result.precision = precision;
        result.precisionMean = precisionMean;
        result.variance = 1.0 / precision;
        result.standardDeviation = sqrt(result.variance);
        result.mean = precisionMean / precision;
        
        return result;
    }
    
    static double LogProductNormalization(GaussianDistribution left, GaussianDistribution right)
    {
        if ((left.precision == 0) || (right.precision == 0))
        {
            return 0;
        }
        
        double varianceSum = left.variance + right.variance;
        double meanDifference = left.mean - right.mean;
        
        double logSqrt2Pi = log(sqrt(2.0 * M_PI));
        return -logSqrt2Pi - (log(varianceSum)/2.0) - ((meanDifference*meanDifference)/(2.0*varianceSum));
    }
    
    static double LogRatioNormalization(GaussianDistribution numerator, GaussianDistribution denominator)
    {
        if ((numerator.precision == 0) || (denominator.precision == 0))
        {
            return 0;
        }
        
        double varianceDifference = denominator.variance - numerator.variance;
        double meanDifference = numerator.mean - denominator.mean;
        
        double logSqrt2Pi = log(sqrt(2*M_PI));
        
        return log(denominator.variance) + logSqrt2Pi - log(varianceDifference)/2.0 + (meanDifference*meanDifference)/(2*varianceDifference);
    }
    
    static double At(double x)
    {
        return At(x, 0, 1);
    }
    
    static double At(double x, double mean, double standardDeviation)
    {
        // See http://mathworld.wolfram.com/NormalDistribution.html
        //                1              -(x-mean)^2 / (2*stdDev^2)
        // P(x) = ------------------- * e
        //        stdDev * sqrt(2*pi)
        
        double multiplier = 1.0/(standardDeviation * sqrt(2.0 * M_PI));
        double expPart = exp((-1.0*pow(x - mean, 2.0))/(2*(standardDeviation*standardDeviation)));
        double result = multiplier*expPart;
        return result;
    }
    
    static double CumulativeTo(double x, double mean, double standardDeviation)
    {
        double invsqrt2 = -0.707106781186547524400844362104;
        double result = ErrorFunctionCumulativeTo(invsqrt2*x);
        return 0.5*result;
    }
    
    static double CumulativeTo(double x)
    {
        return CumulativeTo(x, 0, 1);
    }
    
    static double ErrorFunctionCumulativeTo(double x)
    {
        // Derived from page 265 of Numerical Recipes 3rd Edition
        double z = fabs(x);
        
        double t = 2.0/(2.0 + z);
        double ty = 4*t - 2;
        
        double coefficients[28] = {
            -1.3026537197817094, 6.4196979235649026e-1,
            1.9476473204185836e-2, -9.561514786808631e-3, -9.46595344482036e-4,
            3.66839497852761e-4, 4.2523324806907e-5, -2.0278578112534e-5,
            -1.624290004647e-6, 1.303655835580e-6, 1.5626441722e-8, -8.5238095915e-8,
            6.529054439e-9, 5.059343495e-9, -9.91364156e-10, -2.27365122e-10,
            9.6467911e-11, 2.394038e-12, -6.886027e-12, 8.94487e-13, 3.13092e-13,
            -1.12708e-13, 3.81e-16, 7.106e-15, -1.523e-15, -9.4e-17, 1.21e-16, -2.8e-17
        };
        
        int ncof = 28;
        double d = 0.0;
        double dd = 0.0;
        
        
        for (int j = ncof - 1; j > 0; j--)
        {
            double tmp = d;
            d = ty*d - dd + coefficients[j];
            dd = tmp;
        }
        
        double ans = t*exp(-z*z + 0.5*(coefficients[0] + ty*d) - dd);
        return x >= 0.0 ? ans : (2.0 - ans);
    }
    
    
    static double InverseErrorFunctionCumulativeTo(double p)
    {
        // From page 265 of numerical recipes
        
        if (p >= 2.0)
        {
            return -100;
        }
        if (p <= 0.0)
        {
            return 100;
        }
        
        double pp = (p < 1.0) ? p : 2 - p;
        double t = sqrt(-2*log(pp/2.0)); // Initial guess
        double x = -0.70711*((2.30753 + t*0.27061)/(1.0 + t*(0.99229 + t*0.04481)) - t);
        
        for (int j = 0; j < 2; j++)
        {
            double err = ErrorFunctionCumulativeTo(x) - pp;
            x += err/(1.12837916709551257*exp(-(x*x)) - x*err); // Halley
        }
        
        return p < 1.0 ? x : -x;
    }
    
    static double InverseCumulativeTo(double x, double mean, double standardDeviation)
    {
        // From numerical recipes, page 320
        return mean - sqrt(2)*standardDeviation*InverseErrorFunctionCumulativeTo(2*x);
    }
    
    static double InverseCumulativeTo(double x)
    {
        return InverseCumulativeTo(x, 0, 1);
    }
    
    friend GaussianDistribution operator * (const GaussianDistribution left, const GaussianDistribution right)
    {
        return FromPrecisionMean(left.precisionMean + right.precisionMean, left.precision + right.precision);
    }
    
    friend double operator - (const GaussianDistribution left, const GaussianDistribution right)
    {
        double a = fabs(left.precisionMean - right.precisionMean);
        double b = sqrt(fabs(left.precision - right.precision));
        return a > b ? a : b;
    }
    
    friend GaussianDistribution operator /(const GaussianDistribution numerator, const  GaussianDistribution denominator)
    {
        return FromPrecisionMean(numerator.precisionMean - denominator.precisionMean,
                                 numerator.precision - denominator.precision);
    }
};