#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#else
	#define RED "\x1b[31m"
	#define GREEN "\x1b[32m"
	#define YELLOW "\x1b[33m"
	#define BLUE "\x1b[34m"
	#define MAGENTA "\x1b[35m"
	#define WHITE "\x1b[37m"
	#define RESET "\x1b[0m"
#endif


typedef struct Face Face; //forward declaration of Face to allow for face to contain pointers of itself

typedef struct ViewCube ViewCube; //forward declaration of ViewCube, mostly for consistancy with Face

typedef enum {NORTH, WEST, EAST, SOUTH} Direction;  //allows someone reading the code to understand the rotateCube command with regards to the menu

struct Face
{
	int blocks[3][3];
	Face* north;
	Face* south;
	Face* east;
	Face* west;
};

struct ViewCube
{
  Face* front;
	Direction currentNorthDirection;
};

void printCube(Face *frontFace, Face *originalBack); //prints each side of the cube, with "frontFace" centered in a plus, and the side opposite of frontFace off to the side

void rotateFace(Face *face, int clockwise); //simulates rotating the selected face by 90 degrees either clockwise or counter-clockwise, also shifts around the appropriate rows of the adjacent sides

void rotateMatrix(Face* face, int clockwise); //rotates the matrix in a face 90 degrees right

void rotateCube(ViewCube *cube,  Direction direction, Face *originalBack); //changes the face that the user is able to manipulate and is currently considered front for printing

int checkSolved(Face *currentFront); // checks each face to see if they are all solid "colors"

void shuffleCube(ViewCube * cube, Face *originalBack); //calls rotateFace on random faces a specified number of times

void printRow(Face * face, int row); //prints a row seperated by spaces, helper function for printCube

void printRowBack(Face * face, int row); //prints a row in the opposite direction seperated by spaces, helper function for printCube with the back side

void printMenu(); //prints out the text required for the user to understand how to manipulate the cube

void arraySet(void* in, void* out); //sets the contents of array out to array in provided that they are the same size

void fillBlocks(Face* face, int fill); //tiny shortcut function to easily fill the array blocks in a face.

Face * initCube(Face* front, Face* back, Face* north, Face* south, Face* east, Face* west); //where each side is shown how to relate to the others, mostly to make main() more readable, returns the first front face

void save(Face* currentFront);

void load(Face* currentFront);

void printColor(int data); //is setup to not print in color on windows systems as I have no clue if that would compile

int main()
{
	srand(time(0));
  Face front, back, north, south, east, west;
	ViewCube cube;
  cube.front = initCube(&front, &back, &north, &south, &east, &west);
  int selection;
	int failure=1;
	char buffer[8];
	do {
		printf("How would you like to start?\n");
		printf("\t1:Shuffle a new cube\n");
		printf("\t2:Load your last cube (shuffles a new cube if there is no unsolved save file)\n");
		fgets(buffer, sizeof(buffer), stdin);
		sscanf(buffer, "%d", &selection);
		switch(selection)
		{
			case 1:
				failure=0;
				shuffleCube(&cube, &back);
				break;
			case 2:
				failure=0;
				load(cube.front);
				if (checkSolved(cube.front)==1)
				{
					shuffleCube(&cube, &back);
				}
				break;
		}
	} while(failure);
	if (checkSolved(cube.front)==1)
	{
		shuffleCube(&cube, &back);
	}
  do
  {
		selection=0;
		save(cube.front);
		printCube(cube.front, &back);
		printMenu();
		fgets(buffer, sizeof(buffer), stdin);
		sscanf(buffer, "%d", &selection);
		printf("selection=%d\n", selection);
    switch(selection)
    {
      case 1 ... 4:
				rotateCube(&cube, selection-1, &back);
        break;
			case 5 ... 6:
				rotateFace(cube.front, selection-5);
				break;
			case 7:
				exit(0);
				break;
      default:
        printf("That is not a valid action.\n");
        break;
    }
		save(cube.front);
  } while(!checkSolved(cube.front));
	save(cube.front);
	printf("Congratulations!");
	printCube(cube.front, &back);
	printf("You Have Solved The Rubik's Cube!\n");
	return(0);
}

Face * initCube(Face* front, Face* back, Face* north, Face* south, Face* east, Face* west)
{
  front->north=north;
  front->south=south;
  front->east=east;
  front->west=west;
	back->north=south;
  back->south=north;
  back->east=west;
  back->west=east;
  north->north=back;
  north->south=front;
  north->east=east;
  north->west=west;
  south->north=front;
  south->south=back;
  south->east=east;
  south->west=west;
  east->north=north;
  east->south=south;
  east->east=back;
  east->west=front;
  west->north=north;
  west->south=south;
  west->east=front;
  west->west=back;
	fillBlocks(front, 1);
	fillBlocks(back, 2);
	fillBlocks(north, 3);
	fillBlocks(south, 4);
	fillBlocks(east, 5);
	fillBlocks(west, 6);
	return front;
}

void rotateFace(Face *face, int clockwise)
{
	rotateMatrix(face, clockwise);
	int holdingArray[3]={0};
	int temp;
	if (clockwise==1)
	{
		for (int i=0; i<3; i++)
		{
			holdingArray[i]=face->north->blocks[2][i];
		}
		for (int i=0; i<3; i++)
		{
			temp=face->east->blocks[i][0];
			face->east->blocks[i][0]=holdingArray[i];
			holdingArray[i]=temp;
		}
		for (int i=0; i<3; i++)
		{
			temp=face->south->blocks[0][2-i];
			face->south->blocks[0][2-i]=holdingArray[i];
			holdingArray[i]=temp;
		}
		for (int i=0; i<3; i++)
		{
			temp=face->west->blocks[2-i][2];
			face->west->blocks[2-i][2]=holdingArray[i];
			holdingArray[i]=temp;
		}
		for (int i=0; i<3; i++)
		{
			face->north->blocks[2][i]=holdingArray[i];
		}
	}
	else
	{
		for (int i=0; i<3; i++)
		{
			holdingArray[i]=face->north->blocks[2][i];
		}
		for (int i=0; i<3; i++)
		{
			temp=face->west->blocks[2-i][2];
			face->west->blocks[2-i][2]=holdingArray[i];
			holdingArray[i]=temp;
		}
		for (int i=0; i<3; i++)
		{
			temp=face->south->blocks[0][2-i];
			face->south->blocks[0][2-i]=holdingArray[i];
			holdingArray[i]=temp;
		}
		for (int i=0; i<3; i++)
		{
			temp=face->east->blocks[i][0];
			face->east->blocks[i][0]=holdingArray[i];
			holdingArray[i]=temp;
		}
		for (int i=0; i<3; i++)
		{
			face->north->blocks[2][i]=holdingArray[i];
		}
	}
}

void rotateMatrix(Face* face, int clockwise)
{
  int holdingMatrix[3][3];
  arraySet(&face->blocks, &holdingMatrix);
  for (int i=0; i<3; i++)
  {
    for (int j=0; j<3; j++)
    {
			if (clockwise==1)
			{
				face->blocks[i][j]=holdingMatrix[2-j][i];
			}
			else
			{
				face->blocks[i][j]=holdingMatrix[j][2-i];
			}
    }
  }
}

int checkSolved(Face *currentFront)
{
	int ret=1;
	Face * faces[6]={currentFront, currentFront->north->north, currentFront->north, currentFront->south, currentFront->east, currentFront->west};
	for (int i=0; i<6; i++)
	{
		for (int j=0; j<3; j++)
		{
			for (int k=0; k<3; k++)
			{
				if (faces[i]->blocks[j][k]!=faces[i]->blocks[0][0])
				{
					ret=0;
				}
			}
		}
	}
	return ret;
}

void printCube(Face *frontFace, Face *originalBack)
{
	if (frontFace==originalBack)
	{
		printf("\n      ");
		printRow(frontFace->south, 2);
		printf("\n      ");
		printRow(frontFace->south, 1);
		printf("\n      ");
		printRow(frontFace->south, 0);
		printf("\n");
		printRowBack(frontFace->east, 0);
		printRow(frontFace, 0);
		printRowBack(frontFace->west, 0);
		printf("\n");
		printRowBack(frontFace->east, 1);
		printRow(frontFace, 1);
		printRowBack(frontFace->west, 1);
		printf("\n");
		printRowBack(frontFace->east, 2);
		printRow(frontFace, 2);
		printRowBack(frontFace->west, 2);
		printf("\n");
		printf("      ");
		printRow(frontFace->north, 2);
		printf("\n      ");
		printRow(frontFace->north, 1);
		printf("\n      ");
		printRow(frontFace->north, 0);
		printf("\n");
	}
	else
	{
		printf("\n      ");
		printRow(frontFace->north, 0);
		printf("\n      ");
		printRow(frontFace->north, 1);
		printf("\n      ");
		printRow(frontFace->north, 2);
		printf("\n");
		printRow(frontFace->west, 0);
		printRow(frontFace, 0);
		printRow(frontFace->east, 0);
		printf("\n");
		printRow(frontFace->west, 1);
		printRow(frontFace, 1);
		printRow(frontFace->east, 1);
		printf("\n");
		printRow(frontFace->west, 2);
		printRow(frontFace, 2);
		printRow(frontFace->east, 2);
		printf("\n");
		printf("      ");
		printRow(frontFace->south, 0);
		printf("\n      ");
		printRow(frontFace->south, 1);
		printf("\n      ");
		printRow(frontFace->south, 2);
		printf("\n");
	}
}

void rotateCube(ViewCube *cube,  Direction direction, Face *originalBack)
{
	if (cube->front==originalBack)
	{
		switch(direction)
		{
			case SOUTH:
				cube->front=cube->front->north;
				rotateMatrix(cube->front->east, 1);
				rotateMatrix(cube->front->west, 0);
				break;
			case NORTH:
				cube->front=cube->front->south;
				rotateMatrix(cube->front->east, 0);
				rotateMatrix(cube->front->west, 1);
				break;
			case WEST:
				cube->front=cube->front->east;
				rotateMatrix(cube->front->north, 0);
				rotateMatrix(cube->front->south, 1);
				break;
			case EAST:
				cube->front=cube->front->west;
				rotateMatrix(cube->front->north, 1);
				rotateMatrix(cube->front->south, 0);
				break;
		}
	}
	else
	{
		switch(direction)
		{
			case SOUTH:
				cube->front=cube->front->north;
				rotateMatrix(cube->front->east, 0);
				rotateMatrix(cube->front->west, 1);
				break;
			case NORTH:
				cube->front=cube->front->south;
				rotateMatrix(cube->front->east, 1);
				rotateMatrix(cube->front->west, 0);
				break;
			case WEST:
				cube->front=cube->front->east;
				rotateMatrix(cube->front->north, 1);
				rotateMatrix(cube->front->south, 0);
				break;
			case EAST:
				cube->front=cube->front->west;
				rotateMatrix(cube->front->north, 0);
				rotateMatrix(cube->front->south, 1);
				break;
		}
	}
}

void printRow(Face * face, int row)
{
	for(int i=0; i<3; i++)
	{
		printColor(face->blocks[row][i]);
	}
}
void printRowBack(Face * face, int row)
{
	for(int i=0; i<3; i++)
	{
		printColor(face->blocks[row][2-i]);
	}
}
void printMenu()
{
	printf("\n\t\t1:Rotate Cube Northward\n");
	printf("2:Rotate Cube Westward\t\t3:Rotate Cube Eastward\n");
	printf("\t\t4:Rotate Cube Southward\n\n\n");
	printf("5:Rotate Front Face Counter-Clockwise\t6:Rotate Front Face Clockwise\n");
	printf("\n7:Quit\n\n\n");
}

void save(Face* currentFront)
{
	Face * faces[6]={currentFront, currentFront->north->north, currentFront->north, currentFront->south, currentFront->east, currentFront->west};
	FILE *ifp = NULL;
	#ifdef _WIN32
  	char filename[20]="RubikSaveFile";
	#else
		char filename[40]="/Users/darien/Desktop/RubikSaveFile";
	#endif
		ifp = fopen(filename, "w");
		int colors[3][3];
		for (int i=0; i<6; i++)
		{
			arraySet(faces[i]->blocks, &colors);
			for (int j=0; j<3; j++)
			{
				for (int k=0; k<3; k++)
				{
					fprintf(ifp, "%d ", colors[j][k]);
				}
				fprintf(ifp, "\n");
			}
			fprintf(ifp, "\n\n\n");
		}
		fclose(ifp);
}

void load(Face* currentFront)
{
	Face * faces[6]={currentFront, currentFront->north->north, currentFront->north, currentFront->south, currentFront->east, currentFront->west};
	FILE *ifp = NULL;
	#ifdef _WIN32
  	char filename[20]="RubikSaveFile";
	#else
		char filename[40]="/Users/darien/Desktop/RubikSaveFile";
	#endif
		if ((ifp = fopen(filename, "r")))
		{
			int colors[3][3];
			for (int i=0; i<6; i++)
			{
				for (int j=0; j<3; j++)
				{
					for (int k=0; k<3; k++)
					{
						fscanf(ifp, "%d", &colors[j][k]);
					}
				}
				arraySet(&colors, faces[i]->blocks);
			}
			fclose(ifp);
		}
}

void fillBlocks(Face* face, int fill)
{
	int temp[3][3] = {0};
	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++)
		{
			temp[i][j]=fill;
		}
	}
	arraySet(&temp, &face->blocks);
}

void shuffleCube(ViewCube * cube, Face *originalBack)
{
	int shuffles=0;
	char buffer[8];
	do
	{
		printf("Enter the number of moves you would like the cube to be shuffled:");
		fgets(buffer, sizeof(buffer), stdin);
		sscanf(buffer, "%d", &shuffles);
	} while (shuffles<=0);
	for (int i=0; i<shuffles; i++)
	{
		rotateCube(cube, rand()%4, originalBack);
		rotateFace(cube->front, rand()%2);
	}
}
#ifdef _WIN32
void printColor(int data)
{
	printf("%d ", data);
}
#else
void printColor(int data)
{
	switch(data)
	{
		case 1:
			printf(WHITE "1 " RESET);
			break;
		case 2:
			printf(RED "2 " RESET);
			break;
		case 3:
			printf(GREEN "3 " RESET);
			break;
		case 4:
			printf(YELLOW "4 " RESET);
			break;
		case 5:
			printf(BLUE "5 " RESET);
			break;
		case 6:
			printf(MAGENTA "6 " RESET);
			break;
	}
}
#endif


void arraySet(void* in, void* out)
{
  memcpy(out, in, 9*sizeof (int));
}
