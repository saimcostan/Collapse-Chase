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
float PlayerW = 60.f, PlayerH = 70.f;
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

//variables for the portals and jumppad
float PortalW = 40.f, PortalH = 80.f;
int PortalLockFrames = 180;
int PortalLock = 0;

Vector2f PortalPos1(350.f, 210.f);
Vector2f PortalPos2(750.f, 690.f);

Texture PortalTex1;
Texture PortalTex2;

Sprite Portal1, Portal2;

float JumpPadW = 50.f, JumpPadH = 35.f;
float HighJumpForce = -20.f;

Vector2f JumpPadPos(520.f, GroundY);

Texture JumpPadTex;
Sprite JumpPad;

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
    Sprite Visual;
    Vector2f Velocity;
    Keyboard::Key left, right, jump;
    bool IsOnGround;
    int Freezed = 0;
};


// creating a function which takes in info about the player and assigns them to the structure
Player MakePlayer(Texture& texture, Vector2f Feet,
    Keyboard::Key Left, Keyboard::Key Right, Keyboard::Key Jump)
{
    Player p;

    // Invisible rectangle hitbox
    p.Body.setSize(Vector2f(PlayerW-5.f, PlayerH));
    p.Body.setOrigin(PlayerW / 2.f, PlayerH);
    p.Body.setPosition(Feet);
    p.Body.setFillColor(Color::Transparent);

    // Visible player sprite
    p.Visual.setTexture(texture);

    // Make sprite the same size as the old rectangle
    p.Visual.setScale(
        PlayerW / texture.getSize().x,
        PlayerH / texture.getSize().y
    );

    // Align sprite bottom-centre with the rectangle bottom-centre
    p.Visual.setOrigin(
        texture.getSize().x / 2.f,
        texture.getSize().y
    );

    p.Visual.setPosition(Feet);

    p.left = Left;
    p.right = Right;
    p.jump = Jump;

    p.IsOnGround = true;

    return p;
}
Texture Player1Texture;
Texture Player2Texture;
//=====================================================================================================

// creating a platform structure which contains the shape of the platform
struct Platform {
    Sprite Body;
};
vector<Platform> Platforms;

Texture PlatformTexture;


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

                p.Body.setTexture(PlatformTexture);

                p.Body.setPosition(j * TileSize, i * TileSize);

                // Resize sprite to match your existing platform tile size
                p.Body.setScale(
                    TileSize / PlatformTexture.getSize().x,
                    TileSize / PlatformTexture.getSize().y
                );

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

// function to check if the players are tagged or not and freezing the player who just got tagged
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

	// unlocking the portal at the start of the game
    PortalLock = 0;

    // using MakePlayer to pass values to each characteristic of the player struct according to each player
    p1 = MakePlayer(Player1Texture, Vector2f(300.f, GroundY),Keyboard::A, Keyboard::D, Keyboard::W);
    p2 = MakePlayer(Player2Texture, Vector2f(750.f, GroundY),Keyboard::Left, Keyboard::Right, Keyboard::Up);

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

void CheckPortals(Player& p) {

    //checking if the portal is locked or the player is freezed
    if (PortalLock > 0) {
        return;
    }

    if (p.Freezed > 0) {
        return;
    }

	//checking if the player is intersecting with any of the portals and teleporting them to the other portal
    if (Portal1.getGlobalBounds().intersects(p.Body.getGlobalBounds())) {

        p.Body.setPosition(PortalPos2);
        p.Velocity.y = 0.f;
        PortalLock = PortalLockFrames;
    }
    else if (Portal2.getGlobalBounds().intersects(p.Body.getGlobalBounds())) {

        p.Body.setPosition(PortalPos1);
        p.Velocity.y = 0.f;
        PortalLock = PortalLockFrames;
    }
}
//======================================================================================================

void CheckJumpPad(Player& p, float PreviousFeetY) {
	//variables for storing the bounds of the jump pad and player
    FloatRect PadBounds = JumpPad.getGlobalBounds();
    FloatRect PlayerBounds = p.Body.getGlobalBounds();
    //checking if the player is landing on the jumppad and then giving it jump boost
    if (PlayerBounds.intersects(PadBounds)) {

        if (p.Velocity.y >= 0 && PreviousFeetY <= PadBounds.top + 10.f) {

            p.Body.setPosition(p.Body.getPosition().x, PadBounds.top);

            p.Velocity.y = HighJumpForce;
            p.IsOnGround = false;
        }
    }
}

void Update(Player& p, RenderWindow& window) {

	//boolean variable to check if the player is freezed or not
    bool CanMove = true;

    //resetting the player to full opacity 
    p.Visual.setColor(Color(255, 255, 255, 255));

    if (p.Freezed > 0) {
		// reducing the opacity of the player to show that it is freezed
        p.Visual.setColor(Color(255, 255, 255, 80));
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
    //checking portal and jumppad 
    CheckPortals(p);

    CheckJumpPad(p, PreviousFeetY);

    // Keeping the visible sprite aligned with the invisible rectangle
    p.Visual.setPosition(p.Body.getPosition());

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
	//changing opacity according to the portal lock and drawing the portals
    if (PortalLock == 0) {
        Portal1.setColor(Color(255, 255, 255, 255));
        Portal2.setColor(Color(255, 255, 255, 255));
    }
    else {
        Portal1.setColor(Color(255, 255, 255, 80));
        Portal2.setColor(Color(255, 255, 255, 80));
    }

    window.draw(Portal1);
    window.draw(Portal2);

    window.draw(JumpPad);

    // Drawing the collapsing walls
    window.draw(LeftWall);
    window.draw(RightWall);

    // drawing the visuals of both the players
    window.draw(p1.Visual);
    window.draw(p2.Visual);

    // checking the tagged player and drawing triangle tag according to it
    Vector2f Feet;
    if (randomNum == 1){
        Feet = p1.Body.getPosition();
    }
    else{
        Feet = p2.Body.getPosition();
    }
    triangle.setPosition(Feet.x - 3/2.f, Feet.y - PlayerH - 14.f);
    window.draw(triangle);

	// displaying the timer on the Play screen
    DrawText(window, to_string((RemainingTime + 59) / 60), 70, WindowW / 2.f, 50.f, Color::White);

}

//======================================================================================================

void SetupPortals(Sprite& ThisPortal, Vector2f PortalPos) {


    ThisPortal.setOrigin(PortalTex1.getSize().x / 2, PortalTex1.getSize().y);
    ThisPortal.setScale(
        PortalW / PortalTex1.getSize().x,
        PortalH / PortalTex1.getSize().y
    );

    ThisPortal.setPosition(PortalPos);
}

void SetupJumpPad(Sprite& JumpPad, Vector2f JumpPadPos) {

    JumpPad.setTexture(JumpPadTex);

    JumpPad.setOrigin(JumpPadTex.getSize().x / 2.f, JumpPadTex.getSize().y);
    JumpPad.setScale(
        JumpPadW / JumpPadTex.getSize().x,
        JumpPadH / JumpPadTex.getSize().y
    );

    JumpPad.setPosition(JumpPadPos);
}

int main() {

    // creating a window and player and setting up their dimensions
    RenderWindow window(sf::VideoMode(WindowW,WindowH), "Collapse Chase");

    // Capping the frame rate
    window.setFramerateLimit(60);

    LoadLevel();

    // Loading platform sprite
    if (!PlatformTexture.loadFromFile("assets/terrain_grass_block.png")) {
        cout << "Could not load platform.png" << endl;
        return 1;
    }

    CreatePlatforms();
    //chedcking if the portal images are loaded or not
    if (!PortalTex1.loadFromFile("assets/portal right.png")) {

        cout << "ERROR! Could not load portal right.png";
        return 0;
    }

    if (!PortalTex2.loadFromFile("assets/portal left.png")) {

        cout << "ERROR! Could not load portal left.png";
        return 0;
    }
	//setting the texture of the portal according to the image loaded and calling the setup function to set the position and size of the portal
    Portal1.setTexture(PortalTex1);
    Portal2.setTexture(PortalTex2);

    SetupPortals(Portal1, PortalPos1);
    SetupPortals(Portal2, PortalPos2);
	// checking if the jump pad image is loaded or not
    if (!JumpPadTex.loadFromFile("assets/Jump Pad.png")) {

        cout << "ERROR! Could not load Jump Pad.png";
        return 0;
    }
	// setting the texture of the jump pad according to the image loaded and calling the setup function to set the position and size of the jump pad
    SetupJumpPad(JumpPad, JumpPadPos);

    // Loading Player 1 sprite
    if (!Player1Texture.loadFromFile("assets/red sprite.png"))
    {
        cout << "Could not load Player 1 texture" << endl;
        return 1;
    }

    // Loading Player 2 sprite
    if (!Player2Texture.loadFromFile("assets/blue sprite.png"))
    {
        cout << "Could not load Player 2 texture" << endl;
        return 1;
    }

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

			// reducing the portal lock time each second if it is greater than zero
            if (PortalLock > 0) {
                PortalLock--;
            }

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
                    player1.Visual.setScale(PlayerW * 2.5f / Player1Texture.getSize().x,PlayerH * 2.5f / Player1Texture.getSize().y);

                    player1.Visual.setPosition(WindowW / 2.f , 300);
                    Feet = player1.Visual.getPosition();
                    window.draw(player1.Visual);
                }
                else{
                    // same process for the other player
                    Name = "BLUE";
                    player2.Visual.setScale(PlayerW * 2.5f / Player2Texture.getSize().x,PlayerH * 2.5f / Player2Texture.getSize().y);
                    player2.Visual.setPosition(WindowW / 2.f, 300);
                    Feet = player2.Visual.getPosition();
                    window.draw(player2.Visual);
                }

                
                
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