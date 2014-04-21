/*Text-based Blackjack program. There is one player and one dealer.
The dealer hits until his hand value is 17 or greater.
The basic actions of hitting and standing are implemented.
The player starts with 100 chips and must bet at least
1 chip each hand.

Written by: Mauricio Roman
Date: 15 April 2014

Comments: This was written for submission to Insight Data Engineering
          Fellowship.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>      //We include this to seed random number generator
#include "stack.h"

#define DEBUG        0
#define NUMCARDS    52
#define MINCARDS    10  // No. cards left in stack before stopping game
#define NUMSHUFFLE   3
#define INIT_CHIPS 100

#define MAXLABEL   128

int chips = INIT_CHIPS;  //Initial chips in posession of player

struct cards {
  int shuffled; //Shuffled index
  int val;      //Default value of card
  int val2;     //Upper bound of value of card
  char *label;   //Pointer to array of labels, ex. "Ace"
  char *type;    //Pointer to array of types, ex. "Hearts"
} c[NUMCARDS+1];

const char *labels[] = {"Ace","Two","Three","Four","Five","Six","Seven",
                        "Eight","Nine","Ten","Jack","Queen","King"};

const char *types[] = {"Hearts","Spades","Diamonds","Clubs"};

void print_card(int x){
    printf("+ %s of %s", c[x].label, c[x].type);
    if(DEBUG){
        printf(" val=%d", c[x].val);
    }
    printf("\n");
}

int get_points(stack *s, int bound){
    int total_points=0, i;
    /* Calculate lower bound of points */
    if(bound==0){
        /* We iterate over the cards in the respective stack */
        for(i=0;i< s->count;i++){
            total_points += c[s->s[i]].val;
        }
    /* Calculate points taking upper bound of at most 1 ace */
    } else if(bound==1) {
        int no_aces=0;
        for(i=0;i<s->count;i++){
            if(no_aces==0 && c[s->s[i]].val == 1){
                total_points += c[s->s[i]].val + 10;
                no_aces++;
            } else {
                total_points += c[s->s[i]].val;
            }
        }
    }

    return total_points;
}

int main(){

int i=0, j=0, k=0, n=0;
int no_decks, index=0, bet=0, card=0, hidden_card=0, visible_card=0;
int player_points=0, dealer_points=0, points=0, points_up=0, points_lo=0;
int action=0;
stack s, p, d;         // Stack of cards for main stack, player, dealer
time_t t;

/* Intializes random number generator */
srand((unsigned) time(&t));

if(NUMCARDS % 52 != 0){
    printf("Number of cards must be a multiple of 52");
    exit(0);
}

no_decks = NUMCARDS / 52;
printf("Playing BlackJack with %d deck(s)\n", no_decks);

/* Create deck(s) of cards */
for (i=1;i<=no_decks;i++){
    for (j=1;j<=4;j++){
        for (k=1;k<=13;k++){
            index = (i-1)*52 + (j-1)*13 + k;
            c[index].shuffled = index;
            c[index].type = types[j-1];
            c[index].label = labels[k-1];
            c[index].val = k;
            if(c[index].val>10){  // Limit value to 10 for higher cards
                c[index].val=10;
            }
        }
    }
}

/* Shuffle deck (Knuth-Fisher-Yates shuffle algorithm)
   We will shuffle the deck NUMSHUFFLE times */

for(j=1; j<= NUMSHUFFLE; j++){
    for (i = NUMCARDS; i > 1; i--){
        n = rand() % (i + 1) + 1;
        /* Swap cards */
        int tmp;
        tmp = c[i].shuffled;
        c[i].shuffled = c[n].shuffled;
        c[n].shuffled = tmp;
    }
}
printf("The deck was shuffled %d times\n", NUMSHUFFLE);

/* Place shuffled cards on dealers stack */
init_stack(&s);
for(i=1;i<=NUMCARDS;i++){
    push(&s,c[i].shuffled);
}

/* Remove cards in the context of game -- stop if deck falls
   below 12 cards, or chips run out */
while(stack_size(&s) > MINCARDS && chips > 0){
    /* Request bet */
    printf("\nYou have %d chips\n", chips);
    printf("\nPlease place your bet (no. chips), or -1 to cash out: ");
    scanf ("%d", &bet);
    printf("\n");

    /* Check if bet is within bounds of chips available */
    if(bet < 1 ){
        printf("Thank you for playing Black Jack\n");
        printf("You are cashing out with %d chips\n", chips);
        exit(0);
    } else if(bet > chips){
        printf("You don't have enough chips for this bet!\n");
    }
        else {
        chips -= bet;
        printf("\nYou now have %d chips\n", chips);

        /* Deal cards to dealer */
        init_stack(&d);

        hidden_card = pop(&s); push(&d, hidden_card);
        visible_card = pop(&s);push(&d, visible_card);

        printf("\nDealer's Hand:\n\n");
        printf("** hidden card **\n");
        print_card(visible_card);

        /* Deal first 2 cards to player */
        init_stack(&p);

        printf("\nYour Hand:\n\n");

        card = pop(&s); print_card(card); push(&p, card);
        card = pop(&s); print_card(card); push(&p, card);

        /* Get player decision: hit or stand? */
        int player_done=0;
        while (!player_done){
            printf("\nWhat would you like to do? (1=stand 2=hit):");
            scanf ("%d", &action);
            printf("\n");
            if(action == 1){
               player_done = 1;
            } else if(action == 2) {
                card = pop(&s); print_card(card); push(&p, card);
                if(DEBUG){
                    printf("Printing stack\n");
                    print_stack(&p);
                }
                points = get_points(&p,0);
                /* We force stop when the lower bound of points >= 21 */

                if(points > 21){
                    player_done=1;
                    printf("\nYou have %d points -- you bust!\n", points);
                } else {
                    printf("\nYou have (at least) %d points\n", points);
                }
            } else {
                printf("Action not allowed - select 1 or 2\n");
            }
        }

        /* Assume upper bound */
        player_points = get_points(&p,1);
        /* If exceeds limit, take lower bound */
        if(player_points > 21){
            player_points = get_points(&p,0);
        }

        /* If player's hand is viable, dealer will proceed */
        if(player_points<=21){
            printf("\nYou have %d points\n", player_points);

            printf("\nDealer will now proceed...\n");
            printf("\nDealer's hand:\n\n");
            print_card(hidden_card);
            print_card(visible_card);

            int dealer_done=0;
            points = 0;
            while (!dealer_done){
                points_lo = get_points(&d,0);      //Lower bound
                points_up = get_points(&d,1);   //Upper bound
                /* Dealer hits until its points (lower bound) are 17 or
                higher, or if he is sure to at least break even, or if
                he has 21 points with an ace */
                if(points_lo>=17 || points_lo>= player_points ||
                  (points_up >= player_points && points_up <= 21)
                   || points_up==21){
                    dealer_done = 1;
                } else {
                    card = pop(&s); print_card(card); push(&d, card);
                }
            }

            /* Assume upper bound */
            dealer_points = get_points(&d,1);
            /* If exceeds limit, take lower bound */
            if(dealer_points > 21){
                dealer_points = get_points(&d,0);
            }
            printf("\nDealer has %d points...\n\n", dealer_points);
            /* Assign chips according to outcome */
            if(player_points > dealer_points || dealer_points > 21){
                /* Recover bet, and win the amount that player bet */
                chips += 2*bet;
                if(dealer_points > 21){
                    printf("Dealer busts!\n");
                }
                printf("You win!\n");
            } else if (player_points == dealer_points){
                /* Just recover bet */
                chips += bet;
                printf("Tie...\n");
            } else {
                /* Loose bet */
                printf("You loose!\n");
            }
        } // end if
    } // end else
} // end while
if(stack_size(&s) <= MINCARDS){
    printf("We are running out of cards...\n");
}
if(chips==0){
    printf("You have run out of chips!\n");
}
printf("\nThank you for playing BlackJack!\n");
return 0;
}
