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

// Diferent game states with 1 of 4 active at a time
enum class Screen{Menu, CountDown, Play, End};

// initially the winsow should start from the menu screen 
Screen CurrentScreen = Screen::Menu;

// variables to give a countdown of 3 sec before the actual game starts
const int CountDownTime = 3 * 60;
int CountDownFrames = 0;


// creating a player structure which contains information about each player rather than storing it in individual variables
struct Player{
    RectangleShape Body;
    Vector2f Velocity;
    Keyboard::Key left, right, jump;
    bool IsOnGround;
    int Freezed = 0;
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

void CheckTag(Player& p1, Player& p2) {

    // variable to store freeze time of each player
    int ItFreeze;
	//checking if the player is tagged or not and storing freeze time of the tagged player
    if (randomNum == 1) {
        ItFreeze = p1.Freezed;
    }
    else {
        ItFreeze = p2.Freezed;
    }

    //checking if players can tag each other or not and freezing the player who just got tagged
    if (ItFreeze <= 0) {
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

// gives a fresh start if the user wants to go for another match within the same window running
void ResetGame(Player &p1, Player &p2){

    // using MakePlayer to pass values to each characteristic of the player struct according to each player
    p1 = MakePlayer(Color::Red, Vector2f(300.f, GroundY), Keyboard::A, Keyboard::D, Keyboard::W);
    p2 = MakePlayer(Color::Blue, Vector2f(600.f, GroundY), Keyboard::Left, Keyboard::Right, Keyboard::Up);

    // random number generation for starting tag assignment
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(1, 2);
    randomNum = dist(rng);
    // The game starts with the total time of the game
    RemainingTime = MatchTime;

    // resetting the walls back to the edges of the window for a new game
    WallWidth = 0.f;
    LeftWall.setSize(Vector2f(0.f, WindowH));
    RightWall.setSize(Vector2f(0.f, WindowH));

    LeftWall.setPosition(0.f, 0.f);
    RightWall.setPosition(WindowW, 0.f);

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

// Used to display any kind of text on the window that is required by any screen
void DrawText(RenderWindow& window, const string& ThisString, int Size, float XPos, float YPos, Color ThisCol) {

    Text t(ThisString, GameFont, Size);
    FloatRect Dimesions = t.getLocalBounds();
    t.setOrigin(Dimesions.left + Dimesions.width / 2.f, Dimesions.top + Dimesions.height / 2.f);
    t.setPosition(XPos, YPos);
    t.setFillColor(ThisCol);
    window.draw(t);
}

// Function used to display the Play screen and everything it includes
void DrawWorld(RenderWindow& window, Player& p1, Player& p2, CircleShape& triangle) {


    // drawing each platform
    for (int i = 0; i < Platforms.size(); i++)
    {
        window.draw(Platforms[i].Body);
    }

    // Drawing the collapsing walls
    window.draw(LeftWall);
    window.draw(RightWall);

    // drawing the bodies of both the players
    window.draw(p1.Body);

    window.draw(p2.Body);

    // checking the tagged player and drawing triangle tag according to it
    Vector2f Feet;
    if (randomNum == 1){
        Feet = p1.Body.getPosition();
    }
    else{
        Feet = p2.Body.getPosition();
    }
    triangle.setPosition(Feet.x, Feet.y - PlayerH - 15.f);
    window.draw(triangle);

	// displaying the timer on the Play screen
    DrawText(window, to_string((RemainingTime + 59) / 60), 70, WindowW / 2.f, 50.f, Color::White);

}

//======================================================================================================

int main() {

    // creating a window and player and setting up their dimensions
    RenderWindow window(sf::VideoMode(WindowW,WindowH), "Collapse Chase");

    // Capping the frame rate
    window.setFramerateLimit(60);

    LoadLevel();
    CreatePlatforms();

	//checking if the font file is loaded or not
    if (!GameFont.loadFromFile("arial.ttf")) {
        cout << "Could not load arial.ttf";
        return 1;
    }

    // declaring to players and passing them to reset game
    Player player1, player2;
    ResetGame(player1, player2);

    // creating a triangle above the player to represent tag
    CircleShape triangle(20.f, 3);

    //setting up the colour of triangle tag
    triangle.setFillColor(Color::White);
    // setting origin for allignment
    triangle.setOrigin(triangle.getLocalBounds().width / 2.f, triangle.getLocalBounds().height / 2.f);


    // loop that keeps running until window is closed
    while (window.isOpen()) {
        Event event;

        // loop that checks if there is any change of events and handles them one by one
        while (window.pollEvent(event)) {
            // two ways to close window
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::KeyPressed){
                // checking if esc key is pressed
                if (event.key.code == Keyboard::Escape ){

                    // if menu screen is showing then closes the window
                    if (CurrentScreen == Screen::Menu){
                        window.close();
                    }
                    // if other screens are being shown then returns the screen to menu
                    else{
                        CurrentScreen = Screen::Menu;
                    }
                }
                // if Enter key is pressed in Menu or End screens then moves to CountDown screen and starts a new game
                if ((event.key.code == Keyboard::Enter) && (CurrentScreen == Screen::Menu || CurrentScreen == Screen::End)){

                    ResetGame(player1, player2);
                    // 3 sec countdown starts before the actual game starts
                    CurrentScreen = Screen::CountDown;
                    CountDownFrames = CountDownTime;
                }
            }

        }
		if (CurrentScreen == Screen::CountDown){
            
            // decreasing countdown frames each second and moving to Play screen once it reaches zero
            CountDownFrames--;
            if (CountDownFrames <= 0){
                CurrentScreen = Screen::Play;
            }

            //resetting the radius of triangle tag
            triangle.setRadius(20);
        }
        else if (CurrentScreen == Screen::Play){

            // starting will shrinking once in play state and decreasing game time each second
            UpdateWalls();
            RemainingTime--;

            // calling update for player movements
            Update(player1, window);
            Update(player2, window);

            // checking tag between both players
            CheckTag(player1, player2);

            // moving to End screen once remaining time hits zero
            if (RemainingTime <= 0){
                RemainingTime = 0;
                CurrentScreen = Screen::End;
            }
        }
        // Background colour for all the screens
        window.clear(Color(0, 191, 255));

        if (CurrentScreen == Screen::Menu){

            // passing all the texts to display in the menu screen
            DrawText(window, "COLLAPSE CHASE", 100, WindowW/2.f, 250.f, Color::White);
            DrawText(window, "Press ENTER to start", 50, WindowW/2.f, 400.f, Color::White);
            DrawText(window, "CONTROLS:", 50, WindowW/2.f, 480.f, Color::White);
            DrawText(window, "RED:  A D W    BLUE:  Arrow keys", 40, WindowW/2.f, 550.f, Color::White);
            DrawText(window, "Press ESC to quit", 40, WindowW/2.f, 650.f, Color::White);
        }
        else{
            // loading all the players and platforms
            DrawWorld(window, player1, player2, triangle);

            // showing the countdown timer in the middle of the screen
            if (CurrentScreen == Screen::CountDown){
                DrawText(window, to_string((CountDownFrames + 59)/ 60), 200, WindowW/2.f, WindowH/2.f, Color::White);
            }
            // 
            if (CurrentScreen == Screen::End){
                // covering all the players and platforms with a large rectangle to show end result
                RectangleShape EndBackground(Vector2f(WindowW, WindowH));
                EndBackground.setFillColor(Color(0, 191, 255));
                window.draw(EndBackground);
            
                // used for the triangle positioning to display at the end
                Vector2f Feet;

                // name of the player who remained tag at the end
                string Name;
                // storing name and displaying the player who lost
                if (randomNum == 1){ 
                    Name = "RED";
                    // increasing the size for the end
                    player1.Body.setSize(Vector2f(PlayerW * 2.5 , PlayerH * 2.5));
                    player1.Body.setPosition(WindowW/2.f - 50.f, 200);
                    // storing position for the triangle
                    Feet = player1.Body.getPosition();
                    window.draw(player1.Body);
                }
                else{
                    // same process for the other player
                    Name = "BLUE";
                    player2.Body.setSize(Vector2f(PlayerW * 2.5 , PlayerH * 2.5));
                    player2.Body.setPosition(WindowW/2.f - 50.f, 200);
                    Feet = player2.Body.getPosition();
                    window.draw(player2.Body);
                }

                // displaying the larger triangle and positioning according to Feet
                triangle.setRadius(60);
                triangle.setPosition(Feet.x - 4.f, Feet.y - PlayerH * 2 - 8.f);
                window.draw(triangle);
                
                // Final result text
                DrawText(window, Name + " LOSES!", 90, WindowW / 2.f, WindowH / 2.f, Color::Yellow);
                // options for user to enter
                DrawText(window, "ENTER = play again    ESC = menu", 40, WindowW/2.f, 500.f, Color::White);
                
                }
        }

        // displaying final window
        window.display();
    }
    return 0;
}