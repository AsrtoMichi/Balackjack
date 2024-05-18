#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Arduino.h>
using namespace std;

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo1;
Servo servo2;
const int buttonPinHit = 10;
const int buttonPinStay = 9;

class Card {
public:
    char rank;
    char suit;

    Card() {}

    Card(char rank, char suit) : rank(rank), suit(suit) {}

    int card_value() const {
        if (rank == 'J' || rank == 'Q' || rank == 'K' || rank == 'T') {
            return 10;
        }
        if (rank == 'A') {
            return 1;
        }
        return rank - '0';
    }
};

class Deck {
private:
    Card cards[52];
    int top_card_index;

public:
    Deck() {
        generate_deck();
        top_card_index = 51;
    }

    void generate_deck() {
        int card_index = 0;

        char suits[] = {'S', 'H', 'D', 'C'};
        char ranks[] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};

        for (int s = 0; s < 4; ++s) {
            for (int r = 0; r < 13; ++r) {
                cards[card_index++] = Card(ranks[r], suits[s]);
            }
        }
        shuffle_deck();
    }

    void shuffle_deck() {
        for (int i = 51; i > 0; --i) {
            int j = random(0, i + 1);
            Card temp = cards[i];
            cards[i] = cards[j];
            cards[j] = temp;
        }
    }

    Card deal_card() {
        if (top_card_index < 0) {
            generate_deck();
            top_card_index = 51;
        }
        return cards[top_card_index--];
    }
};

class Hand {
private:
    Deck deck;
    Card cards[10];

public:
    bool soft;
    int card_count;

    Hand() {
        card_count = 0;
        cards[card_count++] = deck.deal_card();
        cards[card_count++] = deck.deal_card();
        soft = false;
    }

    int value() {
        int total = 0;
        int ace_count = 0;

        for (int i = 0; i < card_count; ++i) {
            if (cards[i].rank == 'A') {
                ace_count++;
            } else {
                total += cards[i].card_value();
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
        if (card_count < 10) {
            cards[card_count++] = deck.deal_card();
        }
    }

    String first_card() {
        if (card_count > 0) {
            return String(cards[0].rank) + cards[0].suit + " ";
        }
        return "";
    }

    String hand() {
        String hand;
        for (int i = 0; i < card_count; ++i) {
            hand += cards[i].rank;
            hand += cards[i].suit;
            hand += " ";
        }
        return hand;
    }
};

class Participant {
public:
    bool state = true; 
    bool has_stayed = false; 
    Hand hand;

    void hit() {
        hand.draw();
        check_bust();
    }

    void stay() {
        has_stayed = true;
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
    String hand_view() {
        return hand.hand();
    }
};

class Dealer : public Participant {
public:
    void dealer_strategy() {
        if (hand.value() < 17 || (hand.value() == 17 && hand.soft && hand.card_count < 3)) {
            hit();
        } else {
            stay();
        }
    }

    String card_up() {
        return hand.first_card();
    }
};

class Game {
private:
    Dealer dealer;
    Player player;
    Deck deck;
    bool button_action(int pin) {
        if (digitalRead(pin) == LOW) {
            delay(100);
            return true;
        }
        return false;
    }

public:
    Game() : dealer(), player(), deck() {}

    void player_turn() {
        lcd.setCursor(0, 0);
        lcd.print(player.hand_view());
        int totalValue = player.hand.value();
        lcd.setCursor(14, 0);
        if (totalValue < 10) {
            lcd.print(" ");
        }
        lcd.print(totalValue);
        if (button_action(buttonPinHit)) {
            player.hit();
            Serial.println("Player hits");
        } else if (button_action(buttonPinStay)) {
            player.stay();
            Serial.println("Player stays");
        }
        player.check_bust();
    }

    bool check_win() {
        dealer.check_bust();
        player.check_bust();

        if (!player.state && dealer.state)
            return false;

        if (player.state && !dealer.state)
            return true;

        if (player.state && dealer.state) {
            if (dealer.hand.value() > player.hand.value()) {
                return false;
            } else if (dealer.hand.value() < player.hand.value()) {
                return true;
            } else {
                return false;
            }
        }

        if (player.has_stayed && dealer.has_stayed) {
            if (dealer.hand.value() > player.hand.value()) {
                return false;
            } else if (dealer.hand.value() < player.hand.value()) {
                return true;
            } else {
                return false;
            }
        }
        return false;
    }

    void run() {
        lcd.setCursor(0, 1);
        lcd.print(dealer.card_up());
        while ((player.state && !player.has_stayed) || (dealer.state && !dealer.has_stayed)) {
            if (player.state && !player.has_stayed) {
                player_turn();
            }
            if (dealer.state && !dealer.has_stayed) {
                dealer.dealer_strategy();
                Serial.println("Dealer strategy");
            }
            delay(500);
        }
        delay(1000);
        lcd.setCursor(0, 0);
        lcd.print("Result:         ");
        lcd.setCursor(0, 1);
        if (check_win()) {
            lcd.print("Player wins!    ");
        } else {
            lcd.print("Dealer wins!    ");
        }
        delay(3000);
    }
};

void setup() {
    lcd.init();
    lcd.backlight();
    Serial.begin(9600);
    pinMode(9, INPUT_PULLUP);
    pinMode(10, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    servo1.attach(2);
    servo2.attach(3);
    randomSeed(analogRead(0));
}

void loop() {
    bool button_hit = digitalRead(10);
    bool button_stay = digitalRead(9);
    bool coin_state = digitalRead(5);

    if (coin_state == 1) {
        lcd.setCursor(0, 0);
        lcd.print("INSERIRE  MONETA");
        delay(1000);
        lcd.clear();
        delay(500);
    } else {
        delay(750);
        lcd.setCursor(0, 0);
        lcd.print("   ISTRUZIONI   ");
        lcd.setCursor(0, 1);
        lcd.print("Sinistra = PESCA");
        delay(3000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("   ISTRUZIONI   ");
        lcd.setCursor(0, 1);
        lcd.print(" Destra = PASSA");
        delay(3000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("    Formato:     ");
        lcd.setCursor(0, 1);
        lcd.print("Carta Seme -> CS");
        delay(3000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Carte Giocatore");
        lcd.setCursor(0, 1);
        lcd.print("Carte del Dealer");
        delay(3000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("   ISTRUZIONI   ");
        lcd.setCursor(0, 1);
        lcd.print("2 tasti = INIZIO");
        while (!(button_hit == LOW && button_stay == LOW)) {
            button_hit = digitalRead(10);
            button_stay = digitalRead(9);
            delay(50);
        }
        lcd.clear();
        delay(250);
        Game game;
        game.run();
    }

    delay(500);
}
