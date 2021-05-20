# **Pong: An original take on a timeless classic**

## Cristian Gradinaru 

For this project, I aimed to develop from the ground up a personal rendition of `Pong`, a game we all know and love, and continue onwards to bring improvements upon it, and put a personal spin on its components. I started from scratch, and in this document I will go step by step into its development. 

The first major decision I had to make was what language would I use for my first foiree into the game development space... The decision turned out to be simpler than first immagined, because `C++ `provided multiple advantages: Firstly, I was pretty famililar with it, and considering the mountain that I was about to climb, inserting a "new language" challenge into this equation would most likely hider the process more than help. Secondly, C++ is a language often used in the video game development zone, so I probably would be able to find answers to my most burning questions.

## **Setup**

I was determined to work with `SDL(Simple Directmedia Layer)` because I had seen some extremly nice tutorial which would have helped me better understand what was going on. I shall link the site in the description, as I believe it provides an absolutely amazing introduction (and even touches on some more advanced concepts) of `SDL`. 

The operating system I would be using was `Windows 10` ( a mistake, I know), and with that I paired the only sensible option ... `Visual Studio`. First step in working with a tool such as SDL is seting up the libraries. Following an arduous process of trying to fiddle with the libraries and environment paths of Microsoft's mysterious ways, I had finnaly completed the first steps of the process, and was ready to undertake the exciting task of starting to create.

## **The creative process**

### **The window**

The first stage in creating a video game using `SDL` is of course, creating the window. We need to render a black window, initialize the `SDL` video interface. After all of that is done, will have a black window with the dimentions defined in the top of the file.

### **The net**

The next stage to project is the fabled net (which ironically has no net functionality). In order to do this, we must proceed in two stages. Stage one is to select the white color in order to be able to draw, and afer we have dipped the immaginary brush into the immaginary white paint, we can use a for to draw a line on the y axis, using the middle of the x axis as a set coordinate, we draw every time the y coordinate is a multiple of 10.

### **Ball & Paddles**

The next important bit we have to tackle is of course, creating the `ball` and the `paddles`, our central components of the gameplay pillars. Firstly, we will have to create a class called `VCT` which will be used to represent the location and speed of our `ball` and `paddles`. It will work with coordinates ( x and y) and will be the main backstage pillar of our game. After this is done, we have to define the `ball`, which will use a class of its own. We will define the object ball, which will have two positional coordinates, `x `and `y`, and two thickness and length values, which we have named `Ball_Width` and `Ball_Height`. The next stage is to of course draw the actual square represented by the ball, and that can be done with a `SDL_RenderFillRect` call, which basically fills the area with solid white.

The paddles follow a very similar concept, with their form being just an elongated square and positioned on the extreme of the window borders, to serve as the "goalkeepers".

Some additional functions are present in this class, such as those that deal with collision. I will approach those in their own separate category. 

### **The score**

The `score` was a bit tricky, as i had to import a font first( and also look for a non copyrighted one, because u never know). So after that is done, we need to create a class called `Score`. Within this class we establish the font( the one we imported) the color and we create a texture out of it. Inside this class we also have a `SetScore` method, which allows us to increment the scoreline( we will see how this is used within the main function). We have to be careful each time we create a texture or surface, to destroy it later, otherwise we create a sort of `memory leak` scenario, which can lead to big problems down the line.

<p float= "left">

 <img src="Performance-abnormal.png" height="250">
 <img src="Performance-normal.png" height="250" width= 350>

</p>

### **Movement**

Within this section we have to define the positions in which the `paddles` are allowed to move, so we must restrect the position to `0` ( the top of the screen) and `Win_Height-Paddle_Height`( bottom of the screen), and also implement a position dependency on velocity and our time constant( which makes sure thigs always move at the same pace).

Perhaps the biggest problem in the movement department is taking and processing the keyboard inputs. After dealing with the exit conditions within an `SDL_PollEvent`, we have to deal with eveery possible event that will influence the movement of our paddles. I selected `W `and `S `for the player in the left, and Arrowkeys `Up` and `Down` for the player on the right. With the help of a declaration of buttons we can assign one "button press" to each of the keys that is registered by the keyboard.

After all that is done, and we are in the event polling `loop`, we must also define what each button press symbolizes, by defining the `velocity` influence that each of these changes inflict. So if the button is up, we must have the velocity on the y axis of P1( the one on the left) gain `-Paddle_speed`, because the screen origin is the top left corner, so in order to go up, we have to decrease on the y axis. the same will be true for p1 going down, and for P2. The explination stands exactly the same

The ball implementation is very very similar, with an update function, a constant which represents the ball speed, the only difference being that the player has no say on the ball movement though keyboard inputs, so we put an initial spin on the ball and we let collisions take it from there. Talking about collisions ...

``` C++
case SDL_KEYDOWN:

				//if that key is escape, stop running
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					running = false;
				}
				else if (event.key.keysym.sym == SDLK_w) {		//P1 uses W and S
					buttons[Buttons::P1Up] = true;
				}
				else if (event.key.keysym.sym == SDLK_s) {
					buttons[Buttons::P1Down] = true;
				}
				else if (event.key.keysym.sym == SDLK_UP) {		//P2 uses arrow keys
					buttons[Buttons::P2Up] = true;
				}
				else if (event.key.keysym.sym == SDLK_DOWN) {
					buttons[Buttons::P2Down] = true;
				}
				break;

```

### **Collisions**

As we did for each of the other sections beforehand, the first step is to define a `collision`: I went with a dual approach to this, defining the `wall` and `paddle` `collisions` separately, both being defined by a type of `collision` and a `penetration` (meaning once our object has been detected to have colided- by basically penetrating an object, we have to remove the ball from inside the object)

`Paddle collision` is relatively simple: we define the coordinates of the `ball's `extremities and we look to see when the `ball` has intersected the x and y axies of the `paddles`; when that happens, we have a `collision`. I also split the paddles into 3 pieces, a middle, left and right of the paddle, and depending on which part of the paddle this hits, the ball will take a straight continuation of the movement that was happening, or change velocities to look like a side-hit ( by putting a `0.75*Ball Speed` on the y axis as well).

`Wall collision` follows a similar concept, with some distinguishing features: If we hit the `Top` or `Bottom` walls, we want the `ball` to basically fully reflect off the surface, meaning the speed of the Y axis will get reversed so the `ball` seems to be moving normally as if hit against a wall. If the `ball` hits the Left or right walls however, we our answer must be twofold, we must increment the `scoreline`, and launch the ball back into play. For launching the ball back into play, i chose an approach similar to a tennis serve: from the bottom of the line, towards the enemy half, on a predictable enough distance and speed to be able to be hit consistently by the receiving player.

### **The main function**

 Here I will provide a small revision of what is going on: Firstly we initialize the components which will be used, the `SDL video` format and the Font , after which we are freee to create the window and the renderer and import the font. 

 We must continue with the initializations of our two players, the `ball` and the scoreline for each player. Once that is over we can start out game loop, which will continue to run until the exit conditions ( the X botton) is being met. 

 ```C++
//SDL window, the place where we shall do all the things.
	SDL_Window* window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Win_Width, Win_Height, SDL_WINDOW_SHOWN); //window centered
	//SDL renderer, the thing with which we shall do all the things
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	//Font
	TTF_Font* scoreFont = TTF_OpenFont("CarnevaleeFreakshow.ttf", 40);

	//drawing the ball at the exact centre. anything else will make it slightly off centre
	Ball ball(VCT((Win_Width / 2.0f) - (Ball_Width / 2.0f), (Win_Height / 2.0f) - (Ball_Height / 2.0f)), VCT(Ball_Speed, 0.0f));

	//Declaring the paddles
	Paddle P1(VCT(50.0f, (Win_Height / 2.0f) - (Paddle_Height / 2.0f)), VCT(0.0f, 0.0f));
	Paddle P2(VCT(Win_Width - 50.0f, (Win_Height / 2.0f) - (Paddle_Height / 2.0f)), VCT(0.0f, 0.0f));

	Score P1ScoreText(VCT(Win_Width / 4, 20), renderer, scoreFont);
	Score P2ScoreText(VCT(3 * Win_Width / 4, 20), renderer, scoreFont);

 ```

 Following that, we enter the keyboard polling zone, where we "listen" for keyboard inputs and take the appropriate response for each of the keys being pressed. Here we also initialize the counter which will track the time between each frames to be able to provide a smooth time passing, relative to what is going on. 

 After all the keyboard inputs have been dealt with, its time to deal with collisions, we must call the appropriate functions for each of the possible collisions of the `ball`: with the `paddles` or with the walls, and increment the scoreline if the situation for hitting left or right walls have been met.

 Last but not least we have the graphics, The `paddles`, the `ball` and the backgound are all dealt with in the following section. All three (the background and the two `paddles`) work with a texturised image that is being imported from the file in which the app resides. After all these operations are over, we can safely stop the timer (to count the elapsed time between when we started and now) and destroy all remaining textures, windows and renderers that will no longer be used after the termination of the program.
  <img src="PongGame.png" width="1080">

 As far as the speed acceleration I mentioned at the start, it is dealt with in the `CollideWithPaddle` section, by multiplying the x velocity of the `ball` with a counter, which increases by a little bit each time a `paddle` hits the `ball`, and which resets after a player has scored. This has been counted to 29 hits before the game engine no longer is able to deal with the utter speed of everything that is happening, but luckly during testing, no games were able to realistically reach such a hit number.

  <img src="counter.png" width="200">


The source code itself has a bit more comments about what each of the variables mean, because i couldn't realistically cover absolutely everything here without making this document the size of a small child.


  
