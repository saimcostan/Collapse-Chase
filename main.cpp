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
int FreezeTime = 90;
int MatchTime = 90 * 60;
int RemainingTime = MatchTime;
bool GameOver = false;
Font GameFont;

//setting the rows and cols for the 2d array for finding the text file (the hashes)
const int Rows = 27;
const int Cols = 40;
// setting the size of the platform
const float TileSize = 30.f;

// Variables for the walls used to collapse the screen
RectangleShape LeftWall;
RectangleShape RightWall;

float WallWidth = 0.f;
const float MaxWallWidth = WindowW * 0.25f;

// Speed for the collapsing walls
float WallSpeed = 0.050f;

char Level[Rows][Cols];



// creating a player structure which contains information about each player rather than storing it in individual variables
struct Player{
    RectangleShape Body;
    Vector2f Velocity;
    Keyboard::Key left, right, jump;
    bool IsOnGround;
    int Freezed = 0;
};
//variable to store the player who is currently "it"
Player It;


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

//=====================================================================================================

// creating a platform structure which contains the shape of the platform
struct Platform {
    RectangleShape Body;
};
vector<Platform> Platforms;


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

//======================================================================================================

void CheckTag(Player& p1, Player& p2, CircleShape& triangle) {

	//checking if the player is tagged or not and setting triangle position according to it
    if (randomNum == 1) {
        triangle.setPosition(p1.Body.getPosition().x, p1.Body.getPosition().y - PlayerH - 15.f);
        It = p1;
    }
    else {
        triangle.setPosition(p2.Body.getPosition().x, p2.Body.getPosition().y - PlayerH - 15.f);
        It = p2;
    }
    //checking if players can tag each other or not and freezing the player who just got tagged
    if (It.Freezed <= 0) {
        if (p1.Body.getGlobalBounds().intersects(p2.Body.getGlobalBounds())) {
            if (randomNum == 1) {
                randomNum = 2;
                p2.Freezed = FreezeTime;
            }
            else {
                randomNum = 1;
                p1.Freezed = FreezeTime;
            }
        }
    }
}

// Function used to update player movements according to user input

//======================================================================================================

void UpdateWalls()
{
    // momentarily increase the width of both walls
    if (WallWidth < MaxWallWidth)
    {
        WallWidth += WallSpeed;

        if (WallWidth > MaxWallWidth)
        {
            WallWidth = MaxWallWidth;
        }
    }

    // accumulating the size of both walls
    LeftWall.setSize(Vector2f(WallWidth, WindowH));
    RightWall.setSize(Vector2f(WallWidth, WindowH));

    // Positioning the walls
    LeftWall.setPosition(0.f, 0.f);
    RightWall.setPosition(WindowW - WallWidth, 0.f);

    // wall colours
    LeftWall.setFillColor(Color(80, 80, 80));
    RightWall.setFillColor(Color(80, 80, 80));
}
void Update(Player& p, RenderWindow& window) {

	//boolean variable to check if the player is freezed or not
    bool CanMove = true;
    if (p.Freezed > 0) {
        p.Freezed--;
        CanMove = false;
    }
    // resetting horizontal veolcity to zero to avoid non stop movement
    p.Velocity.x = 0.f;
    float HalfW = PlayerW / 2.f;


    if (CanMove && Keyboard::isKeyPressed(p.left)) {
        p.Velocity.x = -5.f;
    }
    if (CanMove && Keyboard::isKeyPressed(p.right)) {
        p.Velocity.x = 5.f;
    }

    // checking if player is on ground to make sure that only one single jump is made from ground
    if (CanMove && Keyboard::isKeyPressed(p.jump) && p.IsOnGround) {
        p.Velocity.y = -12.f;
        p.IsOnGround = false;
    }

    float PreviousFeetY = p.Body.getPosition().y;
    // adding a constant value of gravity in each frame to make the player moves down smoothly after jumping
    p.Velocity.y += Gravity;
    // variable which contain initial positon currently
    Vector2f pos = p.Body.getPosition();
    // including vertical and horizontal changes in positon
    pos += p.Velocity;
    // checking if player is within left and right boundaries and pushing it back if not
    p.IsOnGround = false;

    // current limits of the playable area
    float LeftBoundary = WallWidth;
    float RightBoundary = WindowW - WallWidth;

    // limiting player from crossing the left wall
    if (pos.x - HalfW < LeftBoundary)
    {
        pos.x = LeftBoundary + HalfW;
        p.Velocity.x = 0.f;
    }

    // limiting player from crossing the right wall
    if (pos.x + HalfW > RightBoundary)
    {
        pos.x = RightBoundary - HalfW;
        p.Velocity.x = 0.f;
    }
    // making sure the player does not fall below the ground
    if (pos.y > GroundY) {
        pos.y = GroundY;
        p.Velocity.y = 0.f;
        p.IsOnGround = true;
    }

    p.Body.setPosition(pos);
    // calling the function by passing player and their previous feet
    CheckPlatformCollision(p, PreviousFeetY);

}

//======================================================================================================

// displays the timer and the end result of the game on the screen
void DrawText(RenderWindow& window, const string& ThisString, int Size, float XPos, float YPos, Color ThisCol) {

    Text t(ThisString, GameFont, Size);
    FloatRect Dimesions = t.getLocalBounds();
    t.setOrigin(Dimesions.left + Dimesions.width / 2.f, Dimesions.top + Dimesions.height / 2.f);
    t.setPosition(XPos, YPos);
    t.setFillColor(ThisCol);
    window.draw(t);
}

// Function used to display the window and players
void Draw(RenderWindow& window, Player& p1, Player& p2, CircleShape& triangle, vector<Platform>& Platforms) {

    // adding window color, both players and triangle tag to the screen
    window.clear(Color(0xADD8E6));
    // drawing each platform
    for (int i = 0; i < Platforms.size(); i++)
    {
        window.draw(Platforms[i].Body);
    }

    // Drawing the collapsing walls
    window.draw(LeftWall);
    window.draw(RightWall);

    window.draw(p1.Body);

    window.draw(p2.Body);
    window.draw(triangle);

	// displaying the timer on the screen
    DrawText(window, to_string((RemainingTime + 59) / 60), 70, WindowW / 2.f, 50.f, Color::White);

	// displaying the winner of the game at the end
    if (GameOver)
    {
        string Name;
        if (randomNum == 1) {
            Name = "RED";
        }
        else {
            Name = "BLUE";
        }

        DrawText(window, Name + " LOSES!", 90, WindowW / 2.f, WindowH / 2.f, Color::Yellow);
    }

    // displaying final window
    window.display();

}

//======================================================================================================

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

	//checking if the font file is loaded or not
    if (!GameFont.loadFromFile("arial.ttf")) {
        cout << "Could not load arial.ttf";
        return 1;
    }

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
		// checking if the game is over or not
        if (!GameOver) {

            // Updating the collapsing walls
            UpdateWalls();

            RemainingTime--;
            if (RemainingTime <= 0) {
                RemainingTime = 0;
                GameOver = true;
            }
            //calling update and checktag until the game is over
            Update(player1, window);
            Update(player2, window);
            CheckTag(player1, player2, triangle);
        }
        
        Draw(window, player1, player2, triangle, Platforms);

    }
    return 0;
}