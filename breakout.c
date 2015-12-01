//
// breakout.c
//
// Computer Science 50
// Problem Set 3
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Stanford Portable Library
#include <spl/gevents.h>
#include <spl/gobjects.h>
#include <spl/gwindow.h>

// height and width of game's window in pixels
#define HEIGHT 600.0
#define WIDTH 400.0

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

#define VELOCITY 5
// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRoundRect initPaddle(GWindow window , double d , double x);
GLabel initScoreboard(GWindow window);
GLabel initLives(GWindow window);
void updateLives(GWindow window , GLabel life , int lives);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(int argc , string argv[])
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

	// number of bricks initially
    int bricks = COLS * ROWS;
    
    // instantiate paddle, centered at bottom of window
    GRoundRect paddle = initPaddle(window,0.5+ bricks*1.0/100 , WIDTH/2 - WIDTH/12);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);
	GLabel life =  initLives(window);
/*	GLabel time = newGLabel("");
	setFont(time , "SansSerif-18");
	add(window , time);
*/	
	//clock_t start , end;
    

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
	double xvelocity = drand48()*VELOCITY;
	double yvelocity;
	
	if(drand48() > 0.5)
		yvelocity = sqrt(VELOCITY*VELOCITY - xvelocity*xvelocity);
	else
		yvelocity = -sqrt(VELOCITY*VELOCITY - xvelocity*xvelocity);	
	
	updateScoreboard(window , label , 0);
	updateLives(window , life , LIVES);
	
	waitForClick();
	
	//start = clock();
	//char c[10];
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
		double m = (100 - bricks)*1.0/50;
		double x = getX(paddle);
		if(argc == 1)	
		{
			GEvent event = getNextEvent(MOUSE_EVENT);
		
			if(event != NULL)
			{
				if(getEventType(event) == MOUSE_MOVED)
				{
					x = getX(event) - getWidth(paddle)/2;
					if(x > 0 && x < WIDTH - getWidth(paddle))
					setLocation(paddle , x , HEIGHT-50);
				}
			}
		}
		else if(strcmp(argv[1],"GOD")==0)
		{
			if((int)(drand48()*100)%2==0)
				setLocation(paddle , getX(ball)+RADIUS - getWidth(paddle)/2 -getWidth(paddle)/2*drand48(), HEIGHT-50);
			else
				setLocation(paddle , getX(ball)+RADIUS - getWidth(paddle)/2 +getWidth(paddle)/2*drand48(), HEIGHT-50);
		}		
		else 
		{
			printf("Wrong command-line input");
			return 1;
		}	
		
		move(ball , xvelocity , yvelocity);
		
		if(getX(ball) < 0 || getX(ball) + 2*RADIUS > WIDTH)
		{
			xvelocity = -xvelocity;
			if(getX(ball)<0)
				setLocation(ball , 1 , getY(ball));
			else
				setLocation(ball , WIDTH - 2*RADIUS -1 , getY(ball));
		}
		
		if(getY(ball) < 0)
			yvelocity = -yvelocity;
		
		if(getY(ball) > HEIGHT)
		{
			lives--;
			updateLives(window , life , lives);
			if(lives == 0)
				break;
			ball = initBall(window);
			
			if(argc == 2 && strcmp(argv[1],"GOD")==0)
			{
				removeGWindow(window,paddle);
				paddle = initPaddle(window ,0.5+ bricks*1.0/100, x);
			}
			else
				setLocation(paddle, WIDTH/2 - WIDTH/12 , HEIGHT -50);
			waitForClick();
			
			xvelocity = drand48()*VELOCITY*m;
			
			if(drand48() > 0.5)
				yvelocity = sqrt(VELOCITY*VELOCITY - xvelocity*xvelocity/m/m)*m;
			else
				yvelocity = -sqrt(VELOCITY*VELOCITY - xvelocity*xvelocity/m/m)*m;	
		}
		
		
		//end = clock();
		//printf("%li\n" ,start);
		//sprintf(c , "Time - %li" , (end-start)/CLOCKS_PER_SEC);
		//setLabel(time , c);
		//setLocation(time , 300 , 30);
		
		GObject obj = detectCollision(window , ball);
		
		if(obj == paddle)
		{
			xvelocity = (getX(ball) + RADIUS -getX(paddle) - (0.5+bricks*1.0/100)*WIDTH/12)/10*m;
			yvelocity = -sqrt(VELOCITY*VELOCITY - xvelocity*xvelocity/m/m)*m;		
			setLocation(ball , getX(ball) , HEIGHT-50-2*RADIUS);
		}
		else if(obj!= NULL && strcmp(getType(obj) , "GRoundRect") == 0)
		{
			if(getY(obj) > 115)
				points += 10;
			else if( getY(obj) > 100)
				points += 15;
			else if(getY(obj) > 85)
				points += 20;
			else if(getY(obj) > 70)
				points += 25;
			else if(getY(obj) > 55)
				points += 30;
			
			if( getX(ball) + 2*RADIUS <= getX(obj) || getX(ball) > getX(obj) + 34 )
			{
				xvelocity = -xvelocity;
				if(getX(ball) + 2*RADIUS <= getX(obj))
					setLocation(ball , getX(obj) - 2*RADIUS , getY(ball));
				else
					setLocation(ball , getX(obj) +35 , getY(ball));	
			}
			else 
				yvelocity = -yvelocity;
				
			removeGWindow(window , obj);
			bricks--;
			updateScoreboard(window , label , points);
			removeGWindow(window , paddle);
			paddle = initPaddle(window , 0.5+ bricks*1.0/100 , x);
			
		}
		
		pause(10);		
        
    }
	
	
	removeGWindow(window , label);
	
	GLabel score = newGLabel("");
	setFont(score , "SansSerif-24");
	add(window , score);
	char s[20];
	sprintf(s , "Score - %i" , points);
	setLabel(score , s);
	setLocation(score , (WIDTH - getWidth(score))/2 , 200);
	
	GLabel lipoint = newGLabel("");
	setFont(lipoint , "SansSerif-24");
	add(window , lipoint);
	sprintf(s , "Life Bonus - %i" , lives*100);
	setLabel(lipoint , s);
	setLocation(lipoint , (WIDTH - getWidth(lipoint))/2 , 250);
	
	GLabel total = newGLabel("");
	setFont(total , "SansSerif-36");
	add(window , total);
	sprintf(s , "Total - %i" , points + lives*100);
	setLabel(total , s);
	setLocation(total , (WIDTH - getWidth(total))/2 , 350);
	setColor(total , "RED");
	
    // wait for click before exiting
    waitForClick();
    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    for(int i=0;i<COLS;i++)
    {
		for(int j=0;j<ROWS;j++)
		{
			GRoundRect r=malloc(sizeof(newGRoundRect));
			r=newGRoundRect(2.5+WIDTH/10*i,50+j*15,35,10,5);
			setFilled(r,true);
			switch(j)
			{
				case 1: setColor(r,"BLUE");
						break;
				case 2: setColor(r,"CYAN");
						break;
				case 3: setColor(r,"GREEN");
						break;
				case 4: setColor(r,"MAGENTA");
						break;
				case 0: setColor(r,"YELLOW");
						break;
			}
			add(window,r);
		}
	}
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval ball = newGOval(WIDTH/2-10,HEIGHT/2-10,2*RADIUS,2*RADIUS);
    add(window ,ball);
	setFilled(ball,true);
	setColor(ball,"BLACK");
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRoundRect initPaddle(GWindow window, double d , double x)
{
    GRoundRect paddle = newGRoundRect(x,HEIGHT-50,WIDTH*d/6,10,5);
    add(window,paddle);
    setFilled(paddle,true);
    setColor(paddle,"BLACK");
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel label = newGLabel("");
    setFont(label, "SansSerif-36");
    add(window, label);
    return label;
}

GLabel initLives(GWindow window)
{
	GLabel life = newGLabel("");
	setFont(life, "SansSerif-18");
	setColor(life , "RED");
	add(window , life);
	return life;
}

void updateLives(GWindow window , GLabel life , int lives)
{
	char s[10];
	sprintf(s , "Life - %i" , lives);
	setLabel(life , s);
	double x = (getWidth(window) - getWidth(life))/2; 
	setLocation(life , x , 30);
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
