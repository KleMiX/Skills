//
//  RatingCalculator.cpp
//  Skills
//
//  Created by KleMiX on 19/01/14.
//  Copyright (c) 2014 Vsevolod Klemetjev. All rights reserved.
//

#include "RatingCalculator.h"
#include "TruncatedGaussianCorrectionFunctions.h"

#include <stdio.h>
#include <math.h>

namespace GameInfo
{
    // Default game values
    const double initialMean = 25.0;
    const double drawProbability = 0.10;
    const double beta = initialMean / 6.0;
    const double dynamicsFactor = initialMean / 300.0;
    const double standardDeviation = initialMean / 3.0;
}

namespace RatingCalculator
{
    Rating  CalculateNewRating(Rating selfRating, Rating opponentRating, GameResult result);
    double  GetDrawMarginFromDrawProbability(double drawProbability, double beta);
}

void RatingCalculator::CalculateNewRatings(Rating& player1, Rating& player2, int rank1, int rank2)
{
    Rating& winner = rank1 > rank2 ? player1 : player2;
    Rating& loser = rank1 <= rank2 ? player1 : player2;
    
    Rating winnerPrevious = Rating(winner.mean, winner.standardDeviation, winner.conservativeMultiplier);
    Rating loserPrevious = Rating(loser.mean, loser.standardDeviation, loser.conservativeMultiplier);
    
    bool wasDraw = rank1 == rank2;
    
    winner = CalculateNewRating(winnerPrevious, loserPrevious, wasDraw ? GAME_RESULT_DRAW : GAME_RESULT_WON);
    loser = CalculateNewRating(loserPrevious, winnerPrevious, wasDraw ? GAME_RESULT_DRAW : GAME_RESULT_LOST);
}

Rating RatingCalculator::CalculateNewRating(Rating selfRating, Rating opponentRating, GameResult result)
{
    double drawMargin = GetDrawMarginFromDrawProbability(GameInfo::drawProbability, GameInfo::beta);
    
    double c = sqrt((selfRating.standardDeviation * selfRating.standardDeviation) + (opponentRating.standardDeviation * opponentRating.standardDeviation) + 2*(GameInfo::beta * GameInfo::beta));
    
    double winningMean = selfRating.mean;
    double losingMean = opponentRating.mean;

    if (result == GAME_RESULT_LOST)
    {
        winningMean = opponentRating.mean;
        losingMean = selfRating.mean;
    }
    
    double meanDelta = winningMean - losingMean;
    
    double v;
    double w;
    double rankMultiplier;
    
    if (result != GAME_RESULT_DRAW)
    {
        // non-draw case
        v = TruncatedGaussianCorrectionFunctions::VExceedsMargin(meanDelta, drawMargin, c);
        w = TruncatedGaussianCorrectionFunctions::WExceedsMargin(meanDelta, drawMargin, c);
        rankMultiplier = (int) result;
    }
    else
    {
        v = TruncatedGaussianCorrectionFunctions::VWithinMargin(meanDelta, drawMargin, c);
        w = TruncatedGaussianCorrectionFunctions::WWithinMargin(meanDelta, drawMargin, c);
        rankMultiplier = 1;
    }
    
    double meanMultiplier = ((selfRating.standardDeviation * selfRating.standardDeviation) + (GameInfo::dynamicsFactor * GameInfo::dynamicsFactor)) / c;
    
    double varianceWithDynamics = (selfRating.standardDeviation * selfRating.standardDeviation) + (GameInfo::dynamicsFactor * GameInfo::dynamicsFactor);
    double stdDevMultiplier = varianceWithDynamics / (c * c);
    
    double newMean = selfRating.mean + (rankMultiplier*meanMultiplier*v);
    double newStdDev = sqrt(varianceWithDynamics*(1 - w*stdDevMultiplier));
    
    return Rating(newMean, newStdDev);
}

double RatingCalculator::CalculateMatchQuality(Rating player1Rating, Rating player2Rating)
{
    // We just use equation 4.1 found on page 8 of the TrueSkill 2006 paper:
    double betaSquared = GameInfo::beta * GameInfo::beta;
    double player1SigmaSquared = player1Rating.standardDeviation * player1Rating.standardDeviation;
    double player2SigmaSquared = player2Rating.standardDeviation * player2Rating.standardDeviation;
    
    // This is the square root part of the equation:
    double sqrtPart = sqrt(
              (2*betaSquared)
              /
              (2*betaSquared + player1SigmaSquared + player2SigmaSquared));
    
    // This is the exponent part of the equation:
    double expPart = exp(
             (-1*((player1Rating.mean - player2Rating.mean) * (player1Rating.mean - player2Rating.mean)))
             /
             (2*(2*betaSquared + player1SigmaSquared + player2SigmaSquared)));
    
    return sqrtPart*expPart;
}

double RatingCalculator::GetDrawMarginFromDrawProbability(double drawProbability, double beta)
{
    // Derived from TrueSkill technical report (MSR-TR-2006-80), page 6
    
    // draw probability = 2 * CDF(margin/(sqrt(n1+n2)*beta)) -1
    
    // implies
    //
    // margin = inversecdf((draw probability + 1)/2) * sqrt(n1+n2) * beta
    // n1 and n2 are the number of players on each team
    double margin = GaussianDistribution::InverseCumulativeTo(.5*(drawProbability + 1), 0, 1) * sqrt(1 + 1) * beta;
    return margin;
}