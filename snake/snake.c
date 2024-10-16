#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    int  x, y ;
    bool valid;
} apple;

typedef struct segment {
    char            c                      ;
    int             y, x, new_x, new_y, dir;
    struct segment* next                   ;
} snekSegment;

void           start_screen();
int            readBestScore();
void           writeBestScore(int score);
void           makeGround(int apples_eaten, int best_score);
apple*         makeApple(snekSegment* head);
void           printApple(apple* this_apple);
snekSegment*   makeInitialSnek(int snake_size);
snekSegment*   addSegment(snekSegment* tail);
void           updateSnek(snekSegment* head);
snekSegment*   getTail(snekSegment* head);
void           freeSnek(snekSegment* head);
bool           collision(snekSegment* head);
void           game_over(int score, int best_score);

#define SCORE_FILE "best_score.txt"
int border_left_x  ;
int border_right_x ;
int border_top_y   ;
int border_bottom_y;

int initial_snake_size = 7;

int main(void)
{
    initscr();   // LINES and COLS initialized
    nodelay(stdscr, TRUE);
    curs_set(0); // make cursor invisible in stdscr
    cbreak();    // so ctrl+c interrupts
    keypad(stdscr, TRUE);
    noecho();
    #define border_left_x 0
    #define border_right_x (COLS - 1)
    #define border_top_y 0
    #define border_bottom_y (LINES - 2)

    start_screen();
    int best_score = readBestScore();
    makeGround(0, best_score);
    
    srand(time(NULL));
    snekSegment* head = makeInitialSnek(initial_snake_size);
    snekSegment* tail = getTail(head)                      ;
    apple* Apple      = makeApple(head)                    ;
    printApple(Apple);
    refresh();

    int ch;
    int apples_eaten     = 0     ;
    int speed            = 50000 ;
    // GAME LOOP:
    while (true)
    {
        //check collision
        if (collision(head))
        {
            int score = apples_eaten;
            apples_eaten = 0;
            free(Apple);
            freeSnek(head);

            game_over(score, best_score);
            head = makeInitialSnek(initial_snake_size);
            tail = getTail(head);
            Apple = makeApple(head);
        }

        ch = getch();
        if (ch == 'q' || ch == 'Q')
            break;
        else if 
            ((ch  ==  KEY_LEFT  && head->dir  !=  KEY_RIGHT && head->dir != KEY_LEFT )  ||
            ( ch  ==  KEY_RIGHT && head->dir  !=  KEY_LEFT  && head->dir != KEY_RIGHT)  ||
            ( ch  ==  KEY_UP    && head->dir  !=  KEY_DOWN  && head->dir != KEY_UP   )  ||
            ( ch  ==  KEY_DOWN  && head->dir  !=  KEY_UP    && head->dir != KEY_DOWN))
                head->dir = ch;
        // add segment if apple eaten
        if (head->x == Apple->x && head->y == Apple->y)
        {
            apples_eaten++;
            tail = addSegment(tail);

            mvaddch(Apple->y, Apple->x, ' ');
            free(Apple);
            Apple = makeApple(head);
        }
        // Check and update best score if necessary
        if (apples_eaten > best_score) 
        {
            best_score = apples_eaten;
            writeBestScore(best_score);
        }

        makeGround(apples_eaten, best_score);
        printApple(Apple);
        updateSnek(head);
        refresh();
        usleep(speed);
    }

    free(Apple);
    freeSnek(head);
    endwin();
}

void start_screen()
{
    char* msgg[] = {"SSSNEK GAME", "( Press ENTER to start, Q to Quit )"};
    char* msg;
    int ch;
    while ((ch = getch()) != '\n') // while its not the ENTER key
    {
        if (ch == 'q' || ch == 'Q') 
        {endwin();  exit(0);}      // end curses mode then quit the program

        for (int i = 0; i < 2; i++)
        {
            msg = msgg[i];
            mvprintw((LINES / 2) + i*2, (COLS - strlen(msg)) / 2, "%s", msg);
        }
    }
    refresh();
    erase(); // clear the messages
}

int readBestScore() 
{
    FILE *file = fopen(SCORE_FILE, "r");
    if (file == NULL) 
    {
        file = fopen(SCORE_FILE, "w");
        if (file != NULL)
        {
            int default_score = 0;
            fprintf(file, "%d", default_score);
            fclose(file);
            /*fscanf reads from a file that is open in read mode. 
            If you write to the file and then try to read from it 
            without closing and reopening, the file pointer will 
            still be at the end of the file. Therefore, you won’t read the expected value.*/
        }
        else
            return 0;
        // reopen file for reading
        file = fopen(SCORE_FILE, "r");
        if (file == NULL) {return 0;}
    }
    
    int best_score;
    fscanf(file, "%d", &best_score);
    fclose(file);
    return best_score;
}

void writeBestScore(int score) 
{
    FILE *file = fopen(SCORE_FILE, "w");
    if (file == NULL) 
    {
        perror("Unable to open score file"); 
        return;
    }
    
    fprintf(file, "%d\n", score);
    fclose(file);
}

void makeGround(int apples_eaten, int best_score)
{
    /*
    mvhline(0, 0, '-', COLS);             //top
    mvhline(LINES - 2, 0, '-', COLS);     // bottom
    mvvline(1, 0, '!', LINES - 2);        //left upyo y = LINES - 2
    mvvline(1, COLS - 1, '!', LINES - 2); // right upto y = LINES - 2 and starts at x = COLS - 1    
    */
    
    mvprintw(LINES - 1, 0, "Press Q to exit |");
    printw(" APPLES EATEN: %i |", apples_eaten);
    printw(" BEST SCORE: %d |", best_score);
}

apple* makeApple(snekSegment* head)
{
    int x,y;
    int i = 0;
    int size = (border_right_x - border_left_x - 1) * (border_bottom_y - border_top_y - 1);
    apple positions[size];
    for (x = border_left_x + 1; x < border_right_x; x++)
    {
        for (y = border_top_y + 1; y < border_bottom_y; y++)
        {
            positions[i].x = x;
            positions[i].y = y;
            positions[i].valid = true;
            i++;
        }
    }

    snekSegment* current = head;
    snekSegment* next = current->next;
    // make the positions of the segments invalid
    while (current != NULL)
    {
        for (i = 0; i < size; i++)
        {
            if (current->x == positions[i].x && current->y == positions[i].y)
            {
                positions[i].valid = false;
            }
        }
        current = current->next;
    }

    i = rand() % (size);
    while (positions[i].valid == false)
    {
        i = rand() % (size);
    }
    apple* new_apple = malloc(sizeof(apple));
    if (new_apple == NULL)
    {
        return NULL;
    }
    new_apple->x = positions[i].x;
    new_apple->y = positions[i].y;
    return new_apple;
}

void printApple(apple* this_apple)
{
    if (this_apple == NULL) {return;}
    start_color();
    use_default_colors();        // to use the transparent background properly
    init_pair(1, COLOR_RED, -1); // -1 indicates transparent background

    attron(COLOR_PAIR(1));
    mvaddch(this_apple->y, this_apple->x, '0');
    attroff(COLOR_PAIR(1));
}

snekSegment* makeInitialSnek(int snake_size)
{
    snekSegment* head = malloc(sizeof(snekSegment));
    if (head == NULL) {return NULL;}
    head->c = '+';
    head->y = LINES / 2;
    head->x = (COLS - snake_size) / 2;
    head->dir = KEY_LEFT; // initially the snake moves left

    mvaddch(head->y, head->x, head->c);

    snekSegment* current = head;
    for (int i = 1; i < snake_size; i++)
    {
        snekSegment* new = malloc(sizeof(snekSegment));
        if (new == NULL) {freeSnek(head); return NULL;}
        current->next = new;
        current = new;
        current->c = 'o';
        current->y = LINES / 2;
        current->x = head->x + i;
        current->dir = KEY_LEFT;

        mvaddch(current->y, current->x, current->c);
    }
    current->next = NULL;
    return head;
}

snekSegment* addSegment(snekSegment* tail)
/* Remember to first add the new tail before generating the apple to avoid 
   conflicts with the new tail's position */
{
    snekSegment* new_tail = malloc(sizeof(snekSegment));
    if (new_tail == NULL) {return NULL;}

    tail->next = new_tail;
    new_tail->next = NULL;
    new_tail->c = tail->c;

    if (tail->dir == KEY_LEFT) {new_tail->y = tail->y; new_tail->x = tail->x + 1;}
    else if (tail->dir == KEY_RIGHT) {new_tail->y = tail->y;new_tail->x = tail->x - 1;}
    else if (tail->dir == KEY_UP) {new_tail->x = tail->x;new_tail->y = tail->y + 1;}
    else if (tail->dir == KEY_DOWN) {new_tail->x = tail->x;new_tail->y = tail->y - 1;}

    new_tail->dir = tail->dir;
    return new_tail;
}

void updateSnek(snekSegment* head)
{    
    if (head->dir == KEY_LEFT) {
        head->new_x = head->x - 1; 
        head->new_y = head->y;
        if (head->x <= border_left_x + 1) {head->new_x = border_right_x - 1;}
        }
    else if (head->dir == KEY_RIGHT) {
        head->new_x = head->x + 1; 
        head->new_y = head->y;
        if (head->x >= border_right_x - 1) {head->new_x = border_left_x + 1;}
        }
    else if (head->dir == KEY_UP) {
        head->new_y = head->y - 1; 
        head->new_x = head->x;
        if (head->y == border_top_y + 1) {head->new_y = border_bottom_y - 1;}
        }
    else if (head->dir == KEY_DOWN) {
        head->new_y = head->y + 1; 
        head->new_x = head->x;
        if (head->y == border_bottom_y - 1) {head->new_y = border_top_y + 1;}
        }

    snekSegment* current = head->next;
    snekSegment* previous = head;

    // first set up all the new locations of the other segments
    while (current != NULL)
    {
        current->new_x = previous->x;
        current->new_y = previous->y;
        current->dir = previous->dir;

        previous = current;
        current = current->next;
    }

    current = head;
    // move segments to new locations
    while(current != NULL)
    {
        mvaddch(current->y, current->x, ' ');
        current->x = current->new_x;
        current->y = current->new_y;
        mvaddch(current->y, current->x, current->c);

        current = current->next;
    }
}

snekSegment* getTail(snekSegment* head)
{
    snekSegment* current = head;
    while(current->next != NULL)
    {
        current = current->next;
    }
    return current; 
}

void freeSnek(snekSegment* head)
{
    snekSegment* current = head;
    snekSegment* new;
    while (current != NULL)
    {
        new = current->next;
        free(current);
        current = new;
    }
}

void game_over(int score, int best_score)
{
    erase();
    char* msgg[] = {
        "GAME OVER",
        "SCORE:",
        "BEST: ",
        "( Press R to Replay, Q to Quit )",
    };
    int ch; char* msg;
    while ((ch = getch()) != 'r' && ch != 'R')
    {
        if (ch == 'q' || ch == 'Q') {endwin(); exit(0);}

        for (int i = 0; i < 4; i++)
        {
            msg = msgg[i];
            mvprintw((LINES / 2) + i*2, (COLS - strlen(msg)) / 2, "%s", msg);
            if (i == 1) {printw("%d", score);}
            else if (i == 2) {printw("%d", best_score);}
        }
    }
    refresh();
    erase(); // clear the messages
}

bool collision(snekSegment* head)
{
    snekSegment* current = head->next;
    while(current != NULL)
    {
        if (head->x == current->x && head->y == current->y)
            return true;

        current = current->next;
    }
}