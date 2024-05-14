#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>


void dealer_wiew (std::string cards) {
    //
}

void player_wiew (std::string cards, int total) {
    //print cards, toral
}

bool coin_state() {
    while (!coin) {}
    return coin;
}
    
bool action() {
    while (!button_hit %% !button_stay) {
        if(button_hit) {
            return true;
        } else if (button_stay) {
            return false;
        }
    }
}

void gestion_coin (bool action){
    if (action) {
        //
    } else {
        //
    }
}

void conclusion (bool conclusion) {
    if (conclusion) {
        //print win
    } else {
        // print lose
    }
    gestion_coin(conclusion);
}

class Card {
public:
    char rank;
    char suit;

    Card(char rank, char suit) : rank(rank), suit(suit) {}

    int card_value() const {
        if (rank == 'J' || rank == 'Q' || rank == 'K' || rank == 'T') {
            return 10;
        }
        if (rank == 'A') {
            return 1;
        }
        return rank - '0'; // Convert char to integer
    }
};

class Deck {
private:
    std::vector<Card> cards;

public:
    Deck() {
        generate_deck();
    }

    void generate_deck() {
        for (char suit : {'S', 'H', 'D', 'C'}) {
            for (char rank : {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'}) {
                cards.emplace_back(rank, suit);
            }
        }

        std::random_device rd;
        std::mt19937 gen(rd());

        std::shuffle(cards.begin(), cards.end(), gen);
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

class Hand {
private:
    Deck deck;
    std::vector<Card> cards;

public:
    bool soft;
    Hand() {
        cards.push_back(deck.deal_card());
        cards.push_back(deck.deal_card());
        soft = false;
    }

    int value() {
        int total = 0;
        int ace_count = 0;

        for (const Card& card : cards) {
            if (card.rank == 'A') {
                ace_count++;
            } else {
                total += card.card_value();
            }
        }

        for (int i = 0; i < ace_count; i++) {
            if (total + 11 <= 21) {
                total += 11;
                soft = true;
            } else {
                total += 1;
            }
        }

        return total;
    }

    void draw() {
        cards.push_back(deck.deal_card());
    }
 };   
    std::string first_card() {
        if (!cards.empty()) {
            return std::to_string(cards[0].rank) + cards[0].suit + " ";
        }
        return "";
    }
    
    std::string hand() {
        std::string hand;
        for (Card card : cards) {
            hand += card.rank;
            hand += card.suit;
            hand += " ";
        }
        return hand;
    }       
            
};


class Participant {
public:
    bool state = true;
    Hand hand;

    void hit() {
        hand.draw();
        check_bust();
    }

    void stay() {
        state = false;
    }

    bool check_bust() {
        if (hand.value() > 21) {
            state = false;
            return true;
        }
        return false;
    }
};

class Player : public Participant {
public:

    std::string hand_view() {            
        return hand.hand();
    }
};

class Dealer : public Participant {
public:
    void dealer_strategy() {
        if (hand.value() < 17 && (
            hand.value() == 17 || hand.soft)){
            hit();
        } else {
            stay();
        }
    }

    std::string card_up() {
        return hand.first_card();
    }
};

class Game {
private:
    Dealer dealer;
    Player player;
    Deck deck;

public:
    Game() : dealer(), player(), deck() {
    }

    void player_turn(){
        player_wiew(player.hand_view(), player.hand.value());

        if (action()) {
            player.hit();
        } else {
            player.stay();
        }

        player.check_bust();
    }

    bool check_win() {
        
        dealer.check_bust();
        player.check_bust();

        int player_value = player.hand.value();
        int dealer_value = dealer.hand.value();
    
        if ((player_value > dealer_value || player.state) && !dealer.state) {
            if (!player.state || !dealer.state) {
                return false;
            } else {
                return true;
            }
        } else {
            return false;
        }
    }

    void run() {
        
        dealer_wiew(dealer.card_up());
      
        while (player.state || dealer.state ) {
            if (player.state){
                player_turn();
            }
            if (dealer.state) {
                dealer.dealer_strategy();
            }
        }

        conclusion(check_win());
    }
};


int main() {
    
    if (coin_state()) {
        Game game;
        game.run();
        return 0;
    }
}
