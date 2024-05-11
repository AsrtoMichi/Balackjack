#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>

char ranks[] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K'};
std::string suits[] = {"Spades", "Hearts", "Diamonds", "Clubs"};

class Card {
public:
    char rank;
    std::string suit;

    Card(char rank, std::string suit) : rank(rank), suit(suit) {}

    int card_value() {
        if (rank == 'J' || rank == 'Q' || rank == 'K') {
            return 10;
        }
        if (rank == 'A') {
            return 1; // In C++, we can't return two values. We'll handle the value of Ace in the game logic.
        }
        return rank - '0'; // Convert char to int
    }

    friend std::ostream& operator<<(std::ostream& os, const Card& card) {
        os << card.rank << "-" << card.suit;
        return os;
    }
};

class Deck {
public:
    std::vector<Card> cards;

    Deck() {
        generate_deck();
    }

    void generate_deck() {
        cards.clear();
        for (auto &suit : suits) {
            for (auto &rank : ranks) {
                cards.emplace_back(rank, suit);
            }
        }
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle(cards.begin(), cards.end(), std::default_random_engine(seed));
    }

    Card deal_card() {
        if (cards.empty()) {
            generate_deck();
        }
        Card dealt_card = cards.back();
        cards.pop_back();
        return dealt_card;
    }
};
