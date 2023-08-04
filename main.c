#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <curses.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>  
#include <signal.h>
#include <string.h>

int pRand(int min, int max)
{
    max -= 1;
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

typedef struct planet_s
{
	int	x;
	int	y;
	int	r;
}	planet_t;

void clear_map()
{
	for (int i = 0; i < LINES; i++)
		mvhline(i, 0, ' ', COLS);
}

void write_lines(int start, char *lines[])
{
	int max_len = 0;
	int count = 0;

	while (lines[count])
	{
		int line_len = strlen(lines[count]);
		if (line_len > max_len)
			max_len = line_len;
		count++;
	}
	for (int i = 0; i < count; i++)
		mvaddstr(LINES / 2 - (start - count - i),
				COLS / 2 - (max_len / 2),
				lines[i]);
}

void menu()
{
	clear_map();
	char *items[9] = {
        "+================+",
        "| Planet  Arcade |",
        "+================+",
        "",
        "Select a game or exit",
        "",
        "[0] Exit",
        "[1] Play",
        NULL
    };
	write_lines(12, items);
	refresh();
	sleep(3);
}

void	*food_thread()
{
	while (1)
	{
		for (int i = 0; i < 100; i++)
			mvaddch(pRand(0, LINES), pRand(0, COLS), '@');
		sleep(3);
	}
}

void	write_planet(planet_t *p)
{
	for (int i = 0; i < p->r; i++)
	{
		for (int j =0; j < p->r; j++)
		{
			mvaddch(p->y + i, p->x + j, '@');
		}
	}
}

void	clear_planet(planet_t *p)
{
	for (int i = 0; i < p->r; i++)
	{
		for (int j =0; j < p->r; j++)
		{
			mvaddch(p->y + i, p->x + j, ' ');
		}
	}
}


void	*user_thread()
{
	planet_t p = {.x = pRand(0, COLS), .y = pRand(0, LINES), .r = 1};
	planet_t bkp;
	write_planet(&p);
	refresh();
	while (1)
	{
		bkp = p;
		switch (getch())
		{
			case 'w':
				p.y -= 1;
				break;
			case 'a':
				p.x -= 1;
				break;
			case 's':
				p.y += 1;
				break;
			case 'd':
				p.r += 1;
				p.x += 1;
				break;	
		}
		clear_planet(&bkp);
		write_planet(&p);	
		refresh();
	}	
}

void run_game() {
    pthread_t threads[2];

    pthread_create(&threads[1], NULL, user_thread, NULL);

    pthread_join(threads[1], NULL);
}



int	choice()
{
	return (1);
}

int main()
{
	initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);
    start_color();
	while (1)
	{
		menu();
		clear_map();
		refresh();
		switch (choice())
		{
			case 0:
				break;
			case 1:
				run_game();
				sleep(1000);
				break;
			default:
				endwin();
				exit(1);
		}
	}
	
	endwin();
}
