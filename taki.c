// Name: Dana Haham. ID: 209278407

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Sizes of strings
#define MAX_NAME 21
#define MAX_KIND_STR 6
#define MAX_NUM_STR 2
#define DIR_STR_LEN 3

// Amount of cards
#define CARD_NUM 14
#define COLOR_NUM 4
#define SPECIAL_CARDS_NUM 5

// Minimal setting
#define START_GAME_CARDS 4
#define MIN_PLAYERS_NUM 2

// Special cards 
#define PLUS_CARD 9
#define STOP_CARD 10
#define CHANGE_DIR_CARD 11
#define TAKI_CARD 12
#define CHANGE_COLOR_CARD 13

// Color cards
#define YELLOW_CARD 0
#define RED_CARD 1
#define BLUE_CARD 2
#define GREEN_CARD 3
#define NO_COLOR_CARD 4

// General consts
#define TAKE_CARD_FROM_DECK 0
#define FIRST_INDEX 0


// Structs
typedef struct Card
{
	int kind;
	int color;
} CARD;

typedef struct player
{
	char name[MAX_NAME];

	// Hold a dynamic amount of cards
	CARD* cards; 
	int logSizeCards;
	int physSizeCards;

} PLAYER;

typedef struct cardStatistics
{
	int kindCard;
	int count;

} CARD_STATISTICS;

typedef struct game
{
	CARD upperCard;

	// Hold a dynamic amount of palyers. Change only once.
	PLAYER* players;
	int sizePlayers;

	// Card-statistic's Data 
	CARD_STATISTICS cardStatistics[CARD_NUM];
	int logSizeStatis;

	// Special card flags
	int turnsDir;
	bool isWaterfall;

} GAME;

// Functions
void initGame(GAME* pGame);
int playGame(GAME* pGame);
void endGame(GAME game, int winnerPlayer);

int scanMove(int sizeCards);
int scanColorChoice();

bool playTurn(GAME* pGame, int currentPlayer, int cardChoice);
bool endTurn(GAME* pGame, int currentPlayer);

int nextTurn(GAME* pGame, int currentPlayer, int cardChoice);
int moveToNextPlayer(int currentPlayer, int turnsDir, int numPlayers);
int moveSpecialCard(int currentPlayer, int* pTurnsDir, int kindCard, int numPlayers);

bool putCardInDeck(GAME* pGame, int currentPlayer, int cardNumChoice);
void takeCardFromDeck(PLAYER* pPlayer, CARD_STATISTICS cardStatistics[]);
void removeCardFromArray(PLAYER* pPlayer, int cardNumChoice);

void printCard(CARD card);
void printPlayerCards(PLAYER player);

int initPlayers(PLAYER** players, CARD_STATISTICS cardStatistics[]);
initPlayer(PLAYER* pPlayer, int numPlayer, CARD_STATISTICS cardStatistics[]);
CARD initCard(int rangeCrads, CARD_STATISTICS cardStatistics[]);

char getColorCard(int colorNum);
void getKindCard(int kindNum, char strKind[]);

void rellocCards(PLAYER* pPlayer);
void mallocFail(void* pointer);
void freeMemoryGame(GAME* pGame);

void sortStatisArray(CARD_STATISTICS arr[], int size);
CARD_STATISTICS* mergeStatisArray(CARD_STATISTICS arr1[], int size1, CARD_STATISTICS arr2[], int size2);
void copyStatisArray(CARD_STATISTICS dest[], CARD_STATISTICS src[], int size);


void main()
{
	GAME game;
	int winnerPlayer;

	printf("************  Welcome to TAKI game !!! ***********\n\n");

	// Initial all the variables of a new game
	initGame(&game);

	// Play the game (All the turns till the game is over) and return the winner player
	winnerPlayer = playGame(&game);

	// Summarize the results of the game and print them
	endGame(game, winnerPlayer);

	//Free all the memory befor end the program
	freeMemoryGame(&game);
}

// Initial the all game. 
// Initial all the variables of a given game-struct inside the adress pGame  
// Assum that the phisycal size of the array cardStatistics is large enough
// Creat a new dynamic arrays -player and cards and dosent free them! Must happend after use.
void initGame(GAME* pGame)
{
	GAME game;

	// Init random
	srand(time(NULL));

	// Init special cards mode
	game.turnsDir = 1;
	game.isWaterfall = false;

	// Init cardStatistics
	game.logSizeStatis = CARD_NUM;
	for (int i = 0; i < game.logSizeStatis; i ++)
	{
		// Hold the amount of times that a card from a spesific kind appears
		game.cardStatistics[i].count = 0;
		game.cardStatistics[i].kindCard = i;
	}

	// Init Players (include every player and every card)
	game.sizePlayers = initPlayers(&game.players, game.cardStatistics);

	// Put card in the middle
	game.upperCard = initCard(CARD_NUM - SPECIAL_CARDS_NUM, game.cardStatistics);

	*pGame = game;
}

// Play a game according to the given game-struct inside the adress pGame
// The function change the value of varibles inside the the adress pGame
// The function play turns untill the game is over
// In every turn, the function play the move of the current palyer and update the turn to the next player
// Creat a new dynamic array cards and dosent free it! Must happend after use.
int playGame(GAME* pGame)
{
	GAME game = *pGame;
	int currentPlayer = 0, cardChoice;
	bool isValidMove, isGameOver = false;

	// There isn't a winner, a new turn is starting
	while (!isGameOver)
	{
		// Deck
		printf("\nUpper card:\n");
		printCard(game.upperCard);

		// Print current player's cards
		printPlayerCards(game.players[currentPlayer]);

		// Init move
		isValidMove = false;

		// Play move
		while (!isValidMove)
		{
			// Scan move from the player
			cardChoice = scanMove(game.players[currentPlayer].logSizeCards);

			// Check if the move is valid and play if it is
			isValidMove = playTurn(&game, currentPlayer, cardChoice);
		}

		// Check if the game is over
		isGameOver = endTurn(&game, currentPlayer);

		// Update next player
		if (!isGameOver)
		{
			currentPlayer = nextTurn(&game, currentPlayer, cardChoice);
		}
	}

	*pGame = game;
	return currentPlayer;
}

// Summarize the results of a game according to the given game-struct inside the adress pGame and print them
// The function sort the results of the game
void endGame(GAME game, int winnerPlayer)
{
	char strKind[MAX_KIND_STR];

	// Sort array
	sortStatisArray(game.cardStatistics, game.logSizeStatis);

	printf("\nThe winner is... %s! Congratulations!\n\n", game.players[winnerPlayer].name);
	
	// Print statistics
	printf("************ Game Statistics ************\n");
	printf("Card# | Frequency\n");
	printf("_________________\n");

	for (int i = 0; i < game.logSizeStatis; i++)
	{
		// Print kind of card
		getKindCard(game.cardStatistics[i].kindCard, strKind);

		// The card is special
		if (strlen(strKind) > DIR_STR_LEN)
		{
			printf(" %-5s ", strKind);
		}
		// The card is change direction
		else if (strlen(strKind) == DIR_STR_LEN)
		{
			printf("  %s  ", strKind);
		}
		// The card is a number
		else
		{
			printf("   %s   ", strKind);
		}

		// Print the card's statistic
		printf("|    %d\n", game.cardStatistics[i].count);
	}
}

// Play a turn of a given player and return wheater the move is valid or not
// The function update the cards acordding to the move of the player
// The function play acording to a given choice of card
// Creat a new dynamic array cards and dosent free it! Must happend after use.
bool playTurn(GAME* pGame, int currentPlayer, int cardChoice)
{
	GAME game = *pGame;
	bool isValidMove = false;

	// Player wants card from deck
	if (cardChoice == TAKE_CARD_FROM_DECK)
	{
		takeCardFromDeck(&game.players[currentPlayer], game.cardStatistics);
		isValidMove = true;
	}

	// Player want put card in the middle
	else
	{
		isValidMove = putCardInDeck(&game, currentPlayer, cardChoice - 1);
	}

	*pGame = game;
	return isValidMove;
}

// End the turn of a given player 
// The function check wheather the cards of a given player are over or not
// In case that the cards are over but the player need to take another card from the deck(acording to the rules), the function add him a new card
// The function change the variables players(cards, logSizeCards, physSizeCards), isWaterfall of the game-struct inside the adress pGame
// The function return wheather the game is over or not
bool endTurn(GAME* pGame, int currentPlayer)
{
	GAME game = *pGame;
	bool isGameOver = false;

	// The current player finish it's cards - the game is over
	if (game.players[currentPlayer].logSizeCards == 0)
	{
		// The last card was plus or stop and there are 2 players - the game isn't over
		if (game.upperCard.kind == PLUS_CARD || (game.upperCard.kind == STOP_CARD && game.sizePlayers == 2))
		{
			takeCardFromDeck(&game.players[currentPlayer], game.cardStatistics);
			
			// Taki waterfall is over
			game.isWaterfall = false;
		}
		else
		{
			isGameOver = true;
		}
	}

	*pGame = game;
	return isGameOver;
}

// Determinate the next player 
// The function check the if the player put card in the middle or finish waterfall and acording to the special cards, retrun the player that his turn is starting
// Update the special card variable - turnsDir in struct-game inside the adress pGame 
int nextTurn(GAME* pGame, int currentPlayer, int cardChoice)
{
	GAME game = *pGame;
	int kindUpperCard = game.upperCard.kind;

	// If player took card from the deck
	if (cardChoice == TAKE_CARD_FROM_DECK)
	{
		// Finish waterfall - pay attention to the last card 
		if (game.isWaterfall)
		{
			// Taki waterfall is over
			game.isWaterfall = false;

			// If the last card of the waterfall was special card - change the turns acorddingly
			currentPlayer =	moveSpecialCard(currentPlayer, &game.turnsDir, kindUpperCard, game.sizePlayers);
		}

		// Continue to next player
		currentPlayer =	moveToNextPlayer(currentPlayer, game.turnsDir, game.sizePlayers);
	}

	// The player put card in the middle
	// Taki card - continue the waterfal. Plus card - another turn to the current player
	// In other cases - change turns
	else if (!game.isWaterfall && kindUpperCard != PLUS_CARD)
	{
		// Change turns acordding to special cards
		currentPlayer = moveSpecialCard(currentPlayer, &game.turnsDir, kindUpperCard, game.sizePlayers);

		// Continue to next player
		currentPlayer = moveToNextPlayer(currentPlayer, game.turnsDir, game.sizePlayers);
	}

	*pGame = game;
	return currentPlayer;
}

// Aplay the special cards 
// Return the changes in the next player acording the special cards
// Change the value inside the given adress pTurnsDir
int moveSpecialCard(int currentPlayer, int* pTurnsDir, int kindCard, int numPlayers)
{
	int turnsDir = *pTurnsDir;

	// Change direction card - switch direction
	if (kindCard == CHANGE_DIR_CARD)
	{
		turnsDir *= -1;

		// Change direction in case of 2 players
		if (numPlayers == 2)
		{
			currentPlayer += turnsDir;
		}
	}

	// Stop card - skip player
	if (kindCard == STOP_CARD)
	{
		currentPlayer += turnsDir;
	}

	*pTurnsDir = turnsDir;
	return currentPlayer;
}

// Return the next player
// Change the player acording the given direction and total number of players
int moveToNextPlayer(int currentPlayer, int turnsDir, int numPlayers)
{
	// Next player
	currentPlayer += turnsDir;

	// Init turens
	while (currentPlayer < 0 || numPlayers  - 1 < currentPlayer)
	{
		currentPlayer = currentPlayer - (turnsDir * numPlayers);
	}

	return currentPlayer;
}

// Get as an input a move of a player acordding to his amount of card (given size)
// Check it's validation 
// Return a valid move
int scanMove(int sizeCards)
{
	int playerChoise;

	printf("Please enter %d if you want to take a card from the deck\nor 1 - %d if you want to put one of your cards in the middle:", TAKE_CARD_FROM_DECK, sizeCards);
	scanf("%d", &playerChoise);

	// Validation
	while ((playerChoise < 0) || (playerChoise > sizeCards))
	{
		printf("Invalid choice! Try again.\n");
		printf("Please enter %d if you want to take a card from the deck\nor 1 - %d if you want to put one of your cards in the middle:", TAKE_CARD_FROM_DECK, sizeCards);
		scanf("%d", &playerChoise);
	}
	
	return playerChoise;
}

// Get a card that the player want to put in the middle
// If it is valid move - the function put the card in the middle and remove it from the cards array throgh the adress pGame
// If the card is from kind change color - the function get as an input the wanted color and update the card
// The function return whether it is valid move or not
// The function update the mode of special card - TAKI (isWaterfall)
// The function change the variables upperCard, isWaterfall and cards
// Creat a new dynamic array cards and dosent free it! Must happend after use.
bool putCardInDeck(GAME* pGame,int currentPlayer, int cardNumChoice)
{
	GAME game = *pGame;
	bool isValidMove = true;
	int colorChoice;
	CARD choosenCard = game.players[currentPlayer].cards[cardNumChoice];

	// Change color
	if (choosenCard.kind == CHANGE_COLOR_CARD)
	{
		// Scan the color of the card from the user
		colorChoice = scanColorChoice();

		// Remove card from player array
		removeCardFromArray(&game.players[currentPlayer], cardNumChoice);

		// Put card in de middle
		choosenCard.color = colorChoice - 1;
		game.upperCard = choosenCard;

		// Taki waterfall is over
		game.isWaterfall = false;
	}

	// It is a valid card
	else if (choosenCard.kind == game.upperCard.kind || choosenCard.color == game.upperCard.color)
	{
		// In waterfall you can put in the middle only cards of the same color
		if (!game.isWaterfall || (game.isWaterfall && choosenCard.color == game.upperCard.color))
		{
			// Remove card from player array
			removeCardFromArray(&game.players[currentPlayer], cardNumChoice);

			// Put card in de middle
			game.upperCard = choosenCard;

			// Taki waterfall is begin
			if (choosenCard.kind == TAKI_CARD)
			{
				game.isWaterfall = true;
			}
		}
		else
		{
			isValidMove = false;
		}
	}

	// Invalid card
	else
	{
		printf("Invalid card!Try again.\n");
		isValidMove = false;
	}

	*pGame = game;
	return isValidMove;
}

// Get as an input a color of change color-card 
// Check it's validation 
// Return a valid color
int scanColorChoice()
{
	int colorChoice;

	printf("Please enter your color choice:\n");
	printf("1 - Yellow\n2 - Red\n3 - Blue\n4 - Green\n");
	scanf("%d", &colorChoice);

	// Validation
	while ((colorChoice < 1) || (colorChoice > COLOR_NUM))
	{
		printf("Invalid choice! Try again.\n");
		printf("Please enter your color choice:\n");
		printf("1 - Yellow\n2 - Red\n3 - Blue\n4 - Green\n");
		scanf("%d", &colorChoice);
	}

	return colorChoice;
}

// Add another card to the given struct-player inside pPlayer adress
// The function change variables (card array, his logSizeCard, his physSizeCard) of the struct-player inside pPlayer adress
// Creat a new dynamic array cards and dosent free it! Must happend after use.
// Assume that the given array is large enough
void takeCardFromDeck(PLAYER* pPlayer, CARD_STATISTICS cardStatistics[])
{
	PLAYER player = *pPlayer;

	// There is no place in the array
	if (player.logSizeCards == player.physSizeCards)
	{
		// Increse the array size
		rellocCards(&player);
	}

	// Add to cards a card from the deck
	player.cards[player.logSizeCards] = initCard(CARD_NUM, cardStatistics);
	player.logSizeCards++;

	*pPlayer = player;
}

// Remove a given card from a given array  
// The function change variables (card array and his logSizeCard) of the struct-player inside pPlayer adress
void removeCardFromArray(PLAYER* pPlayer, int cardNumChoice)
{
	PLAYER player = *pPlayer;

	for (int i = cardNumChoice; i < player.logSizeCards - 1; i++)
	{
		player.cards[i] = player.cards[i + 1];
	}

	player.logSizeCards--;

	*pPlayer = player;
}

// Initial players array and change him with a given pointer to his adress (pPlayers)
// The function create a new dynamic array acording to an input
// The function initial all the players in the array 
// Creat a new dynamic array player and dosent free it! Must happend after use.
// Assume that the given array is large enough
int initPlayers(PLAYER** pPlayers, CARD_STATISTICS cardStatistics[])
{
	int numPlayers;
	PLAYER* players = *pPlayers; // Array players

	printf("Please enter the number of players:");
	scanf("%d", &numPlayers);

	while (numPlayers < MIN_PLAYERS_NUM)
	{
		printf("Invalid choice! Try again.\n");
		printf("Please enter the number of players:");
		scanf("%d", &numPlayers);
	}

	players = (PLAYER*)malloc(numPlayers * sizeof(PLAYER));

	mallocFail(players);

	// Init Player Data
	for (int i = 0; i < numPlayers; i++)
	{
		initPlayer(players + i, i, cardStatistics);
	}

	*pPlayers = players;
	return numPlayers;
}

// Initial all the variables of a given player-struct inside the adress pPlayer 
// Create a new array with malloc
// Creat a new dynamic array cards and dosent free it! Must happend after use.
// Assume that the given array is large enough
initPlayer(PLAYER* pPlayer, int numPlayer, CARD_STATISTICS cardStatistics[])
{
	PLAYER player;

	// Init name
	// Assum that the scan name dosen't contain spaces or white-spaces and conatin at most 20 chars
	printf("Please enter the first name of player #%d:\n", numPlayer + 1);
	scanf("%s", player.name);

	// Create cards array
	player.cards = (CARD*)malloc(START_GAME_CARDS * 2 * sizeof(CARD));

	mallocFail(player.cards);

	player.logSizeCards = START_GAME_CARDS;
	player.physSizeCards = START_GAME_CARDS * 2;

	// Init cards
	for (int i = 0; i < player.logSizeCards; i++)
	{
		player.cards[i] = initCard(CARD_NUM, cardStatistics);
	}

	*pPlayer = player;
}

// Create a card and return it
// The function initial the variables of the new card
// The function chosoe a rand value for the kind of the card in the range: 0 to a given number
// The function count in a given array the number of creation of every kind card
// Assume that the given array is large enough
CARD initCard(int rangeCrads, CARD_STATISTICS cardStatistics[])
{
	CARD card;

	card.kind = rand() % rangeCrads;

	// The card has color
	if (card.kind != CHANGE_COLOR_CARD)
	{
		card.color = rand() % COLOR_NUM;
	}

	// The card has color
	else
	{
		card.color = NO_COLOR_CARD;
	}

	// Add the new card to statistic
	cardStatistics[card.kind].count++;

	return card;
}

// Print the given card
// The function find the representation of kind and color of the card and print them inside a template of a card
// The function print the card in the color of the given card and in the end of the function reset back the color of print
void printCard(CARD card)
{
	char color;
	char strKind[MAX_KIND_STR];

	// Get the card's color and print in this color
	color = getColorCard(card.color);

	// Get card's kind
	getKindCard(card.kind, strKind);

	printf("*********\n");
	printf("*       *\n");

	// Special card
	if (strlen(strKind) > DIR_STR_LEN)
	{
		printf("* %5s *\n", strKind);
	}
	else if (strlen(strKind) == DIR_STR_LEN)
	{
		printf("*  %s  *\n", strKind);
	}
	else
	{
		printf("*   %s   *\n", strKind);
	}

	printf("*   %c   *\n", color);
	printf("*       *\n");
	printf("*********\n\n");

	// Reset print color
	printf("\033[0m");
}

// Print the cards of the given player
void printPlayerCards(PLAYER player)
{
	printf("%s's turn:\n\n", player.name);

	for (int i = 0; i < player.logSizeCards; i++)
	{
		printf("Card #%d:\n", i + 1);
		printCard(player.cards[i]);
	}
}

// Return a char that represent a color according to the given number of color
// Set the prints from now untill a new order to be in this color
char getColorCard(int colorNum)
{
	char color = ' ';

	switch (colorNum)
	{
	case YELLOW_CARD:
		color = 'Y';
		printf("\033[1;33m");
		break;

	case RED_CARD:
		color = 'R';
		printf("\033[1;31m");
		break;

	case BLUE_CARD:
		color = 'B';
		printf("\033[1;34m");
		break;

	case GREEN_CARD:
		color = 'G';
		printf("\033[1;32m");
		break;

	case NO_COLOR_CARD:
		color = ' ';
		break;
	}

	return color;
}

// Enter the name of a given kind card into strKind string
// Assume that the string is large engouh
void getKindCard(int kindNum, char strKind[])
{
	char num[MAX_NUM_STR];

	// Enter the name of the card into strKind
	switch (kindNum)
	{
	case PLUS_CARD:
		strcpy(strKind, "PLUS");
		break;

	case STOP_CARD:
		strcpy(strKind, "STOP");
		break;

	case CHANGE_DIR_CARD:
		strcpy(strKind, "<->");
		break;

	case TAKI_CARD:
		strcpy(strKind, "TAKI");
		break;

	case CHANGE_COLOR_CARD:
		strcpy(strKind, "COLOR");
		break;

	default:
		num[FIRST_INDEX] = '0' + (kindNum + 1);
		num[FIRST_INDEX + 1] = '\0';
		strcpy(strKind, num);
		break;
	}
}

// Exit the program in case that malloc fail - there is no more plae in the memory for the program
void mallocFail(void* pointer)
{
	if (pointer == NULL)
	{
		printf("Memory allocation failed!!");
		exit(1);
	}
}

// Remalloc - increase the physical size of a given dynamic array - cards (inside players)
// Create a new array multiple 2 from his former size
// Copy the value of the current elemets insidr the former array
// Free the former array memory
// Dosent free the new dynamic array cards! Must happend after use.
void rellocCards(PLAYER* pPlayer)
{
	PLAYER player = *pPlayer;
	CARD* tempCards;

	// Create a new array
	player.physSizeCards *= 2;
	tempCards = (CARD*)malloc(player.physSizeCards * sizeof(CARD));

	mallocFail(tempCards);

	// Copy the value of the former array
	for (int i = 0; i < player.logSizeCards; i++)
	{
		tempCards[i] = player.cards[i];
	}

	// Free the formr array
	free(player.cards);

	// Change the adress of pointer to the new array
	player.cards = tempCards;

	*pPlayer = player;
}

// Free the memory that assigned from malloc all over the program
// Assum that the given pGame is initial
void freeMemoryGame(GAME* pGame)
{
	// Free cards array for each player
	for (int i = 0; i < pGame->sizePlayers; i++)
	{
		free(pGame->players[i].cards);
	}

	// Free players array
	free(pGame->players);
}

// Sort CARD_STATISTICS array in decreasing order using merge sort algorithm
// The function change the given array
// Assume that the array is large engouh
void sortStatisArray(CARD_STATISTICS arr[], int size)
{
	CARD_STATISTICS* tmpArr;

	// Merge sort
	if (size > 1)
	{
		// Sort 2 halfs of the array
		sortStatisArray(arr, size/2);
		sortStatisArray(arr + (size/2), size - (size/2));

		// Merge the sorts array
		tmpArr = mergeStatisArray(arr, size / 2, arr + (size / 2), size - (size / 2));
		mallocFail(tmpArr);

		// Copy the sort array to the original array and free the temporary array
		copyStatisArray(arr, tmpArr, size);
		free(tmpArr);
	}	
}

// Merge the two given arrays into a new sorted array (from the biggest to the smallest)
// The function return the new array
// Creat a new dynamic array cardstatistics and dosent free it! Must happend after use.
// Assume that the given arrays are large enough
CARD_STATISTICS* mergeStatisArray(CARD_STATISTICS arr1[], int size1, CARD_STATISTICS arr2[], int size2)
{
	int ind1 = 0, ind2 = 0, indNewArr = 0;
	
	CARD_STATISTICS* newArr = (CARD_STATISTICS*)malloc((size1 + size2) * sizeof(CARD_STATISTICS));
	mallocFail(newArr);

	// Both arrays have elements
	while ((ind1 < size1) && (ind2 < size2))
	{
		if (arr1[ind1].count >= arr2[ind2].count)
		{
			newArr[indNewArr] = arr1[ind1];
			ind1++;
		}
		else
		{
			newArr[indNewArr] = arr2[ind2];
			ind2++;
		}

		indNewArr++;
	}

	// One of the array is not empty - add its elements to the nwe array
	while (ind1 < size1)
	{
		newArr[indNewArr] = arr1[ind1];
		ind1++;
		indNewArr++;
	}

	while (ind2 < size2)
	{
		newArr[indNewArr] = arr2[ind2];
		ind2++;
		indNewArr++;
	}

	return newArr;
}

// Copy into the first given array the elments of the second given array
// The function change the first array
// Assume that the given arrays are large enough
void copyStatisArray(CARD_STATISTICS dest[], CARD_STATISTICS src[], int size)
{
	for (int i = 0; i < size; i++)
	{
		dest[i] = src[i];
	}
}