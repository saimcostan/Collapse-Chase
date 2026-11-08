#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
int randomNum;
using namespace sf;

//variables for player movements 
float Gravity = 0.5f;
float VerticalVelocity1 = 0.f;
float VerticalVelocity2 = 0.f;
bool IsOnGround1 = true;
bool IsOnGround2 = true;
float GroundY = 630.f;

// Function used to update player movements according to user input
void Update(RectangleShape &Player1, RectangleShape &Player2, RenderWindow &window,CircleShape& triangle1);

// Function used to display the window and players
void Draw(RectangleShape &Player1, RectangleShape &Player2, RenderWindow &window, CircleShape& triangle1);

int main() {


    // creating a window and player and setting up their dimensions
    RenderWindow window(sf::VideoMode(1000, 700), "Collapse Chase");
    RectangleShape Player1(Vector2f(50.f, 70.f));
    RectangleShape Player2(Vector2f(50.f, 70.f));

    // Capping the frame rate
    window.setFramerateLimit(60);

    // Setting up initial player position and color
    Player1.setFillColor(Color::Red);
    Player1.setPosition(Vector2f(300.f, 620.f));
    Player2.setFillColor(Color::Blue);
    Player2.setPosition(Vector2f(600.f, 620.f));

    // creating a triangle above the player to represent tag
    CircleShape triangle1(20.f, 3);

    //setting up the colour and position of triangle tag
    triangle1.setFillColor(Color::Green);
    triangle1.setPosition(500.f, 500.f);

    // generating a random number either 1 or 2 so the tag gets assigned to the players at random
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(1, 2);
    randomNum = dist(rng);

    // loop that keeps running until window is closed
    while (window.isOpen()) {
        Event event;

        // loop that checks if there is any change of events and handles them one by one
        while (window.pollEvent(event)) {
            // two ways to close window
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                window.close();

        }
        
        Update(Player1,Player2, window,triangle1);
        Draw(Player1,Player2, window,triangle1);

    }
    return 0;
}

void Update(RectangleShape& Player1, RectangleShape& Player2, RenderWindow& window, CircleShape& triangle1) {

    // Checking which key was pressed and if the player is within the boundary of the window
    
    if (Keyboard::isKeyPressed(Keyboard::A) && Player1.getPosition().x > 0) {
        Player1.move(-5.f, 0.f);
    }

    if (Keyboard::isKeyPressed(Keyboard::D) && (Player1.getPosition().x + Player1.getGlobalBounds().width) < window.getSize().x) {
        Player1.move(5.f, 0.f);
    }

	if (Keyboard::isKeyPressed(Keyboard::W) && IsOnGround1) { // If the player is on the ground and the jump key is pressed, initiate a jump
        VerticalVelocity1 = -12.f;
        IsOnGround1 = false;
    }

	//adding constant gravity in vertical velocity in every frame and moving the player downwards according to the velocity
    VerticalVelocity1 += Gravity;
    Player1.move(0.f, VerticalVelocity1);// the changing velocity shows a smooth jump

    if (Player1.getPosition().y + Player1.getGlobalBounds().height >= window.getSize().y) {
        // landing the player to the ground without it falling out of the window
        Player1.setPosition(Player1.getPosition().x, GroundY);
        IsOnGround1 = true;
		VerticalVelocity1 = 0.f; // Resetting the vertical velocity when the player lands on the ground
    }
  
  
    // The same process is repeated for the movement of player 2
    if (Keyboard::isKeyPressed(Keyboard::Left) && Player2.getPosition().x > 0) {
        Player2.move(-5.f, 0.f);
    }

    if (Keyboard::isKeyPressed(Keyboard::Right) && (Player2.getPosition().x + Player2.getGlobalBounds().width) < window.getSize().x) {
        Player2.move(5.f, 0.f);
    }
    // the same physics is applied to player 2
    if (Keyboard::isKeyPressed(Keyboard::Up) && IsOnGround2) {

        VerticalVelocity2 = -12.f;
        IsOnGround2 = false;
    }

    VerticalVelocity2 += Gravity;
    Player2.move(0.f, VerticalVelocity2);

    if (Player2.getPosition().y + Player2.getGlobalBounds().height >= window.getSize().y) {

        Player2.setPosition(Player2.getPosition().x, GroundY);
        IsOnGround2 = true;
        VerticalVelocity2 = 0.f;
    }
    if (randomNum == 1) {
        triangle1.setPosition(Player1.getPosition().x + 7.f, Player1.getPosition().y - 35.f);
    }
    else {
        triangle1.setPosition(Player2.getPosition().x + 7.f, Player2.getPosition().y - 35.f);
    }
    if (Player1.getGlobalBounds().intersects(Player2.getGlobalBounds())) {
        if (randomNum == 1) {
            randomNum = 2;
        }
        else {
            randomNum = 1;
        }
    }
}

void Draw(RectangleShape &Player1, RectangleShape &Player2, RenderWindow &window, CircleShape& triangle1){

    // adding window color,both players and triangle tag to the screen
    window.clear(Color(0xADD8E6));
    window.draw(Player1);
   
    window.draw(Player2);
    window.draw(triangle1);
    // displaying final window
    window.display();

}

