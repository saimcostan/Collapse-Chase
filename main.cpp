#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <fstream>
#include <vector>
#include <string>

using namespace sf;
using namespace std;

//variables for player size and boundaries
int randomNum; 
float Gravity = 0.5f;
float PlayerW = 50.f, PlayerH = 70.f;
float WindowW = 1200, WindowH = 810;
float GroundY = 810.f;

//setting the rows and cols for the 2d array for finding the text file (the hashes)
const int Rows = 27;
const int Cols = 40;
// setting the size of the platform
const float TileSize = 30.f;

char Level[Rows][Cols];



// creating a player structure which contains information about each player rather than storing it in individual variables
struct Player{
    RectangleShape Body;
    Vector2f Velocity;
    Keyboard::Key left, right, jump;
    bool IsOnGround;
};
// creating a platform structure which contains the shapr of the platform
struct Platform {
    RectangleShape Body;
};

vector<Platform> Platforms;

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
// for loading the level.txt file that contains the hashes and empty spaces for the platforms
void LoadLevel()
{   //opening file that contains the platforms
    ifstream file("level.txt");
    //checking if file has opened
    if (!file.is_open())
    {
        cout << "Error opening level.txt" << endl;
        return;
    }

    string line;
    // filling the array with the lines in the text file 
    for (int i = 0; i < Rows; i++)
    {
        getline(file, line);

        for (int j = 0; j < Cols; j++)
        {
            Level[i][j] = line[j];
        }
    }

    file.close();
}
// function to create the plaforms (just the display)
void CreatePlatforms()
{
    // checking the whole array and creates a platform square where ever '#' is found in the file
    for (int i = 0; i < Rows; i++)
    {
        for (int j = 0; j < Cols; j++)
        {
            if (Level[i][j] == '#')
            {
                Platform p;

                p.Body.setSize(Vector2f(TileSize, TileSize));

                p.Body.setPosition(j * TileSize, i * TileSize);

                p.Body.setFillColor(Color::Green);

                Platforms.push_back(p);
            }
        }
    }
}
// function for making the player land on the platform by checking the bounds of player and platform
void CheckPlatformCollision(Player& p, float PreviousFeetY)
{
    for (int i = 0; i < Platforms.size(); i++)
    {
        FloatRect PlayerBounds = p.Body.getGlobalBounds();
        FloatRect PlatformBounds = Platforms[i].Body.getGlobalBounds();
        // check if the player is horizontally overlapping the platform
        if (PlayerBounds.left < PlatformBounds.left + PlatformBounds.width &&
            PlayerBounds.left + PlayerBounds.width > PlatformBounds.left)
        {   
            // checking if the player was previously below the platform AND is now above the top of platform while falling
            if (PreviousFeetY <= PlatformBounds.top &&
                p.Body.getPosition().y >= PlatformBounds.top &&
                p.Velocity.y >= 0)
            {
                // resets the feet of the player to the top of the platform 
                p.Body.setPosition(
                    p.Body.getPosition().x,
                    PlatformBounds.top
                );
                // resets the vertical velocity to 0 AND setting IsOnGround to true for next jump
                p.Velocity.y = 0.f;
                p.IsOnGround = true;
            }
        }
    }
}

// Function used to update player movements according to user input
void Update(Player &p1, Player &p2, RenderWindow &window, CircleShape &triangle);

// Function used to display the window and players
void Draw(RenderWindow &window, Player &p1, Player &p2, CircleShape &triangle, vector<Platform>& Platforms);
void LoadLevel();
void CreatePlatforms();
void CheckPlatformCollision(Player& p, float PreviousFeetY);
int main() {

    // creating a window and player and setting up their dimensions
    RenderWindow window(sf::VideoMode(WindowW,WindowH), "Collapse Chase");

    // Capping the frame rate
    window.setFramerateLimit(60);

    LoadLevel();
    CreatePlatforms();


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
        Draw(window, player1, player2, triangle, Platforms);

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
        p1.Velocity.y = -13.f;
        p1.IsOnGround = false;
    }
    float PreviousFeetY1 = p1.Body.getPosition().y;

    p1.IsOnGround = false;

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
    // caliing the function by passing player and their previous feet
    CheckPlatformCollision(p1, PreviousFeetY1);
  

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
    float PreviousFeetY2 = p2.Body.getPosition().y;

    p2.IsOnGround = false;

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
    CheckPlatformCollision(p2, PreviousFeetY2);


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

void Draw(RenderWindow &window, Player &p1, Player &p2, CircleShape& triangle, vector<Platform>& Platforms){

    // adding window color, both players and triangle tag to the screen
    window.clear(Color(0xADD8E6));
    // drawing each platform
    for (int i = 0; i < Platforms.size(); i++)
    {
        window.draw(Platforms[i].Body);
    }
    window.draw(p1.Body);
   
    window.draw(p2.Body);
    window.draw(triangle);
    // displaying final window
    window.display();

}

