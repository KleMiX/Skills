//
//  main.cpp
//  Skills
//
//  Created by KleMiX on 19/01/14.
//  Copyright (c) 2014 Vsevolod Klemetjev. All rights reserved.
//

#include <iostream>
#include "RatingCalculator.h"
#include "GaussianDistribution.h"

struct Player
{
    Rating r;
    int gamesWon;
    int gamesLost;
    int luck;
    
    Player()
    {
        r = Rating(25.0, 25.0 / 3.0);
        gamesLost = 0;
        gamesWon = 0;
        luck = rand() % 10;
    }
    
    void PrintInfo()
    {
         std::cout << "rating: " << r.mean << ", " << r.standardDeviation << ", total games: " << gamesLost + gamesWon << ", games won: " << gamesWon << ", games lost: " << gamesLost << std::endl;
    }
};

bool sortPlayers(const Player left, const Player right)
{
    double leftRating = left.r.ConservativeRating();
    double rightRating = right.r.ConservativeRating();
    
    if (leftRating == rightRating)
        return false;
    return leftRating > rightRating;
}

int main(int argc, const char * argv[])
{
    const int PLAYER_COUNT = 256;
    const int ATTACKS_COUNT = 100;
    Player players[PLAYER_COUNT];
    
    for (int i = 0; i < PLAYER_COUNT; i++)
    {
        players[i] = Player();
    }

    for (int i = 0; i < PLAYER_COUNT; i++)
    {
        for (int j = 0; j < ATTACKS_COUNT; j++) // every player will do max 15 attacks against some other players
        {
            Player& opponent = players[rand() % PLAYER_COUNT];
            if (&opponent != &players[i])
            {
                bool won = players[i].luck >= opponent.luck;//rand() % 2 == 1;

                if (won)
                {
                    players[i].gamesWon++;
                    opponent.gamesLost++;
                }
                else
                {
                    players[i].gamesLost++;
                    opponent.gamesWon++;
                }
                
                RatingCalculator::CalculateNewRatings(players[i].r, opponent.r, won ? 1 : 0, won ? 0 : 1);
            }
        }
    }
    
    std::sort(players, players + PLAYER_COUNT, sortPlayers);
    
    for (int i = 0; i < PLAYER_COUNT; i++)
    {
        std::cout << "Player " << i+1 << " (" << players[i].r.ConservativeRating() << "): ";
        players[i].PrintInfo();
    }
    
    return 0;
}

