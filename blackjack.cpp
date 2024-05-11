#include <iostream>
#include <string>

class Card {
private:
    std::string rank;
    std::string suit;

public:
    // Constructor
    Card(const std::string& rank, const std::string& suit) : rank(rank), suit(suit) {}

    // Evaluate the value of a card
    int card_value() {
        // If the rank is a king, queen, or jack, return 10 as the value of the card.
        if (rank == "J" || rank == "Q" || rank == "K") {
            return 10;
        }
        // If the rank is an ace, return 1 and 11. This will later let us choose whether we want the ace to be a one or eleven.
        if (rank == "A") {
            return 1; // You can handle the choice of 1 or 11 elsewhere in your game logic.
        }
        // If it's nothing special, just return the numeric value.
        return std::stoi(rank);
    }

    // Overloaded stream insertion operator for printing the card
    friend std::ostream& operator<<(std::ostream& os, const Card& card) {
        os << card.rank << "-" << card.suit;
        return os;
    }
};

int main() {
    // Example usage
    Card myCard("A", "Hearts");
    std::cout << "Card value: " << myCard.card_value() << std::endl;
    std::cout << "Card details: " << myCard << std::endl;
    return 0;
}
