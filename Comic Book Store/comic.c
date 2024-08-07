#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comic.h"

/////////////////////////////////////////////////Load Function///////////////////////////////////////////////////////////////////
/*
Reads in each line of the CSV file and store it into a Comic. 
That Comic should then be put into the comicList representing all of the comics in the current CSV file.
*/
void load(FILE* in, FILE* out, struct Comic_List* comicList, char* filename) {
    fprintf(out, "Command: load %s\n", filename); //Prints out the command used

    char line[1024]; //Just saying that the max length is 1024 charcters
    int comicsLoaded = 0; //Amount of comics loaded from a csv file

    //Skips the first line detailing the format of the csv file, while also notifying if there is
    //an empty file. (first line is null)
    if (fgets(line, sizeof(line), in) == NULL) { 
        //Handles error in file if there is one
        fprintf(out, "File is empty.\n");
        return;
    }

    //Processes the rest of the data in the csv file
    while (fgets(line, sizeof(line), in) != NULL) {
        //Skip empty blank lines
            if (strlen(line) <= 1) { //so this is lines less than length 1
                continue;
            }

        //Remove the trailing newline character if present
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        //Creates a comic struct
        struct Comic newComic;

        //Buffers for the 5 fields of a comic struct
        char dateBuffer[256]; //Buffer for the date
        char codeBuffer[256]; //Buffer for the code
        char publisherBuffer[256]; //Buffer for the publisher
        char titleBuffer[1024]; //Buffer for the title
        char costBuffer[256]; //Buffer for the cost

        //Parse line and stores fields into corresponding buffer
            //"%[^,]" reads characters until a comma is encountered
        //The sscanf function returns number of successful conversions
        //If it does not match the expected count which is 5, then there is a parsing error
            //Which prompts a print error statement
        if (sscanf(line, "%[^,],%[^,],%[^,],%[^,],%s", dateBuffer, codeBuffer, publisherBuffer, titleBuffer, costBuffer) != 5) {
            // fprintf(out, "Can't parse line.\n");
            // continue;
        }

        //Cost buffer without $ symbol and trailing whitespace
        char* fixedCost = costBuffer;
        while (*fixedCost && (*fixedCost == ' ' || *fixedCost == '$')) {
            fixedCost++;
        }

        //Allocate (with malloc) memory for each field and copies the content
        newComic.date = malloc(strlen(dateBuffer) + 1);
        strcpy(newComic.date, dateBuffer);

        newComic.code = malloc(strlen(codeBuffer) + 1);
        strcpy(newComic.code, codeBuffer);

        newComic.publisher = malloc(strlen(publisherBuffer) + 1);
        strcpy(newComic.publisher, publisherBuffer);

        newComic.title = malloc(strlen(titleBuffer) + 1);
        strcpy(newComic.title, titleBuffer);

        newComic.cost = malloc(strlen(fixedCost) + 1);
        strcpy(newComic.cost, fixedCost);

        //Check if the comic List needs to be resized
        if (comicList->count == comicList->size) {
            comicList->size *= 2; // Increase size by 10 (or any other suitable value)
            comicList->list = realloc(comicList->list, comicList->size * sizeof(struct Comic));
        }

        //Add the new comic to the Comic List
        comicList->list[comicList->count] = newComic;
        comicList->count++;
        comicsLoaded++;

    }
    
    //Prints out number of comics loaded
    fprintf(out, "\tNumber of comics: %d\n", comicsLoaded);

}

/////////////////////////////////////////////////Buy Function///////////////////////////////////////////
/*
Makes a purchaseList, almost like a wish list for comics that are desired for purchase from the csv file
*/
void buy(FILE* out, struct Comic_List* entireComicList, struct Comic_List* purchaseList, int position) {
    fprintf(out, "Command: buy %d\n", position); //Prints out the command used and the position
    
    //Check if the position is okay to use(too small or too large position
    if (position < 0 || position >= entireComicList->count) {
        fprintf(out, "Unable to buy comic #%d\n", position); //Prints this if not okay
        return;
    }

    //Checks if the purchaseList needs to be resized
    if (purchaseList->count == purchaseList->size) {
        //Doubles the size of the array
        purchaseList->size *= 2;
        purchaseList->list = realloc(purchaseList->list, purchaseList->size * sizeof(struct Comic));
        if (purchaseList->list == NULL) {
            printf("Reallocation failed.\n");
            return;
        }
    }

    //Performs deep copy of the comic at the specified position
    struct Comic* newComic = &(entireComicList->list[position]);
    struct Comic* boughtComic = &(purchaseList->list[purchaseList->count]);
    boughtComic->date = malloc(strlen(newComic->date) + 1); //allocate memory for the string (+1 for null terminator)
    strcpy(boughtComic->date, newComic->date); //Copy the string

    boughtComic->code = malloc(strlen(newComic->code) + 1);
    strcpy(boughtComic->code, newComic->code);

    boughtComic->publisher = malloc(strlen(newComic->publisher) + 1);
    strcpy(boughtComic->publisher, newComic->publisher);

    boughtComic->title = malloc(strlen(newComic->title) + 1);
    strcpy(boughtComic->title, newComic->title);

    boughtComic->cost = malloc(strlen(newComic->cost) + 1);
    strcpy(boughtComic->cost, newComic->cost);

    //Count of comics on the purchaseList increases
    purchaseList->count++;

    fprintf(out, "Comic #%d added to purchase list\n", position); //Prints out the comic added and its position to the purchase list
}

/////////////////////////////////////////////////Checkout Function/////////////////////////////////////////////////////////////////
/*
Checkout purchase all of the comics on the buy/purchase list. It prints all the comics and a receipt with the subtotal, 
the tax (5%), and the total after tax. Comics with "AR" listed as the price will still be printed in the output file 
but will not be added to the subtotal of the purchase.
*/
void checkout(FILE* out, struct Comic_List* purchaseList){
    fprintf(out, "Command: checkout\n"); //Prints out the command used
    fprintf(out, "Comics in Purchase List\n"); //Prints out the comics in purchase list

    double subtotal = 0.0; //Accumulating subtotal for the comics
    if (purchaseList->count == 0) { //Check if the purchase list is empty
        fprintf(out, "There are no comic books in the purchase list.\n");
    } else {
        for (int i = 0; i < purchaseList->count; i++) { //Print all the comics in the purchase list and their information
            fprintf(out, "Comic Number: %d\n", i + 1);
            fprintf(out, "\tDate: %s\n", purchaseList->list[i].date);
            fprintf(out, "\tCode: %s\n", purchaseList->list[i].code);
            fprintf(out, "\tPublisher: %s\n", purchaseList->list[i].publisher);
            fprintf(out, "\tTitle: %s\n", purchaseList->list[i].title);
            if (strcmp(purchaseList->list[i].cost, "AR") != 0) { //If the cost is not AR print it with $
                double notARmoney = atof(purchaseList->list[i].cost); //Since this is not an AR value the char* will become a double
                fprintf(out, "\tCost: $%s\n", purchaseList->list[i].cost);
                subtotal += notARmoney; //This double will be added to the subtotal
            } else {
                fprintf(out, "\tCost: %s\n", purchaseList->list[i].cost); //Else print without $ (AR values)
            }
        }

        
        //Free memory allocated for each comic in the buy list
        for (int i = 0; i < purchaseList->count; i++) {
            free(purchaseList->list[i].date);
            free(purchaseList->list[i].code);
            free(purchaseList->list[i].publisher);
            free(purchaseList->list[i].title);
            free(purchaseList->list[i].cost);
        }
        //Resets info about the purchaseListt
        purchaseList->count = 0;
        purchaseList->size = 0;
    }

    double tax = subtotal * 0.05; //Calculate tax --> 5% of subtotal
    double total = subtotal + tax; //Calculate total

    //Prints out the subtotal, tax, and total
    fprintf(out, "Subtotal:  $%.2f\n", subtotal);
    fprintf(out, "     Tax:  $%.2f\n", tax);
    fprintf(out, "   Total:  $%.2f\n", total);
}

/////////////////////////////////////////////////Display Function/////////////////////////////////////////////////////////////////
/*
Display each comic from the Comic List (comicList) into the output file.  
Be sure to reference how to print both when the list is empty and when the list contains entries.
*/
void display(FILE* out, struct Comic_List comicList) {
    fprintf(out, "Command: display\n"); //Prints out the command used
    if (comicList.count == 0) { //Prints out when empty
        fprintf(out, "List is currently empty.\n");
    }
    for (int i = 0; i < comicList.count; i++) { //Prints all the comics in the list and the information about them
        fprintf(out, "Comic Number: %d\n", i + 1);
        fprintf(out, "\tDate: %s\n", comicList.list[i].date);
        fprintf(out, "\tCode: %s\n", comicList.list[i].code);
        fprintf(out, "\tPublisher: %s\n", comicList.list[i].publisher);
        fprintf(out, "\tTitle: %s\n", comicList.list[i].title);
        if (strcmp(comicList.list[i].cost, "AR") != 0) { //If the cost is not AR print it with $
            fprintf(out, "\tCost: $%s\n", comicList.list[i].cost);
        } else {
           fprintf(out, "\tCost: %s\n", comicList.list[i].cost);  //Else print without $ (AR values)
        }
    }
}

/////////////////////////////////////////////////Save Function/////////////////////////////////////////////////////////////////
/*
Save will write the current list of comics into the file that is given. 
The format should match the format of the original CSV file.
*/
void save(FILE* out, struct Comic_List* comicList, char* filename) {
    fprintf(out, "Command: save %s\n", filename); //Prints out the command used

    //Open file to write to
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(out, "Unable to open file to write to.\n");
        return;
    }

    //Write the first line of the other csv files to say the format
    fprintf(file, "DATE,CODE,PUBLISHER,TITLE,PRICE\n");

    //Writes each comic from the Comic List to the file
    for (int i = 0; i < comicList->count; i++) {
        if (strcmp(comicList->list[i].cost, "AR") != 0) { //If the cost is not AR print it with $
            fprintf(file, "%s,%s,%s,%s,$%s\n", comicList->list[i].date, comicList->list[i].code, comicList->list[i].publisher, comicList->list[i].title, comicList->list[i].cost);
        } else {
        fprintf(file, "%s,%s,%s,%s,%s\n", comicList->list[i].date, comicList->list[i].code, comicList->list[i].publisher, comicList->list[i].title, comicList->list[i].cost);
        }
    }

    //Closes the file
    fclose(file);
}

/////////////////////////////////////////////////Clear Function/////////////////////////////////////////////////////////////////
/*
Empties list
*/
void clear(FILE* out, struct Comic_List* comicList) {
    fprintf(out, "Command: clear\n"); //Prints out the command used

    //Free memory for every comic on list
    for (int i = 0; i < comicList->count; i++) {
        free(comicList->list[i].date);
        free(comicList->list[i].code);
        free(comicList->list[i].publisher);
        free(comicList->list[i].title);
        free(comicList->list[i].cost);
    }
    //sets the count to 0 because there are no comics in the list
    comicList->count = 0;
    comicList->size = 10;
}

/////////////////////////////////////////////////Find Function/////////////////////////////////////////////////////////////////
/*
Finds comic at given index. If not a valid index says its not at that index. If it is at the index
it outputs information about the comic.
*/
void find(FILE* out, struct Comic_List* comicList, int index) {
    fprintf(out, "Command: find %d\n", index); //Prints out the command used along with what index should be removed

    //I check if the given index is valid
    if (index < 0 || index >= comicList->count) {
        fprintf(out, "There is no comic at index #%d in the list.\n", index);
        return;
    }

    //Display information about the comic at the given index
    fprintf(out, "\tDate: %s\n", comicList->list[index].date);
    fprintf(out, "\tCode: %s\n", comicList->list[index].code);
    fprintf(out, "\tPublisher: %s\n", comicList->list[index].publisher);
    fprintf(out, "\tTitle: %s\n", comicList->list[index].title);
    if (strcmp(comicList->list[index].cost, "AR") != 0) { //If the cost is not AR print it with $
        fprintf(out, "\tCost: $%s\n", comicList->list[index].cost);
    } else {
        fprintf(out, "\tCost: %s\n", comicList->list[index].cost);  //Else print without $ (AR values)
    }
}

/////////////////////////////////////////////////Remove Function/////////////////////////////////////////////////////////////////
/*
Removes comic at the the index (if it is a proper index). Then it shifts all the comics above it downward (so no gap)
Can't be named remove because there is a function in C already named that. So i named it remove_comic, since that's what it does
*/
void remove_comic(FILE* out, struct Comic_List* comicList, int index) {
    fprintf(out, "Command: remove %d\n", index); // Print out the command used

    //Check if given index is good/valid
    if (index < 1 || index > comicList->count) {
        fprintf(out, "Comic at index %d was not removed\n", index);
        return;
    }

    //Free memory of the information for the comic at given index
    free(comicList->list[index].date);
    comicList->list[index].date = NULL;
    free(comicList->list[index].code);
    comicList->list[index].code = NULL;
    free(comicList->list[index].publisher);
    comicList->list[index].publisher = NULL;
    free(comicList->list[index].title);
    comicList->list[index].title = NULL;
    free(comicList->list[index].cost);
    comicList->list[index].cost = NULL;

    //If the comic to remove is not the last one, shift elements down
    if (index < comicList->count - 1) {
        for (int i = index; i < comicList->count - 1; i++) {
            comicList->list[i] = comicList->list[i + 1];
        }
    }

    //Decrement the count of comics in the list
    comicList->count--;

    fprintf(out, "Comic at index %d successfully removed\n", index); //Print out the comic index removed from
}
