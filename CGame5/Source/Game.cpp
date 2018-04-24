// We made a few changes since the last lesson
// Our test playfield is now living in the Game.h file, this will make it easier for us to pass that grid to other classes
// we changed the Scale idea to have X and Y scale, so the shader has been changed a little and the initial set up of SimpleObj class
// The play field is now quite large, but you will notice our game is rather slow, this is going to be addressed next lesson, since the speed is just not
// good enough and we have to refine some of the ways we are doing things.


#include "Game.h"
#include "SimpleObj.h"


GLint SimpleObj::ProgramObject = 0;
GLint SimpleObj::positionLoc =0;
GLint SimpleObj::texCoordLoc = 0;

// if we want to position it somewhere we need to ask for the handle for the position uniform and other unfirms
GLint SimpleObj::positionUniformLoc = 0;
GLint SimpleObj::samplerLoc = 0;
GLint SimpleObj::ScreenCoordLoc = 0;
GLint SimpleObj::ScaleLoc = 0;

// because we defined this as static variables in our header we have to make sure it exists

std::vector<SimpleObj*> Game::MyObjects;
std::vector<SimpleObj*> Game::MyBullets;
Game::Game()
{
	OGLES.Init(); // make sure we initialise the OGL instance we called OGLES
};

Game::~Game() {}; // We should close down some things here but can do it later.


void Game::Update()
{

// strictly speaking this is an init and update, but this update never ends, we will clean up later

// this time instead of
char* TileFiles[] =  // here's a good example of an array, being used to store text, which is actually stored somewhere else in memory so we point * at it!
	{
		(char*)"images/tile0.png",
		(char*)"images/tile1.png",
		(char*)"images/tile2.png",
		(char*)"images/tile3.png",
		(char*)"images/tile4.png",
		(char*)"images/tile5.png",
		(char*)"images/tile6.png",
		(char*)"images/tile7.png",
		(char*)"images/tile8.png",
		(char*)"images/tile9.png",
		(char*)"images/tile10.png",
		(char*)"images/tile11.png",
		(char*)"images/tile12.png",
		(char*)"images/tile13.png",
		(char*)"images/tile14.png",
		(char*)"images/tile15.png",
	};

// here's another useful array
	GLint TileIDs[16]; // define an array to hold the id's we get


	// lets turn these all these graphic files into textures
	for(int i = 0 ; i < 16; i++)
	{
		int Grwidth, Grheight, comp;
		unsigned char* data = stbi_load(TileFiles[i], &Grwidth, &Grheight, &comp, 4);    // ask it to load 4 componants since its rgba
		//now its loaded we have to create a texture, which will have a "handle" ID that can be stored, we have added a nice function to make this easy
		TileIDs[i] = this->OGLES.CreateTexture2D(Grwidth, Grwidth, (char*) data);     //just pass the width and height of the graphic, and where it is located and we can make a texture
		free(data); // when we load an image, it goes into memory, making it a texture copies it to GPU memory, but we need to remove the CPU memory before to load another or it causes a leak
	}

	// now make a 2Dgrid, this is just a small test one.


	MyBat = new Paddle(); // its defined in the h file

// repeat the loading code we used for tiles
	int Grwidth, Grheight, comp;
	unsigned char* data = stbi_load((char*) "images/paddle.png", &Grwidth, &Grheight, &comp, 4);    // ask it to load 4 componants since its rgba
	//now its loaded we have to create a texture, which will have a "handle" ID that can be stored, we have added a nice function to make this easy
	MyBat->TextureID = this->OGLES.CreateTexture2D(Grwidth, Grwidth, (char*) data);     //just pass the width and height of the graphic, and where it is located and we can make a texture
	free(data); // when we load an image, it goes into memory, making it a texture copies it to GPU memory, but we need to remove the CPU memory before to load another or it causes a leak
	MyBat->ScaleX = 32;
	MyBat->ScaleY = 8;

	// notice how we created a new instance of the bat, new returns a pointer so we use Paddle *, the * symbol indicates this is a pointer to memory, that contains the instance of a Paddle called MyBat
	// another change is how we load values into that instance we don't use MyBat.TextureID we now use MyBat->TextureID
// lets do the same with a ball
	 MyBall = new Ball(); // its defined in the h file to make it easier for the Paddle to access it for a texture
	 data = stbi_load((char*) "images/ball.png", &Grwidth, &Grheight, &comp, 4);     // ask it to load 4 componants since its rgba

	MyBall->TextureID = this->OGLES.CreateTexture2D(Grwidth, Grwidth, (char*) data);      //just pass the width and height of the graphic, and where it is located and we can make a texture
	free(data);  // when we load an image, it goes into memory, making it a texture copies it to GPU memsssssssssssssssssssssssaaaory, but we need to remove the CPU memory before to load another or it causes a leak
	MyBall->ScaleX = 16;
	MyBall->ScaleY = 16;

	std::vector<SimpleObj> MyTiles; // we can still create instances of objects in a vector

// lets make our playfiled the same way as last lesson but this time use a vector

	int		index = 0; // we need to keep an index counter
	for(int Row = 0 ; Row < 40 ; Row++) // this is an outer loop we will use the Row variable to go down the grid
	{
		for (int Column = 0; Column < 64; Column++) // this is an inner loop and will go along the grid
		{
			GLint WhatTile = PlayField[Row][Column]; // what tile is it?
			if(WhatTile != 0) // llets not make 0 tiles
			{
				SimpleObj Tile;
				MyTiles.push_back(Tile); // place it in the vector
	// use the vector just like an array if you want, or continue to use the name Tile and add the features
				MyTiles[index].m_MyName = "I am a generic tile";  // we can still
				MyTiles[index].TextureID = TileIDs[WhatTile];
				MyTiles[index].Xpos = Column * 16 +8  ;  // the tiles are 16 pixels apart in X and Y lets move them +8 because the referenc is in the centre
				MyTiles[index].Ypos = 768 - ((Row * 16) + 8 );  // <<woahhh.. whats this? See below
				MyTiles[index].ProgramObject = OGLES.programObject;
				// now its all set up, just increment the index
				index++;  // we will now set up the next MyTile
			}
		}
	}



	// Get the attribute locations, we'll store these in values we can call handles.
	SimpleObj::positionLoc = glGetAttribLocation(SimpleObj::ProgramObject, "a_position");
	SimpleObj::texCoordLoc = glGetAttribLocation(SimpleObj::ProgramObject, "a_texCoord");

	// if we want to position it somewhere we need to ask for the handle for the position uniform and other unfirms
	SimpleObj::positionUniformLoc = glGetUniformLocation(SimpleObj::ProgramObject, "u_position");    	 // << you can see we ask for the handle of the variable in our shader called u_position
	SimpleObj::samplerLoc = glGetUniformLocation(SimpleObj::ProgramObject, "s_texture");
	SimpleObj::ScreenCoordLoc = glGetUniformLocation(SimpleObj::ProgramObject, "u_Screensize");
	SimpleObj::ScaleLoc = glGetUniformLocation(SimpleObj::ProgramObject, "u_Scale");

/*what was this 768-((Row*16)+8+200)
 *
 *Its just becuase our Screen system is based on having 0,0 at the bottom left of the screen
 *but the grid is read from the top down, so if we didn't flip it like this we'd draw the grid upside down
 *We'll have to account for that whenever we use a grid.
 *the +8 is because our reference point for the tile is its centre, and we want its edge to move over half the tile width
 *the 200 is just to take it away from the edge. When we use a full size map we won't need that offset
 *
 *
 **/

/**************************************************************************/
// Here's where the min loop occurs
// now we will do a loop to run 10000 cyles to see the result this
	for (int i = 0; i < 10000; i++)
	{

		glClear(GL_COLOR_BUFFER_BIT); // clear the screen
// and now lets print our 100 tiles which should show our grid,

		for(int Tiles = 0 ; Tiles < MyTiles.size() ; Tiles++) // We may not know how many Tiles we made but the Size of the vector can be asked for
		{
			MyTiles[Tiles].Update(); // not only can we access the data of a class we can call its methods

		}


// we have 2 special objects referenced by name so lets work with those 1st, they could be put in the MyObjects vector but as they are important we just do them by name
		MyBall->Update(this); // "this" is a useful thing, its a pointer to "this" particular instance of game, so this update is looking for Game* AnInstanceOfGame.
		MyBall->Draw();

		MyBat->Update(this);
		MyBat->Draw();

// we can go through a vector the same way we can an array.
		for (int i = 0; i < MyObjects.size(); i++)
		{
		bool IsItAlive =	MyObjects[i]->Update(this);
							MyObjects[i]->Draw();
			if (IsItAlive == false)
			{
				delete MyObjects[i]; // it was new'd into existance so we must delete it
				MyObjects.erase(MyObjects.begin() + i); // then remove it from the vector
			}

		}
// repeat for bullets which are in a seperate vector to allow easier testing
// the balls will test if they hit any of them, and delete them for us, but we can also
// test the IsItAlive value they return (don't need to though)
		for(int i = 0 ; i < MyBullets.size() ; i++)
		{
			bool IsItAlive =	MyBullets[i]->Update(this);
			MyBullets[i]->Draw();
			if (IsItAlive == false)
			{
				delete MyBullets[i];   // it was new'd into existance so we must delete it
				MyBullets.erase(MyBullets.begin() + i);    // then remove it from the vector
			}

		}
// put our image on screen
		eglSwapBuffers(OGLES.state.display, OGLES.state.surface);

 // we might not want to run through 10,000 cycles, so lets test if the esc key was pressed ( you might need to hold it)
// this is a standard i/o key read..its usable, but we'll use a better way soon
// its actually set up in the paddle creation code, so we need to use it afer the paddle has been created
		int fd = fileno(stdin); // we have to do a bit of setting up
		char c = '\0'; // provide a default
		int err = read(fd, &c, 1); // actually read the key char
		if (err == 0) c = '\0'; // if there was no key reaffirm a null key

		// returns err == 0 for no key, err == 1 for pressed, and char in c
		if(err != 0)  // lets see what was pressed
		{

			if (c == 27)  // the esc key is character 27
			{// clean the memory out before we escape, we newed a lot of things, lets delete them
				delete MyBat;
				delete MyBall;
				for (int i = 0; i < MyObjects.size(); i++)
					delete MyObjects[i]; // delete the objects
				MyObjects.erase(MyObjects.begin(), MyObjects.end()); // clear the vector

				for (int i = 0; i < MyBullets.size(); i++)
					delete MyBullets[i]; // delete the bullets
				MyBullets.erase(MyBullets.begin(), MyBullets.end());

				break; // escape from the loop (or return)
			}

		}


		// This isn't a very good bit of coding, its rather slow to update, and we are repeating a lot of graphics, but we will
		// make this more effective next time.

		}
	return; // we don't really need the return but it helps us to see the end of the method was reached and we can breakpoint it
};

