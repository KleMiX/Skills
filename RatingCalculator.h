//
//  RatingCalculator.h
//  Skills
//
//  Created by KleMiX on 19/01/14.
//  Copyright (c) 2014 Vsevolod Klemetjev. All rights reserved.
//

#pragma once

#include "Rating.h"

enum GameResult
{
    GAME_RESULT_LOST = -1,
    GAME_RESULT_DRAW = 0,
    GAME_RESULT_WON = 1,
};

namespace RatingCalculator
{
    double  CalculateMatchQuality(Rating player1, Rating player2);
    void    CalculateNewRatings(Rating& player1, Rating& player2, int rank1, int rank2);
}