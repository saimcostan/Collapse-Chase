#include <SFML/Graphics.hpp>
using namespace sf;

//variables for player movements 
float Gravity = 0.5f;
float VerticalVelocity1 = 0.f;
float VerticalVelocity2 = 0.f;
bool IsOnGround1 = true;
bool IsOnGround2 = true;
float GroundY = 630.f;

// Function used to update player movements according to user input
void Update(RectangleShape &Player1, RectangleShape &Player2, RenderWindow &window);

// Function used to display the window and players
void Draw(RectangleShape &Player1, RectangleShape &Player2, RenderWindow &window);

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
        
        Update(Player1,Player2, window);
        Draw(Player1,Player2, window);

    }
    return 0;
}

void Update(RectangleShape& Player1, RectangleShape& Player2, RenderWindow& window) {

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

	//adding constant gravity in vertical velocity and moving the player downwards according to the velocity
    VerticalVelocity1 += Gravity;
    Player1.move(0.f, VerticalVelocity1);  

    if (Player1.getPosition().y + Player1.getGlobalBounds().height >= window.getSize().y) {

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

}

void Draw(RectangleShape &Player1, RectangleShape &Player2, RenderWindow &window){

    // adding window color and both players to the screen
    window.clear(Color(0xADD8E6));
    window.draw(Player1);
   
    window.draw(Player2);
    // displaying final window
    window.display();

}

