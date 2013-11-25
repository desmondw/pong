#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <string>
using namespace std;

float camEyeX=0.0, camEyeY=0.0, camEyeZ=1000.0, camLookX=0.0, camLookY=0.0, camLookZ=-1.0, camTiltX=0.0f, camTiltY=1.0f, camTiltZ=0.0f; //camers position/angle
int initCheck, ballMoveCheck, player1Score=0, player2Score=0, modifier, lastWin = 0;
float player1Loc, player2Loc, ballLocX, ballLocY, ballDirectX, ballDirectY;
bool canMovePaddle;

const float PADDLE_HEIGHT=200.0, PADDLE_WIDTH=50.0, PADDLE_SPEED=0.25, BALL_SIZE=30.0, BALL_SPEED=7;
const int BALL_MIN_ANGLE=15, BALL_MAX_ANGLE=75; //range is 1 - 89, must be different
const float BOUNDARY_HEIGHT=100.0, BOUNDARY_WIDTH=2000.0, TOP_BOUNDARY=900, BOTTOM_BOUNDARY=-900, LEFT_BOUNDARY=-950+PADDLE_WIDTH, RIGHT_BOUNDARY=950-PADDLE_WIDTH;
const int NUM_KEY_STATES = 256, NUM_KEY_STATES_SPECIAL=246;

int ballAngleRange = BALL_MAX_ANGLE - BALL_MIN_ANGLE;
int ballSpeed = BALL_SPEED;
bool keyState[NUM_KEY_STATES], keyStateSpecial[NUM_KEY_STATES_SPECIAL];
string title = "Pong! Player1:0 Player2:0";

void resetGame()
{
	canMovePaddle = true;
	initCheck=0;
	ballMoveCheck=0;
	ballSpeed = BALL_SPEED;
	player1Loc=100;
	player2Loc=100;
	
	int randomVertical;
	if (rand()%2 == 1)
		randomVertical = 1;
	else
		randomVertical = -1;

	if (lastWin == 0)
		lastWin == rand()%2;

	//randomize where ball begins and serve
	if (lastWin == 1)
	{
		ballLocX=RIGHT_BOUNDARY - 115;
		ballLocY=BALL_SIZE/2;
		ballDirectY=(float)(rand()%ballAngleRange+1+BALL_MIN_ANGLE)/90*randomVertical;
		ballDirectX=-(2-fabs(ballDirectY));
	}
	else
	{
		ballLocX=LEFT_BOUNDARY + 115;
		ballLocY=BALL_SIZE/2;
		ballDirectY=(float)(rand()%ballAngleRange+1+BALL_MIN_ANGLE)/90*randomVertical;
		ballDirectX=(2-fabs(ballDirectY));
	}
}

void makeBox(float x, float y, float width, float height)
{
	//x,y is top left corner!
	glBegin(GL_QUADS);
		glVertex3f(x, y-height, 0);
		glVertex3f(x, y, 0);
		glVertex3f(x+width, y, 0);
		glVertex3f(x+width, y-height, 0);
	glEnd();
}

void renderWalls()
{
	makeBox(-1000, TOP_BOUNDARY+BOUNDARY_HEIGHT, BOUNDARY_WIDTH, BOUNDARY_HEIGHT); //top
	makeBox(-1000, BOTTOM_BOUNDARY, BOUNDARY_WIDTH, BOUNDARY_HEIGHT); //bottom
}

void renderPaddles()
{
	//player 1
	makeBox(LEFT_BOUNDARY-PADDLE_WIDTH, player1Loc, PADDLE_WIDTH, PADDLE_HEIGHT);

	//player 2
	makeBox(RIGHT_BOUNDARY, player2Loc, PADDLE_WIDTH, PADDLE_HEIGHT);
}

void renderBall()
{
	makeBox(ballLocX, ballLocY, BALL_SIZE, BALL_SIZE);
}

void roundOver(int player)
{
	renderBall();
	canMovePaddle = false;
	
	if (player == 1)
	{
		player1Score++;
		lastWin = 1;
	}
	else
	{
		player2Score++;
		lastWin = 2;
	}

	//update title with score

}

void movePlayer(int player, float speed)
{
	switch(player)
	{
		case 1 :
			//if within boundaries move fully, otherwise move partially
			if (player1Loc+speed >= TOP_BOUNDARY)
				player1Loc = TOP_BOUNDARY;
			else if (player1Loc+speed <= BOTTOM_BOUNDARY+PADDLE_HEIGHT)
				player1Loc = BOTTOM_BOUNDARY+PADDLE_HEIGHT;
			else
				player1Loc+=speed;
			
			renderPaddles();
			break;
		case 2 :
			if (player2Loc+speed > TOP_BOUNDARY)
				player2Loc = TOP_BOUNDARY;
			else if (player2Loc+speed < BOTTOM_BOUNDARY+PADDLE_HEIGHT)
				player2Loc = BOTTOM_BOUNDARY+PADDLE_HEIGHT;
			else
				player2Loc+=speed;
			
			renderPaddles();
			break;
	}
}

void moveBall()
{
	//move ball slower based on ball speed
	if (ballMoveCheck<ballSpeed)
	{
		renderBall();
		ballMoveCheck++;
		return;
	}

	ballMoveCheck = 0;

	//Collision Detection
	if (ballLocY-BALL_SIZE+ballDirectY <= BOTTOM_BOUNDARY) //bottom
	{
		ballLocX+=ballDirectX;
		ballLocY=BOTTOM_BOUNDARY+BALL_SIZE;
		renderBall();
		ballDirectY*=-1;
	}
	else if (ballLocY+ballDirectY >= TOP_BOUNDARY) //top
	{
		ballLocX+=ballDirectX;
		ballLocY=TOP_BOUNDARY;
		renderBall();
		ballDirectY*=-1;
	}
	else if (ballLocX+ballDirectX <= LEFT_BOUNDARY) //left
	{
		if (ballLocY-BALL_SIZE <= player1Loc && ballLocY >= player1Loc-PADDLE_HEIGHT) //left paddle
		{
			ballLocX=LEFT_BOUNDARY;
			ballLocY+=ballDirectY;
			renderBall();
			
			//changes ball direction based on where it hits (upward,random,downward)
			if (ballLocY-BALL_SIZE >= player1Loc-PADDLE_HEIGHT/3)
			{
				modifier = 1;
				ballSpeed = BALL_SPEED * .7;
			}
			else if (ballLocY <= player1Loc-PADDLE_HEIGHT*2/3)
			{
				modifier = -1;
				ballSpeed = BALL_SPEED * .7;
			}
			else
			{
				modifier = -(ballDirectY/-fabs(ballDirectY));
				ballSpeed = BALL_SPEED;
			}

			ballDirectY=(float)(rand()%ballAngleRange+1+BALL_MIN_ANGLE)/90*modifier;
			ballDirectX=(2-fabs(ballDirectY));
		}
		else
		{
			roundOver(2);
		}
	}
	else if (ballLocX+BALL_SIZE+ballDirectX >= RIGHT_BOUNDARY) //right
	{
		if (ballLocY-BALL_SIZE <= player2Loc && ballLocY >= player2Loc-PADDLE_HEIGHT) //right paddle
		{
			ballLocX=RIGHT_BOUNDARY-BALL_SIZE;
			ballLocY+=ballDirectY;
			renderBall();
			
			//changes ball direction based on where it hits (upward,random,downward)
			if (ballLocY-BALL_SIZE >= player2Loc-PADDLE_HEIGHT/3)
			{
				modifier = 1;
				ballSpeed = BALL_SPEED * .7;
			}
			else if (ballLocY <= player2Loc-PADDLE_HEIGHT*2/3)
			{
				modifier = -1;
				ballSpeed = BALL_SPEED * .7;
			}
			else
			{
				modifier = -(ballDirectY/-fabs(ballDirectY));
				ballSpeed = BALL_SPEED;
			}

			ballDirectY=(float)(rand()%ballAngleRange+1+BALL_MIN_ANGLE)/90*modifier;
			ballDirectX=-(2-fabs(ballDirectY));
		}
		else
		{
			roundOver(1);
		}
	}
	else
	{
		ballLocX+=ballDirectX;
		ballLocY+=ballDirectY;
		renderBall();
	}
}

void keyResponse()
{
	if (keyState[27])
		exit(0);
	if (keyState['w'] && canMovePaddle)
		movePlayer(1,PADDLE_SPEED);
	if (keyState['s'] && canMovePaddle)
		movePlayer(1,-PADDLE_SPEED);

	if (keyStateSpecial[GLUT_KEY_UP] && canMovePaddle)
		movePlayer(2,PADDLE_SPEED);
	if (keyStateSpecial[GLUT_KEY_DOWN] && canMovePaddle)
		movePlayer(2,-PADDLE_SPEED);
}

void keyDown(unsigned char key, int x, int y)
{
	keyState[key] = true;
}

void keyUp(unsigned char key, int x, int y)
{
	keyState[key] = false;

	if (key == 'r' || key == '0')
	{
		if (!canMovePaddle)
			resetGame();
	}
}

void keyDownSpecial(int key, int x, int y)
{
	keyStateSpecial[key] = true;
}

void keyUpSpecial(int key, int x, int y)
{
	keyStateSpecial[key] = false;
}

void processSpecialKeys(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP :
			movePlayer(2,PADDLE_SPEED);
			break;
		case GLUT_KEY_DOWN :
			movePlayer(2,-PADDLE_SPEED);
			break;
	}
}

void resizeWindow(int w, int h)
{
	// Prevent a divide by zero
	if(h == 0)
		h = 1;

	float ratio = (float)(1.0* w / h);

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(90,ratio,1,1005); //fov, ratio, near clip, far clip
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camEyeX, camEyeY, camEyeZ, camLookX, camLookY, camLookZ, 0.0f, 1.0f, 0.0f);
}

void mainLoopInit()
{
	glLoadIdentity();
	gluLookAt(camEyeX, camEyeY, camEyeZ, camLookX, camLookY, camLookZ, 0.0f, 1.0f, 0.0f);
	
	initCheck++;
}

void mainLoop(void)
{
	keyResponse();

	if (initCheck < 1)
		mainLoopInit();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clears screen
	
	glColor3f(1.0,1.0,1.0);
	renderWalls();
	renderPaddles();
	moveBall();

	glutSwapBuffers(); //swap buffers so above is shown
}

void main(int argc, char **argv)
{
	//initialization
	resetGame();
	glutInit(&argc, argv); //required
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("Pong!");

	for(int i=0;i<NUM_KEY_STATES;i++)
	{
		keyState[i] = false;
	}
	for(int i=0;i<NUM_KEY_STATES_SPECIAL;i++)
	{
		keyStateSpecial[i] = false;
	}

	glEnable(GL_DEPTH_TEST);

	//function association
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutSpecialFunc(keyDownSpecial);
	glutSpecialUpFunc(keyUpSpecial);

	glutDisplayFunc(mainLoop); //defines rendering function
	glutIdleFunc(mainLoop); //what to do when idle
	glutReshapeFunc(resizeWindow); //defines window resize function

	//rendering
	glutMainLoop(); //begins rendering
}
