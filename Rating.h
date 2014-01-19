//
//  Rating.h
//  Skills
//
//  Created by KleMiX on 19/01/14.
//  Copyright (c) 2014 Vsevolod Klemetjev. All rights reserved.
//

#pragma once

#include "GaussianDistribution.h"

// default one is 3
#define DEFAULT_CONSERVATIVE_MULTIPLIER 3.0

struct Rating
{
    double conservativeMultiplier;
    double mean;
    double standardDeviation;
    
    Rating() : mean(0), standardDeviation(0), conservativeMultiplier(DEFAULT_CONSERVATIVE_MULTIPLIER) { }
    Rating(double mean, double standardDeviation) : mean(mean), standardDeviation(standardDeviation), conservativeMultiplier(DEFAULT_CONSERVATIVE_MULTIPLIER) { }
    Rating(double mean, double standardDeviation, double conservativeMultiplier) : mean(mean), standardDeviation(standardDeviation), conservativeMultiplier(conservativeMultiplier) { }
    
    // A conservative estimate of skill based on the mean and standard deviation
    double ConservativeRating()
    {
        return mean - conservativeMultiplier * standardDeviation;
    }
    
    static Rating GetPartialUpdate(Rating prior, Rating fullPosterior, double updatePercentage)
    {
        GaussianDistribution priorGaussian = GaussianDistribution(prior.mean, prior.standardDeviation);
        GaussianDistribution posteriorGaussian = GaussianDistribution(fullPosterior.mean, fullPosterior.standardDeviation);
        
        // From a clarification email from Ralf Herbrich:
        // "the idea is to compute a linear interpolation between the prior and posterior skills of each player
        //  ... in the canonical space of parameters"
        
        double precisionDifference = posteriorGaussian.precision - priorGaussian.precision;
        double partialPrecisionDifference = updatePercentage*precisionDifference;
        
        double precisionMeanDifference = posteriorGaussian.precisionMean - priorGaussian.precisionMean;
        double partialPrecisionMeanDifference = updatePercentage*precisionMeanDifference;
        
        GaussianDistribution partialPosteriorGaussion = GaussianDistribution::FromPrecisionMean(priorGaussian.precisionMean + partialPrecisionMeanDifference, priorGaussian.precision + partialPrecisionDifference);
        
        return Rating(partialPosteriorGaussion.mean, partialPosteriorGaussion.standardDeviation,
                          prior.conservativeMultiplier);
    }
};