/*
Stores information about the comic such as the date, code, publisher, title, and cost of the comic
*/
struct Comic
{
    char* date;
    char* code;
    char* publisher;
    char* title;
    char* cost;
};

/*
A sturct that holds an array of Comic structs (list) as well as the maximum size for the array 
and a count of Comics currently in the array. 
*/
struct Comic_List
{
    struct Comic* list;
    int size;
    int count;
};

/*
All the functions that that comic.c can call
*/
void load(FILE* in, FILE* out, struct Comic_List* comicList, char* filename);
void buy(FILE* out, struct Comic_List* entireComicList, struct Comic_List* purchaseList, int position);
void checkout(FILE* out, struct Comic_List* purchaseList);
void display(FILE* out, struct Comic_List list);
void save(FILE* out, struct Comic_List* comicList, char* filename);
void clear(FILE* out, struct Comic_List* comicList);
void find(FILE* out, struct Comic_List* comicList, int index);
void remove_comic(FILE* out, struct Comic_List* comicList, int index);
