#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comic.h"

int main(int argc, char* argv[])
{
     //Checks if correct # of arguments are given
    if (argc != 3) {
        fprintf(stderr, "Incorrect number of arguments.\n");
        fprintf(stderr, "USAGE: ./%s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    //Open input and output files
    FILE* in = fopen(argv[1], "r");
    FILE* out = fopen(argv[2], "w");

    //Check if files were opened successfully
    if (in == NULL || out == NULL) {
        fprintf(stderr, "Can't open input/output file\n");
        return 1; //indicates problem since not zero
    }

    //Makes default struct for things to be added to
    struct Comic_List myComicList;
    myComicList.size = 10; //Initial size of the comic list
    myComicList.count = 0; //Initially, no comics in the list
    myComicList.list = (struct Comic*)malloc(myComicList.size * sizeof(struct Comic));

    //Makes wish list/buy list struct for things to be added to
    struct Comic_List myComicBuyList;
    myComicBuyList.size = 10; //Initial size of the comic buy list
    myComicBuyList.count = 0; //Initially, no comics in the buy list
    myComicBuyList.list = (struct Comic*)malloc(myComicBuyList.size * sizeof(struct Comic));

    char cmd[10]; //Buffer to store the command

    //Read commands from input file
    while (fscanf(in, "%s ", cmd) != EOF) {
        if (cmd[0] == 'l') { //Load command -- loading the comic lisst
            char filename[256]; //to obtain the name of the csv file
            fscanf(in, "%s", filename); //actually obtains it
            FILE* loaded_file = fopen(filename, "r"); //converts into a file
           load(loaded_file, out, &myComicList, filename);
           fclose(loaded_file);
        } else if (cmd[0] == 'b') { //Buy command -- makes buy list
            int index; //used to store the index
            fscanf(in, "%d", &index); //obtains the index
            buy(out, &myComicList, &myComicBuyList, index);
        } else if (cmd[0] == 'c' && cmd[1] == 'h') {//Checkout command -- checkouts and purchases comics from this list
            checkout(out, &myComicBuyList);
        } else if (cmd[0] == 'd') { //Display command - displays comics in list
            display(out, myComicList);
        } else if (cmd[0] == 's') { //Save command -- saves list, writes, and outputs it
            char filename[256]; //used to store filename, assumeing 256 length
            fscanf(in, "%s", filename); //obtains filename
            save(out, &myComicList, filename);
        } else if (cmd[0] == 'c' && cmd[1] == 'l') { //Clear command -- empties list
           clear(out, &myComicList);
        } else if (cmd[0] == 'f') { //Find command -- finds comic at given index
            int index; //used to store the index
            fscanf(in, "%d", &index); //obtains the index
            find(out, &myComicList, index);
        } else if (cmd[0] == 'r') { //Remove Command -- Removes comic at given index
            int index; //used to store the index
            fscanf(in, "%d", &index); //obtains the index
            remove_comic(out, &myComicList, index);
        } else {

        }
    }

    //Close input and output files
    fclose(in);
    fclose(out);

    //Free memory allocated for each comic in the default list
    for (int i = 0; i < myComicList.count; i++) {
        free(myComicList.list[i].date);
        free(myComicList.list[i].code);
        free(myComicList.list[i].publisher);
        free(myComicList.list[i].title);
        free(myComicList.list[i].cost);
    }

    //Free memory allocated for the comic list
    free(myComicList.list);
    myComicList.list = NULL;
    free(myComicBuyList.list);
    myComicBuyList.list = NULL;

    return 0;
}