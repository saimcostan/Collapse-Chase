#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
using namespace sf;

//variables for player size and boundaries
int randomNum; 
float Gravity = 0.5f;
float PlayerW = 50.f, PlayerH = 70.f;
float WindowW = 1000, WindowH = 700;
float GroundY = 700.f;

// creating a player structure which contains information about each player rather than storing it in individual variables
struct Player{
    RectangleShape Body;
    Vector2f Velocity;
    Keyboard::Key left, right, jump;
    bool IsOnGround;
};

// creating a function which takes in info about the player and assigns them to the structure
Player MakePlayer(Color color, Vector2f Feet, Keyboard::Key Left, Keyboard::Key Right, Keyboard::Key Jump){
    Player p;
    p.Body.setSize(Vector2f(PlayerW, PlayerH));
    // setting the origin of the player to bottom centre rather than the default (top left)
    p.Body.setOrigin(Vector2f(PlayerW/2, PlayerH));
    p.Body.setPosition(Feet);
    p.Body.setFillColor(color);
    p.left = Left;
    p.right = Right;
    p.jump = Jump;
    p.IsOnGround = true;

    return p;
}

// Function used to update player movements according to user input
void Update(Player &p1, Player &p2, RenderWindow &window, CircleShape &triangle);

// Function used to display the window and players
void Draw(RenderWindow &window, Player &p1, Player &p2, CircleShape &triangle);

int main() {

    // creating a window and player and setting up their dimensions
    RenderWindow window(sf::VideoMode(1000, 700), "Collapse Chase");

    // Capping the frame rate
    window.setFramerateLimit(60);

    // Calling MakePlayer to add the given arguments to the structure of both the players
    Player player1 = MakePlayer(Color::Red, Vector2f(300.f, GroundY), Keyboard::A, Keyboard::D, Keyboard::W);
    Player player2 = MakePlayer(Color::Blue, Vector2f(600.f, GroundY), Keyboard::Left, Keyboard::Right, Keyboard::Up);
    

    // creating a triangle above the player to represent tag
    CircleShape triangle(20.f, 3);

    //setting up the colour and position of triangle tag
    triangle.setFillColor(Color::White);
    triangle.setPosition(500.f, 500.f);
    // setting origin for allignment
    triangle.setOrigin(triangle.getLocalBounds().width / 2.f, triangle.getLocalBounds().height / 2.f);

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
        
        Update(player1,player2, window,triangle);
        Draw(window, player1, player2, triangle);

    }
    return 0;
}

void Update(Player &p1, Player &p2, RenderWindow &window, CircleShape &triangle) {

    // resetting horizontal veolcity to zero to avoid non stop movement
    p1.Velocity.x = 0.f;
    
    float HalfW = PlayerW / 2.f;
    
    // Checking which key was pressed and changing player movement accordingly
    if (Keyboard::isKeyPressed(p1.left)){
        p1.Velocity.x = -5.f; 
    }

    if (Keyboard::isKeyPressed(p1.right)){
        p1.Velocity.x = 5.f;
    }

    // checking if player is on ground to make sure that only one single jump is made from ground
    if (Keyboard::isKeyPressed(p1.jump) && p1.IsOnGround){
        p1.Velocity.y = -12.f;
        p1.IsOnGround = false;
    }

    // adding a constant value of gravity in each frame to make the player moves down smoothly after jumping
    p1.Velocity.y += Gravity;

    // variable which contain initial positon currently
    Vector2f pos = p1.Body.getPosition();

    // including vertical and horizontal changes in positon
    pos += p1.Velocity;

    // checking if player is within left and right boundaries and pushing it back if not
    if (pos.x - HalfW < 0.f){
        pos.x = HalfW;
        p1.Velocity.x = 0.f;
    }
    
    if (pos.x + HalfW > window.getSize().x){
        pos.x = window.getSize().x - HalfW;
        p1.Velocity.x = 0.f;
    }

    // making sure the player does not fall below the ground
    if (pos.y > GroundY){
        pos.y = GroundY;
        p1.Velocity.y = 0.f;
        p1.IsOnGround = true;
    }
    
    // setting the altered position as the new position of the player in each frame
    p1.Body.setPosition(pos);
  

    // The same process is repeated for the movement of player 2  
    p2.Velocity.x = 0.f;
    
    if (Keyboard::isKeyPressed(p2.left)){
        p2.Velocity.x = -5.f; 
    }
    if (Keyboard::isKeyPressed(p2.right)){
        p2.Velocity.x = 5.f;
    }

    if (Keyboard::isKeyPressed(p2.jump) && p2.IsOnGround){
        p2.Velocity.y = -12.f;
        p2.IsOnGround = false;
    }

    p2.Velocity.y += Gravity;
    pos = p2.Body.getPosition();
    pos += p2.Velocity;

    if (pos.x - HalfW < 0.f){
        pos.x = HalfW;
        p2.Velocity.x = 0.f;
    }
    
    if (pos.x + HalfW > window.getSize().x){
        pos.x = window.getSize().x - HalfW;
        p2.Velocity.x = 0.f;
    }

    if (pos.y > GroundY){
        pos.y = GroundY;
        p2.Velocity.y = 0.f;
        p2.IsOnGround = true;
    }

    p2.Body.setPosition(pos);


    // checking where to add the triangle based on the random number generated
    if (randomNum == 1) {
        triangle.setPosition(p1.Body.getPosition().x , p1.Body.getPosition().y - PlayerH - 15.f);
    }
    else {
        triangle.setPosition(p2.Body.getPosition().x , p2.Body.getPosition().y - PlayerH - 15.f);
    }

    // changing the position of the triangle to follow the other player if a collision is detected
    if (p1.Body.getGlobalBounds().intersects(p2.Body.getGlobalBounds())) {
        if (randomNum == 1) {
            randomNum = 2;
        }
        else {
            randomNum = 1;
        }
    }
}

void Draw(RenderWindow &window, Player &p1, Player &p2, CircleShape& triangle){

    // adding window color, both players and triangle tag to the screen
    window.clear(Color(0xADD8E6));
    window.draw(p1.Body);
   
    window.draw(p2.Body);
    window.draw(triangle);
    // displaying final window
    window.display();

}

