//Using SDL and standard IO
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <SDL_ttf.h>
#include <chrono>
#include <string>

//the height and width of the window, const cuz they'll never change
const int Win_Height = 720;
const int Win_Width = 1280;


const int Ball_Width = 20;
const int Ball_Height = 20;

const int Paddle_Height = 75;
const int Paddle_Width = 12;
const float Paddle_Speed = 0.5f;
const float Ball_Speed = 0.5f;


//this declares the key presses and assigns
//automatically a constant to them, so default is 0, up is 1, down is 2 etc
enum Buttons {
	P1Up,
	P1Down,
	P2Up,
	P2Down
};

enum class CollisionType {
	None,
	Top,
	Middle,
	Bottom,
	Left,
	Right
};

struct Contact {

	CollisionType type;
	float penetration;

};

class VCT {

public:

	float x, y;
	//constructor without parameters
	VCT() : x(0.0f), y(0.0f) {}

	//parameter constuctor
	VCT(float x, float y) : x(x), y(y) {}

	//defining + for this class( aka overload i guess)
	VCT operator+(VCT const& rhs) {
		return VCT(x + rhs.x, y + rhs.y);
	}

	VCT& operator+=(VCT const& rhs) {
		x += rhs.x;
		y += rhs.y;

		return *this;
	}

	VCT operator*(float rhs) {
		return VCT(x * rhs, y * rhs);
	}


};


class Ball {
public:
	VCT position;
	VCT velocity;
	SDL_Rect rect{};

	//declaring ze ball
	Ball(VCT position, VCT velocity) : position(position), velocity(velocity) {
		//casting position
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		//dimensions
		rect.w = Ball_Width;
		rect.h = Ball_Height;
	}

	void Update(float dt) {

		position += velocity * dt;

	}

	void CollideWithPaddle(Contact const& contact) {
		//makes sure the ball never penetrates the actual paddle
		position.x += contact.penetration;
		//reverse the velocity
		velocity.x = -velocity.x;

		if (contact.type == CollisionType::Top) {
			velocity.y = -0.75f * Ball_Speed;
		}
		if (contact.type == CollisionType::Bottom) {
			velocity.y = 0.75 * Ball_Speed;
		}

	}

	void CollideWithWall(Contact const& contact) {

		if ((contact.type == CollisionType::Top) || (contact.type == CollisionType::Bottom)) {
			//reset the penetration, so the ball doesnt go INTO the WALL
			position.y += contact.penetration;
			velocity.y = -velocity.y;
		}
		else if (contact.type == CollisionType::Left) {
			position.x = Win_Width / 10.0f;
			position.y = Win_Height / 2.0f;
			velocity.x = Ball_Speed;
			velocity.y = 0.75 * Ball_Speed;
		}
		else if (contact.type == CollisionType::Right) {
			position.x = Win_Width / 1.1 ;
			position.y = Win_Height / 2.0f;
			velocity.x = -Ball_Speed;
			velocity.y = 0.75 * Ball_Speed;
		}

	}

	//draw the rect, with the renderer at the specified positions
	void Draw(SDL_Renderer* renderer) {
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);

		SDL_RenderFillRect(renderer, &rect);

	}
};



class Paddle {
public:
	VCT position;
	VCT velocity;
	SDL_Rect rect{};

	//declaring ze Paddle( update,we take into account the paddle velocity now
	Paddle(VCT position, VCT velocity) : position(position), velocity(velocity) {
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		rect.w = Paddle_Width;
		rect.h = Paddle_Height;
	}

	//we need to look into updating the position of the paddle, to update based on movement speed
	void Update(float dt) {
		position += velocity * dt;
		//doesnt let us go further than the top of the screen
		if (position.y < 0) {
			position.y = 0;
		}
		//doesnt let us go furtehr down than the botom of the screen
		else if (position.y > Win_Height - Paddle_Height) {
			position.y = Win_Height - Paddle_Height;
		}
	}

	//draw the rect, with the renderer at the specified positions
	void Draw(SDL_Renderer* renderer) {
		rect.y = static_cast<int>(position.y);
		SDL_Surface* image = IMG_Load("Bottom.png");
		SDL_Texture* loadedSurface = SDL_CreateTextureFromSurface(renderer, image);
		SDL_RenderCopy(renderer, loadedSurface, nullptr, &rect);
		//	SDL_RenderFillRect(renderer, &rect);
		SDL_DestroyTexture(loadedSurface);
		SDL_FreeSurface(image);
	}
};


class Score {
public:
	SDL_Surface* surface{};
	SDL_Texture* texture{};
	TTF_Font* font;
	SDL_Renderer* renderer;
	SDL_Rect rect{};


	//constructor
	Score(VCT position, SDL_Renderer* renderer, TTF_Font* font) : renderer(renderer), font(font) {

		//surface and texture for the score, we want them in font, white
		surface = TTF_RenderText_Solid(font, "0", { 0xFF, 0xFF, 0xFF, 0xFF });
		texture = SDL_CreateTextureFromSurface(renderer, surface);

		int width, height;
		SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		rect.w = width;
		rect.h = height;
	}
	//function for allowing us to increment the scoreline
	void SetScore(int score) {

		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);

		surface = TTF_RenderText_Solid(font, std::to_string(score).c_str(), { 0xFF, 0xFF, 0xFF, 0xFF });
		texture = SDL_CreateTextureFromSurface(renderer, surface);

		int width, height;
		SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
		rect.w = width;
		rect.h = height;
	}

	//frees the memory
	~Score() {
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}

	void Draw() {
		SDL_RenderCopy(renderer, texture, nullptr, &rect);
	}



};


Contact PaddleCollision(Ball const& ball, Paddle const& paddle) {
	//the ball starts as positioned on the top left corner
	//so in order to see how much it spreads to the left, we take its position
	//and in order to see how much it spreads to the right, we take pos+width
	float ballLeft = ball.position.x;
	float ballRight = ball.position.x + Ball_Width;
	//same on the Y axis, it spreads from the top left, downwards.
	float ballTop = ball.position.y;
	float ballBottom = ball.position.y + Ball_Height;


	float paddleLeft = paddle.position.x;
	float paddleRight = paddle.position.x + Paddle_Width;
	float paddleTop = paddle.position.y;
	float paddleBottom = paddle.position.y + Paddle_Height;

	Contact contact{};


	//if on any of the 4 coordinates of the ball-paddle combo there is no intersection
	if (ballRight <= paddleLeft) {
		return contact; //means there was no colision YET
	}
	if (ballLeft >= paddleRight) {
		return contact;
	}
	if (ballTop >= paddleBottom) {
		return contact;
	}
	if (ballBottom <= paddleTop) {
		return contact;
	}


	//hit in the upper 3rd
	float UpperCollision = paddleBottom - (2.0f * Paddle_Height / 3.0f);
	//hit in the bottom 3rd
	float MiddleCollision = paddleBottom - (Paddle_Height / 3.0f);

	//left paddle penetration
	if (ball.velocity.x < 0) {
		contact.penetration = paddleRight - ballLeft;
	}
	else if (ball.velocity.x > 0) {
		contact.penetration = paddleLeft - ballRight;  //right paddle penetration
	}

	if ((ballBottom > paddleTop) && (ballBottom < UpperCollision)) {
		contact.type = CollisionType::Top;
	}
	else if ((ballBottom > UpperCollision) && (ballBottom < MiddleCollision)) {
		contact.type = CollisionType::Middle;
	}
	else {
		contact.type = CollisionType::Bottom;
	}

	return contact;

}

//colliding with the wall should be treated differently
Contact WallCollision(Ball const& ball) {

	float ballLeft = ball.position.x;
	float ballRight = ball.position.x + Ball_Width;
	float ballTop = ball.position.y;
	float ballBottom = ball.position.y + Ball_Height;

	Contact contact{};

	//checking the types of collisions
	//top and bottom make the ball recoil, left and right should increment scoreline
	if (ballLeft < 0.0f) {
		contact.type = CollisionType::Left;
	}
	else if (ballRight > Win_Width) {
		contact.type = CollisionType::Right;
	}
	else if (ballTop < 0.0f) {
		contact.type = CollisionType::Top;
		contact.penetration = -ballTop;
	}
	else if (ballBottom > Win_Height) {
		contact.type = CollisionType::Bottom;
		contact.penetration = Win_Height - ballBottom;
	}
	return contact;
}





int main(int argc, char* args[]) {

	//initializes the SDL video component that we will use. 
	//could have put an error checker code here, but that would just add complexity

	SDL_Init(SDL_INIT_VIDEO);

	TTF_Init();


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



	float dt = 0.0f;
	//code is running at this time

	int P1Score = 0;
	int P2Score = 0;

	bool running = true;
	bool buttons[4] = {};



	while (running) {
		//two different timers live here
		//one for fps limiter, another for tracking the time between the frames
		auto startTime = std::chrono::high_resolution_clock::now();
		//event stack, gonna keep track of keeb and mouse interaction
		SDL_Event event;
		//Uint64 start = SDL_GetPerformanceCounter();
		//while we have not run out of events



		while (SDL_PollEvent(&event)) {
			//if one presses the quit button, we stop running the show

			switch (event.type) {

			case SDL_QUIT:

				running = false;

				break;

			case SDL_MOUSEMOTION:

				break;

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
			case SDL_KEYUP:

				if (event.key.keysym.sym == SDLK_w) {
					buttons[Buttons::P1Up] = false;
				}
				else if (event.key.keysym.sym == SDLK_s) {
					buttons[Buttons::P1Down] = false;
				}
				else if (event.key.keysym.sym == SDLK_UP) {
					buttons[Buttons::P2Up] = false;
				}
				else if (event.key.keysym.sym == SDLK_DOWN) {
					buttons[Buttons::P2Down] = false;
				}
				break;
			}

		}

		if (buttons[Buttons::P1Up]) {
			P1.velocity.y = -Paddle_Speed; //values increase from top of screen to bottom.
			//so ideologicaly we are goin "down"
		}
		else if (buttons[Buttons::P1Down]) {
			P1.velocity.y = Paddle_Speed;
		}
		else {
			P1.velocity.y = 0.0f;
		}

		if (buttons[Buttons::P2Up])
		{
			P2.velocity.y = -Paddle_Speed;
		}
		else if (buttons[Buttons::P2Down])
		{
			P2.velocity.y = Paddle_Speed;
		}
		else
		{
			P2.velocity.y = 0.0f;
		}

		P1.Update(dt);
		P2.Update(dt);

		ball.Update(dt);


		if (Contact contact = PaddleCollision(ball, P1); contact.type != CollisionType::None) {
			ball.CollideWithPaddle(contact);
		}
		else if (contact = PaddleCollision(ball, P2);  contact.type != CollisionType::None) {
			ball.CollideWithPaddle(contact);
		}
		else if (contact = WallCollision(ball); contact.type != CollisionType::None) {
			ball.CollideWithWall(contact);

			if (contact.type == CollisionType::Left) {
				++P2Score;
				P2ScoreText.SetScore(P2Score);
			}
			else if (contact.type == CollisionType::Right) {
				++P1Score;
				P1ScoreText.SetScore(P1Score);
			}
		}



		//	auto stopTime = std::chrono::high_resolution_clock::now();

		//	float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
		//	SDL_Delay(floor(16.6f - elapsedMS));
			//clear the window to black
		SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0x0);
		SDL_RenderClear(renderer);
		//drawing the net
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); //set draw color to white

		for (int y = 0; y < Win_Height; ++y) {
			if (y % 10) {
				SDL_RenderDrawPoint(renderer, Win_Width / 2, y);//draw the net at Width/2,y
			}

		}
		//draw the ball, paddle, score.
		SDL_Surface* image = IMG_Load("Screen.png");
		SDL_Texture* loadedSurface = SDL_CreateTextureFromSurface(renderer, image);
		SDL_RenderCopy(renderer, loadedSurface, nullptr, NULL);
		//SDL_RenderFillRect(renderer, &rect);

		ball.Draw(renderer);
		P1.Draw(renderer);
		P2.Draw(renderer);
		P1ScoreText.Draw();
		P2ScoreText.Draw();
		SDL_RenderPresent(renderer);
		SDL_DestroyTexture(loadedSurface);
		SDL_FreeSurface(image);

		auto stopTime = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float, std::chrono::milliseconds::period>(stopTime - startTime).count();
		//dt = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(scoreFont);
	TTF_Quit();
	SDL_Quit();

	return 0;
}
