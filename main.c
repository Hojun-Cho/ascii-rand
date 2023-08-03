#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <curses.h>
#include <stdio.h>
#include <time.h>  
#include <signal.h>

typedef struct
{
	int w;
	int h;
	int speed;
	int color;
	char shape;
} drop_t;

typedef struct
{
	drop_t *drops;
	int size;
	int cap;
} vector_t;

int	pRand(int min, int max)
{
	max -= 1;
	return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

void v_add(vector_t *v, drop_t d);

void exit_err(const char *err)
{
    /*exitCurses();*/
	perror(err);
    exit(1);
}

drop_t new_drop()
{
	drop_t d;

	d.w = pRand(0, COLS);
	d.h = pRand(0, LINES);
	d.speed = pRand(1, 3);
	d.shape = pRand(32, 126);
	d.color = pRand(0, 255);
	return (d);
}

void v_init(vector_t *v, uint32_t cap)
{
	assert (cap != 0);

	v->drops = malloc(sizeof(drop_t) * cap);
	if (v->drops == NULL)
		exit_err("can't alloc");
	v->size = 0;
	v->cap = cap;	
}

void v_free(vector_t *v)
{
	if (v->drops != 0)
	{
		free(v->drops);
		v->drops = 0;
	}

	v->size = 0;
	v->cap = 0;
}

void v_resize(vector_t *v, uint32_t new_cap)
{
	v->drops = realloc(v->drops, (sizeof(drop_t) * new_cap));
	if (v->drops == NULL)
		exit_err("can't alloc");
	v->cap = new_cap;
	for (uint32_t i = v->size; i < new_cap; i++)
		v_add(v, new_drop());
}



void v_add(vector_t *v, drop_t d)
{
	if (v->size >= v->cap)
		v_resize(v, 2 * v->cap);
	v->drops[v->size] = d;
	v->size++;
}

drop_t *v_getAt(vector_t *v, uint32_t pos)
{
	assert ((pos < v->size));

	return v->drops + pos;
}

void handleResize()
{
	endwin();
	refresh();
	erase();
}

void init_curses()
{
	initscr();
	noecho();
	cbreak();
	keypad(stdscr, 1); 
	curs_set(0);
	timeout(10);
	signal(SIGWINCH, handleResize);

	if (!has_colors() || !can_change_color())
		exit_err("can't change color");
	
	use_default_colors();
	start_color();
	for (uint16_t i = 0; i < COLORS; i++)
		init_pair(i + 1, i, -1);
}

void exit_curses_()
{
	curs_set(1);
	clear();
	refresh();
	resetty();
	endwin();
}

void d_fall(drop_t *d)
{
	if ((d->h += d->speed) >= LINES - 1)
		d->h = pRand(0, 10);	
}

void d_show(drop_t *d)
{
	attron(COLOR_PAIR(d->color));
	mvaddch(d->h, d->w, d->shape);
}

int main()
{
	srand((uint32_t) getpid());
	init_curses();

	while (1)
	{
		uint32_t drops_total = (uint32_t) (COLS * 2.0);
		vector_t drops;
		v_init(&drops, drops_total);
		for (uint32_t i = 0; i < drops_total; i++)
			v_add(&drops, new_drop());

		for (int i = 0; i < drops_total; i++)
		{
			d_fall(v_getAt(&drops, i));
			d_show(v_getAt(&drops, i));
		}

		sleep(1);

		if (wgetch(stdscr) == 'q')
			break;	
	}

	exit_curses_();
}
