#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curses.h>

#define SHAPES 7
#define N 4
#define COLS 15
#define ROWS 20

#define MAX(a,b) (((a) > (b)) ? (a) : (b))

const char shapes[SHAPES][N][N] = {
    {
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 1, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }
};

char table[ROWS][COLS];
char shape[N][N];
int x, y;

void print_table(void)
{
    move(0, 0);
    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            if (table[row][col])
                printw("%c ", '#');
            else
                printw("%c ", '.');
        }
        printw("\n");
    }
}

void print_shape(void)
{
    for (int i = 0; i < N; i++)
    {
        int row = y+i;

        if (row < 0) continue;

        for (int j = 0; j < N; j++)
        {
            int col = x+j;

            if (shape[i][j])
            {
                move(row, col*2);
                printw("%c ", '#');
            }
        }
    }
}

void finish_game(void)
{
    print_table();
    print_shape();
    move(ROWS, 0);
    printw("Game Over! Press any key to end program.");
    refresh();
    getchar();
}

void select_shape(void)
{
    memcpy(shape, shapes[rand() % SHAPES], sizeof(shape));
    x = (COLS-N)/2+1;
    y = -1;
}

int can_move(char (*new_shape)[N], int new_x, int new_y)
{
    for (int i = 0; i < N; i++)
    {
        int row = new_y+i;

        for (int j = 0; j < N; j++)
        {
            int col = new_x+j;

            if (new_shape[i][j])
            {
                if (col < 0 || col >= COLS)
                    return 0;

                if (row >= ROWS || table[row][col])
                    return 0;
            }
        }
    }

    return 1;
}

void rotate_shape(void)
{
    int i, j, w = 0;

    // find the maximum width/height of the shape
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            if (shape[i][j])
                w = MAX(w, MAX(i+1, j+1));
        }
    }

    char new_shape[N][N] = {0,};

    // rotate the new shape 90 degree clockwise
    for (i = 0; i < w; i++)
    {
        for (j = 0; j < w; j++)
        {
            new_shape[i][w-j-1] = shape[j][i];
        }
    }

    if (can_move(new_shape, x, y))
    {
        memcpy(shape, new_shape, sizeof(shape));
    }
}

void place_shape(void)
{
    for (int i = 0; i < N; i++)
    {
        int row = y+i;

        for (int j = 0; j < N; j++)
        {
            int col = x+j;

            if (shape[i][j])
            {
                table[row][col] = shape[i][j];
            }
        }
    }
}

void clear_lines(void)
{
    int filled, i;

    for (int row = 0; row < ROWS; row++)
    {
        // check if this row makes a line
        filled = 0;
        for (int col = 0; col < COLS; col++)
        {
            if (table[row][col]) filled++;
        }

        if (filled == COLS)
        {
            // remove the line
            for (i = row; i >= 1; i--)
                memcpy(table[i], table[i-1], sizeof(table[i]));
            memset(table[i], 0, sizeof(table[i]));
        }
    }
}

int main(void)
{
    int ch;

    // initialize curses
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    curs_set(0);
    timeout(1);

    srand(time(NULL));

    select_shape();

    time_t old = time(NULL);

    do {
        print_table();
        print_shape();

        ch = getch();

        switch (ch) {
        case KEY_UP:
            rotate_shape();
            break;
        case KEY_DOWN:
            if (can_move(shape, x, y+1)) y++;
            break;
        case KEY_LEFT:
            if (can_move(shape, x-1, y)) x--;
            break;
        case KEY_RIGHT:
            if (can_move(shape, x+1, y)) x++;
            break;
        case ' ':
            while (can_move(shape, x, y+1)) y++;
            old = 0; // this causes a timeout
            break;
        }

        // move shape down every 1.0 second
        time_t cur = time(NULL);
        if (difftime(cur, old) >= 1.0)
        {
            if (can_move(shape, x, y+1)) y++;
            else
            {
                place_shape();
                clear_lines();
                select_shape();

                // game over?
                if (!can_move(shape, x, y))
                {
                    finish_game();
                    break;
                }
            }
            old = cur;
        }
    } while (ch != 'q');

    // end curses
    endwin();

    return 0;
}
