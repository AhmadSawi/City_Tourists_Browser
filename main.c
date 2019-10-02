/*
 * Birzeit University
 * To: Dr. Majdi Mafarja
 * Ahmad Sawi
 * 1150007
 * Data Structures Project 3
 * */

/*
 * Summary: This is a Simple program that takes in a list of cities and how mant tourists they get, and processes all
 *          the information and stores it in an AVL self balancing tree data structure which has functions that can be
 *          accessed using the menus in this application. then this application moves all datat in the AVL tree into a
 *          hash table that also has many functions that can be accessed
 * */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>    //to get boolean
#include <string.h>

#define CITY_NAME_LENGTH 50            /* macros for use with strings */
#define COUNTRY_NAME_LENGTH 100
#define SCANNED_STRING_MAX_LENGTH 200
#define FILE_PATH_MAX_LENGTH 200

struct city{                           /* Linked List data structure */
    int rank;
    char cityName[CITY_NAME_LENGTH];
    double numberOfTourists;
    struct city *next;
};

typedef struct city city_t;
typedef city_t *p2c;
typedef p2c List;
typedef p2c Position;


struct country{                                /* AVL tree data structure */
    char countryName[COUNTRY_NAME_LENGTH];
    List touristCities;
    double totalTourists;
    struct country *left;
    struct country *right;
    int height;
};

typedef struct country country_t;
typedef country_t *p2t; //t for tree
typedef p2t TreePosition;
typedef p2t CountryTree;

struct hash{                                  /* Hash table data structure */
    char cityName[CITY_NAME_LENGTH];
    double tourists;
    int rank;
    bool isSlotTaken;
};

typedef struct hash hash_t;
typedef hash_t *p2h;
typedef p2h HashTable;


int fileLineCount(FILE*);                                    //counts the number of lines in a file
void getFromFile(FILE*, char[][SCANNED_STRING_MAX_LENGTH]);   //gets data from file
void removeNewLine(char[]);                                   //removes the new line character at the end of a string
void removeEmptyLines(char[][SCANNED_STRING_MAX_LENGTH], int*);//removes empty lines in a file
CountryTree fillTreeFromFile(CountryTree, FILE*, int);         //fills all data from file to AVL tree
bool isLast(Position);
Position newNode(int, char[], double);
bool insertList(List, int, char[], double);
int max(int, int);                                              //returns the max of two integers
static int height(TreePosition);                                //returns the height of an avl tree node
static TreePosition singleRotateRight(TreePosition);            /* rotation functions for use with balancing the tree */
static TreePosition singleRotateLeft(TreePosition);
static TreePosition doubleRotateRight(TreePosition);
static TreePosition doubleRotateLeft(TreePosition);
CountryTree AVLinsert(int, char[], char[], double, CountryTree);//insert a node to the avl tree
TreePosition findMin(CountryTree);                              //finds the min of a tree by going left over and over
CountryTree AVLdelete(char[COUNTRY_NAME_LENGTH], TreePosition); //deletes a node from a tree while keeping it balanced
void updateHeight(CountryTree);                                 //updates height for every single node in the tree
TreePosition AVLfind(CountryTree, char[]);                      //finds the position of a certian node
void allCountriesInOrder(CountryTree);                          //prints all countries from AVL tree inorder (Alphabettically)
bool isPrime(unsigned int);
unsigned int nextPrime(unsigned int);                           //returns the next prime number of an integer
unsigned int hash(char[], unsigned int);                        //hash function for use with hash table
unsigned int doubleHash(unsigned int, unsigned int);
void hashInsert(HashTable[], char[], double, int, unsigned int);//insert a record to hash table
bool hashDelete(HashTable *, char *, unsigned int);             //delete a record from hash table
int hashFind(HashTable[], char[], unsigned int);                //finds the index of a record in a hash table
void freeHashTable(HashTable[], unsigned int);
void printHashTable(HashTable[], unsigned int);
void printHashTableToFile(HashTable[], unsigned int);
int getNumberOfRecordsinHashTable(HashTable[], unsigned int);   //finds the number of records in a hash table
void initializeHashTable(HashTable[], unsigned int);            //initializes records with default values
void fillHashTableFromTree(CountryTree, HashTable[], unsigned int);//fills hash stable from AVL tree nodes
void reHash(HashTable[], HashTable[], unsigned int, unsigned int); //rehashing for when the records exceed the max in a hash table
void openMenu();                                                 //main program procedure
void showMainMenu();                                             /* displaying menus for user */
void showAVLmenu();
void showHashMenu();
void AVLfunctions(TreePosition);                               // all functions for AVL tree
void hashFunctions(HashTable[], unsigned int);                 //all functions for hash table

int main() {
   openMenu();
    return 0;
}

int fileLineCount(FILE *file){
    int count = 0;
    char ch;
    do{
        ch = (char) fgetc(file);
        if(ch == '\n')
            count++;
    } while (!feof(file));
    rewind(file);     //returns the cursor to the beginning of the file for future use
    return count+1;   //+1 because it starts from 0
}

void getFromFile(FILE *file, char ret[][SCANNED_STRING_MAX_LENGTH]){
    int i = 0;
    char buffer[SCANNED_STRING_MAX_LENGTH];
    while (fgets(buffer, SCANNED_STRING_MAX_LENGTH, file) != NULL) {
        removeNewLine(buffer);
        strcpy(ret[i++], buffer);
    }
    fclose(file);
}

void removeNewLine(char s[]){
    int i = 0, k = 0;
    for(i = 0; i < strlen(s); i++){
        if(s[i] == '\n'){
            for(k = i; k < strlen(s); k++){
                s[k] = s[k+1];
            }
            s[k-1] = '\0';
        }
    }
}

void removeEmptyLines(char cityStrings[][SCANNED_STRING_MAX_LENGTH], int *numberOfLines){
    int i = 0, k = 0;
    for (i = 0; i < *numberOfLines; i++){
        removeNewLine(cityStrings[i]);
        if( strcmp(cityStrings[i], "") == 0 ){
            for (k = i; k < *numberOfLines; k++){
                strcpy(cityStrings[k], cityStrings[k+1]);
            }
            *numberOfLines = *numberOfLines - 1;
            i--;
        }
    }
}

CountryTree fillTreeFromFile(CountryTree T, FILE *rankFile ,int numberOfLines){
    char dataStrings[numberOfLines][SCANNED_STRING_MAX_LENGTH], data[4][COUNTRY_NAME_LENGTH];
    getFromFile(rankFile, dataStrings);
    removeEmptyLines(dataStrings, &numberOfLines);

    int i = 0, q = 0;
    for(i = 0; i < numberOfLines; i++){
        char *buffer;                //buffer for storing slitted strings
        char cityName[CITY_NAME_LENGTH], countryName[COUNTRY_NAME_LENGTH];
        int rank;
        double tourists;

        buffer = strtok(dataStrings[i], "*");

        q = 0;
        while(buffer != NULL){
            strcpy(data[q++], buffer);
            buffer = strtok(NULL, "*");
        }

        rank = atoi(data[0]);
        strcpy(cityName, data[1]);
        strcpy(countryName, data[2]);
        tourists = atof(data[3]);

        //printf("%d %s - %s %f\n", rank, cityName, countryName, tourists);

        T = AVLinsert(rank, cityName, countryName, tourists, T);
    }

    return T;

}


bool isLast(Position p){
    return (p->next == NULL);
}

Position newNode(int rank, char name[], double tourists){
    Position new;
    new = (Position) malloc(sizeof(city_t));
    if(new == NULL){
        printf("OUT OF RAM! malloc realted error.\n");
        return NULL;
    }
    new->rank = rank;
    strcpy(new->cityName, name);
    new->numberOfTourists = tourists;
    new->next = NULL;
    return new;
}

bool insertList(List L, int rank, char name[], double tourists){
    Position p = L, new = newNode(rank, name, tourists);
    if(new == NULL)
        return false;
    while(!isLast(p))
        p = p->next;
    p->next = new;
    new->next = NULL;
    return true;
}

void deleteList(List L){
    Position p, temp;
    p = L->next;
    while(p != NULL){
        temp = p->next;
        free(p);           //so it frees every node from memory
        p = temp;
    }
    L->next = NULL;
}

int max(int first, int second){
    if(first >= second)
        return first;
    else
        return second;
}

static int height(TreePosition P){
    if(P == NULL)
        return -1;
    else
        return P->height;
}

static TreePosition singleRotateRight(TreePosition P){
    TreePosition K;
    K = P->left;
    P->left = K->right;
    K->right = P;

    P->height = max( height(P->left), height(P->right)) + 1;      //to maintain correct heights
    K->height = max( height(K->left), P->height ) + 1;

    return K;  //returning new root
}

static TreePosition singleRotateLeft(TreePosition P){
    TreePosition K;
    K = P->right;
    P->right = K->left;
    K->left = P;

    P->height = max( height(P->left), height(P->right)) + 1;
    K->height = max( height(K->right), P->height ) + 1;

    return K;
}

static TreePosition doubleRotateRight(TreePosition P){
    P->left = singleRotateLeft(P->left);
    return singleRotateRight(P);
}

static TreePosition doubleRotateLeft(TreePosition P){
    P->right = singleRotateRight(P->right);
    return singleRotateLeft(P);
}

CountryTree AVLinsert(int rank, char cityName[CITY_NAME_LENGTH], char countryName[COUNTRY_NAME_LENGTH], double tourists, CountryTree T){

    if(T == NULL){
        T = (CountryTree) malloc(sizeof(country_t));
        if(T == NULL){
            printf("OUT OF RAM! malloc realted error.\n");
            return NULL;
        }else{
            strcpy(T->countryName, countryName);
            T->totalTourists = tourists;

            T->touristCities = (List) malloc(sizeof(city_t));
            if(T->touristCities == NULL){
                printf("ERROR: Out of memory!\n");
                exit(9);   //exit code 9 for programmer to identify where the error is from
            }
            T->touristCities->next = NULL;

            if(strcmp(cityName, "") != 0)  //for use when adding a country without city values
            insertList(T->touristCities, rank, cityName, tourists);

            T->height = 0;
            T->left = NULL;
            T->right = NULL;
        }
    }

    else if(strcmp(countryName, T->countryName) < 0){
        T->left = AVLinsert(rank, cityName, countryName, tourists, T->left);
        if( height(T->left) - height(T->right) == 2){
            if(strcmp(countryName, T->left->countryName) < 0)
                T = singleRotateRight(T);
            else
                T = doubleRotateRight(T);
        }
    }

    else if(strcmp(countryName, T->countryName) > 0){
        T->right = AVLinsert(rank, cityName, countryName, tourists, T->right);
        if( height(T->right) - height(T->left) == 2){
            //printf("hi");
            if(strcmp(countryName, T->right->countryName) > 0)
                T = singleRotateLeft(T);
            else
                T = doubleRotateLeft(T);
        }
    }

    else{
        insertList(T->touristCities, rank, cityName, tourists);
        T->totalTourists += tourists;
    }

    T->height = max( height(T->left), height(T->right) ) + 1;

    return T;
}

TreePosition findMin(CountryTree T){
    if(T == NULL)
        return NULL;
    else if(T->left == NULL)
        return T;
    else
        return findMin(T->left);
}

CountryTree AVLdelete(char countryName[COUNTRY_NAME_LENGTH], CountryTree T){
    if(T == NULL) {
        printf("nothing to be deleted!\n");
        return NULL;
    }

    if(strcmp(countryName, T->countryName) < 0) { //check left side
        T->left = AVLdelete(countryName, T->left);
        if( height(T->left) - height(T->right) == 2){
            if(strcmp(countryName, T->left->countryName) < 0)
                T = singleRotateLeft(T);
            else
                T = doubleRotateLeft(T);
        }
    }

    else if(strcmp(countryName, T->countryName) > 0) { //check right side
        T->right = AVLdelete(countryName, T->right);
        if( height(T->right) - height(T->left) == 2){
            if(strcmp(countryName, T->right->countryName) > 0)
                T = singleRotateRight(T);
            else
                T = doubleRotateRight(T);
        }
    }

    else if(T->right != NULL && T->left != NULL){   //has 2 children
        TreePosition minOfRight = findMin(T->right);
        strcpy(T->countryName, minOfRight->countryName);
        deleteList(T->touristCities);
        T->touristCities = minOfRight->touristCities;
        T->totalTourists = minOfRight->totalTourists;
        T->right = AVLdelete(T->countryName, T->right);
    }

    else{                                        //has 0 or 1 child
        CountryTree toBeDeleted = T;
        if(T->left != NULL)
            T = T->left;
        else
            T = T->right;

        free(toBeDeleted);
    }

    return T;

}

void updateHeight(CountryTree T){
    if(T != NULL){
        updateHeight(T->left); //pre order because children have to get height before parents
        updateHeight(T->right);
        T->height = max( height(T->left), height(T->right) ) + 1;
    }
}

TreePosition AVLfind(CountryTree T, char Countryname[COUNTRY_NAME_LENGTH]){
    if(T == NULL)
        return NULL;
    if(strcmp(Countryname, T->countryName) < 0)
        return AVLfind(T->left, Countryname);
    else if(strcmp(Countryname, T->countryName) > 0)
        return AVLfind(T->right, Countryname);
    else
        return T;

}

void allCountriesInOrder(CountryTree T){  //will result in alphabatically sorted list
    if(T != NULL){
        allCountriesInOrder(T->left);
        printf("%s\n", T->countryName);
        allCountriesInOrder(T->right);
    }
}

bool isPrime(unsigned int number){
    int i = 2;
    bool flag = true;

    while(i < number){
        if((number % i) == 0)
            return false;
        i++;
    }

    return flag;
}

unsigned int nextPrime(unsigned int index){
    unsigned int answer = index;
    while(!isPrime(answer))
        answer++;
    return answer;
}

unsigned int hash(char key[], unsigned int size){
    unsigned int hashValue = 0;
    int i = 0;

    while(key[i] != '\0'){
        hashValue += (((hashValue << 5) + 7) * key[i++]);       //hash function
    }

    return (hashValue % size);
}

unsigned int doubleHash(unsigned int number, unsigned int size){
    return ( (7 - (number % 7)) % size );
}

void hashInsert(HashTable H[], char cityName[], double tourists, int rank ,unsigned int size){
    unsigned int index = 0, i = 0;

    index = hash(cityName, size);

    while( H[index]->isSlotTaken ){
        index = (((index + i) * doubleHash(index, size)) % size);  //double hashing
        //index = ((index + i) % size);                            //linear probing
        i++;
    }

    strcpy(H[index]->cityName, cityName);
    H[index]->tourists = tourists;
    H[index]->rank = rank;
    H[index]->isSlotTaken = true;
}

bool hashDelete(HashTable H[], char cityToDelete[], unsigned int size){
    unsigned int index = 0, i = 0;
    index = hash(cityToDelete, size);

    while( (i < size) && strcmp(H[index]->cityName, cityToDelete) != 0 ){         //so it stops when it reaches size
        index = (((index + i) * doubleHash(index, size)) % size);
        i++;
    }

    if(i < size && H[index]->isSlotTaken) {
        H[index]->isSlotTaken = false;
        return true;
    }
    else {
        printf("Nothing to be deleted!\n");
        return false;
    }

}

int hashFind(HashTable H[], char cityToFind[], unsigned int size){
    unsigned int index = 0, i = 0;
    index = hash(cityToFind, size);

    while( (i < size) && strcmp(H[index]->cityName, cityToFind) != 0 ){
        index = (((index + i) * doubleHash(index, size)) % size);
        i++;
    }

    if( (i < size) && H[index]->isSlotTaken)
        return index;
    else
        return -1;      //to identify not found
}

void freeHashTable(HashTable H[], unsigned int size){
    int i = 0;
    for (i = 0; i < size; i++)
        free(H[i]);
}

void printHashTable(HashTable H[], unsigned int size){
    int i = 0;

    printf("index\t\tRank\t\tCity\t\t\t     Tourists\n");
    printf("-----------------------------------------------------------\n");
    for(i = 0; i < size; i++){

        if(H[i]->isSlotTaken){
            printf("%d\t\t\t%d\t\t\t%-20s%0.2f Millions\n", i, H[i]->rank, H[i]->cityName, H[i]->tourists);
            printf("-----------------------------------------------------------\n");
        } else {
            printf("%d\t\t\t\tEmpty\n", i);
            printf("-----------------------------------------------------------\n");
        }

    }
}

void printHashTableToFile(HashTable H[], unsigned int size){

    FILE *output;
    printf("ente the name of the file you want to print hash table to\n");
    char path[FILE_PATH_MAX_LENGTH];
    scanf(" %[^\n]s", path);
    output = fopen(path, "w");


    int i = 0;

    fprintf(output,"index\t\tRank\t\tCity\t\t\t     Tourists\n");
    fprintf(output,"-----------------------------------------------------------\n");
    for(i = 0; i < size; i++){

        if(H[i]->isSlotTaken){
            fprintf(output,"%d\t\t\t%d\t\t\t%-20s%0.2f Millions\n", i, H[i]->rank, H[i]->cityName, H[i]->tourists);
            fprintf(output,"-----------------------------------------------------------\n");
        } else {
            fprintf(output,"%d\t\t\t\tEmpty\n", i);
            fprintf(output,"-----------------------------------------------------------\n");
        }

    }
    fclose(output);
    printf("The file %s has been filled with the hased table!\n", path);
}

int getNumberOfRecordsinHashTable(HashTable H[], unsigned int size){
    int records = 0, i = 0;
    for(i = 0; i < size; i++){
        if(H[i]->isSlotTaken)
            records++;
    }
    return records;
}

void initializeHashTable(HashTable H[], unsigned int size){
    int i = 0;
    for (i = 0; i < size; ++i) {
        H[i] = (HashTable) malloc(sizeof(hash_t));
        H[i]->isSlotTaken = false;
        H[i]->tourists = 0;
        H[i]->rank = 0;              //default values
        strcpy(H[i]->cityName, "");
    }
}

void fillHashTableFromTree(CountryTree T, HashTable H[], unsigned int size){

    if(T != NULL){
        Position p = T->touristCities->next;
        while(p != NULL){                 //filling cities inside a country
            hashInsert(H, p->cityName, p->numberOfTourists, p->rank, size);
            p = p->next;
        }

        fillHashTableFromTree(T->left, H, size);
        fillHashTableFromTree(T->right, H, size);
    }

}

void reHash(HashTable old[], HashTable new[], unsigned int oldSize, unsigned int newSize){         //rehashing old values into new hash table
    int i = 0;
    for(i = 0; i < oldSize; i++){
        if(old[i]->isSlotTaken)
        hashInsert(new, old[i]->cityName, old[i]->tourists, old[i]->rank, newSize);
    }

    char cityToAdd[CITY_NAME_LENGTH];
    double touristsToAdd;
    int rankToAdd;

    printf("Enter the name of the city you want to insert to the hash table:\n");
    scanf(" %[^\n]s", cityToAdd);
    printf("Enter its number of tourists:\n");
    scanf("%lf", &touristsToAdd);
    printf("Enter its rank:\n");
    scanf("%d", &rankToAdd);
    hashInsert(new, cityToAdd, touristsToAdd, rankToAdd, newSize);
}

void openMenu(){
    CountryTree T;
    T = NULL;

    //FILE *input = fopen("Cities.txt", "r");     //pre set file for easy testing
    FILE *input;
    printf("ente the name of the file that has the info you want to load:\n");
    char path[FILE_PATH_MAX_LENGTH];
    scanf("%[^\n]s", path);
    input = fopen(path, "r");
    int numberOfLines = fileLineCount(input);
    T = fillTreeFromFile(T, input, numberOfLines);

    unsigned int tableSize = 0;
    tableSize = nextPrime(2 * (unsigned)numberOfLines);
    HashTable H[tableSize];

    int mainMenuChoice = 0;

    while(mainMenuChoice != 3) {
        showMainMenu();
        scanf("%d", &mainMenuChoice);
        switch (mainMenuChoice) {
            case 1:
                AVLfunctions(T);
                break;

            case 2:
                initializeHashTable(H, tableSize);
                fillHashTableFromTree(T, H, tableSize);
                hashFunctions(H, tableSize);
                break;

            case 3:
                printf("Thanks!\n");
                exit(1); //return code 1 identifies exit from main menu

            default:
                printf("\n*********************************************************\n");
                printf("ERROR:\n");
                printf("*********************************************************\n");
                printf("wrong entry! check the number you enetered and try again.\n");
                printf("*********************************************************\n\n");

        }
    }

}

void showMainMenu(){
    printf("Welcome to the Countrie's tourist places data structure!\n");
    printf("-------------------------------------------------\n");
    printf("Choose one of the following cluster of functions:\n");
    printf("-------------------------------------------------\n");
    printf("1. AVL Tree Functions\n");
    printf("2. Hashing Functions\n");
    printf("3. Exit\n");
    printf("-------------------------------------------------\n");
}

void showAVLmenu(){
    printf("-------------------------------------------------\n");
    printf("Choose one of the following functions:\n");
    printf("-------------------------------------------------\n");
    printf("1. Print out all countries sorted\n");
    printf("2. Search for a specific country\n");
    printf("3. Insert a new country\n");
    printf("4. Delete a specific country\n");
    printf("5. Calculate tree height\n");
    printf("6. Return to main menu\n");
    printf("7. Exit\n");
    printf("-------------------------------------------------\n");
}

void showHashMenu(){
    printf("-------------------------------------------------\n");
    printf("Choose one of the following functions:\n");
    printf("-------------------------------------------------\n");
    printf("1. Print out Hashed Table\n");
    printf("2. Print out table size\n");
    printf("3. Print out used hash function\n");
    printf("4. Insert a new record to hash table\n");
    printf("5. search for a specific record\n");
    printf("6. Delete a specific record\n");
    printf("7. Save hash table to file\n");
    printf("8. Return to main menu\n");
    printf("9. Exit\n");
    printf("-------------------------------------------------\n");
}

void AVLfunctions(TreePosition T){

    int functionChooser = 0;
    char countryToFind[COUNTRY_NAME_LENGTH], countryToInsert[COUNTRY_NAME_LENGTH], countryToDelete[COUNTRY_NAME_LENGTH];


    while(functionChooser != 6){  //6 is the return to main menu option
        showAVLmenu();
        scanf("%d", &functionChooser);
        switch(functionChooser){
            case 1:
                allCountriesInOrder(T);
                //printf("root is: %s with height: %d\n", T->countryName, T->height);
                break;

            case 2:
                printf("\nEnter the name of the country you want to get info for:\n");
                scanf(" %[^\n]s", countryToFind);   //so it scans string with spaces without the need to use the dangerous gets
                TreePosition foundNode = AVLfind(T, countryToFind);

                if(foundNode == NULL)
                    printf("\nthe country you entered could not be found in the tree!\n");
                else{
                    printf("\nCountry name:\t%s\n", foundNode->countryName);
                    printf("Country's total tourists:\t%0.2f Millions\n", foundNode->totalTourists);
                    printf("Top tourist cities in %s are:\n", countryToFind);
                    Position p = foundNode->touristCities->next;

                    if(p != NULL) {
                        printf("Rank\tName\t\t\t\tTourists\n");
                        printf("-------------------------------------------\n");
                        while (p != NULL) {
                            printf("%d\t\t%-20s%0.2f Millions\n", p->rank, p->cityName, p->numberOfTourists);
                            p = p->next;
                        }
                    } else
                        printf("No tourist cities in this Country!\n");
                }
                break;

            case 3:
                printf("Enter the name of the counrty you want to insert\n");
                scanf("%s", countryToInsert);
                T = AVLinsert(0, "", countryToInsert, 0 ,T);

                break;

            case 4:
                printf("Enter the name of the country you would like to delete:\n");
                scanf(" %[^\n]s", countryToDelete);
                T = AVLdelete(countryToDelete, T);
                updateHeight(T);q
                break;

            case 5:
                printf("\nTree height is: %d\n",height(T));
                break;

            case 6:
                return;

            case 7:
                printf("Thanks!\n");
                exit(2); //return code 2 identifies exit from AVL menu

            default:
                printf("\n*********************************************************\n");
                printf("ERROR:\n");
                printf("*********************************************************\n");
                printf("wrong entry! check the number you enetered and try again.\n");
                printf("*********************************************************\n\n");

        }
    }

}

void hashFunctions(HashTable H[], unsigned int size){

    char cityToAdd[CITY_NAME_LENGTH], cityToFind[CITY_NAME_LENGTH], cityToDelete[CITY_NAME_LENGTH];
    double touristsToAdd;
    int rankToAdd;
    int functionChooser = 0;

    int numberOfRecords = getNumberOfRecordsinHashTable(H, size);

    while(functionChooser != 8){ //8 is the return to main menu command
        showHashMenu();
        scanf("%d", &functionChooser);

        switch(functionChooser){

            case 1:
                printHashTable(H, size);
                break;

            case 2:
                printf("Hash Table size is: %d\nAnd there are %d records in it\n", size, numberOfRecords);
                break;

            case 3:
                printf("The hash function used in this implementation is:\n");
                printf("index = ((index << 5) + 7) * charachter\n");
                break;

            case 4:
                if((numberOfRecords + 1) > (size / 2)){    //rehashing has to be done

                    printf("#################### REHASH HAS BEEN DONE ####################\n");

                    unsigned int newSize = nextPrime(2 * size);
                    HashTable newTable[newSize];
                    initializeHashTable(newTable, newSize);
                    reHash(H, newTable, size, newSize);
                    freeHashTable(H, size);
                    hashFunctions(newTable, newSize);
                    return;

                } else {
                    printf("Enter the name of the city you want to insert to the hash table:\n");
                    scanf(" %[^\n]s", cityToAdd);
                    printf("Enter its number of tourists:\n");
                    scanf("%lf", &touristsToAdd);
                    printf("Enter its rank:\n");
                    scanf("%d", &rankToAdd);
                    hashInsert(H, cityToAdd, touristsToAdd, rankToAdd, size);
                    numberOfRecords++;  //updating number of records
                    break;
                }

            case 5:
                printf("Enter the name of the city you want the record for:\n");
                scanf(" %[^\n]s", cityToFind);
                int indexFound = hashFind(H, cityToFind, size);
                if(indexFound != -1) {
                    printf("-----------------------------------------------------\n");
                    printf("%d\t\t%d\t\t\t%-20s%0.2f Millions\n", indexFound, H[indexFound]->rank,
                           H[indexFound]->cityName, H[indexFound]->tourists);
                    printf("-----------------------------------------------------\n");
                } else{
                    printf("----------------------------------------------------------\n");
                    printf("Unfortunately! The City could not be found in the records!\n");
                    printf("----------------------------------------------------------\n");
                }
                break;

            case 6:
                printf("Enter the name of the city you want to delete:\n");
                scanf(" %[^\n]s", cityToDelete);
                if(hashDelete(H, cityToDelete, size)){
                    numberOfRecords--;
                }
                break;

            case 7:
                printHashTableToFile(H, size);
                break;

            case 8:
                return;

            case 9:
                printf("Thanks!\n");
                exit(3);  //exit code 3 identifies exit from Hash menu

            default:
                printf("\n*********************************************************\n");
                printf("ERROR:\n");
                printf("*********************************************************\n");
                printf("wrong entry! check the number you enetered and try again.\n");
                printf("*********************************************************\n\n");
        }

    }
}