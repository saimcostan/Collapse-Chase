# Collapse Chase

Collapse Chase is a two-player chase game written in C++ with SFML. One player is "It" and has to tag the other before time runs out, while the walls of the arena slowly close in. It's inspired by the game Tag by Jet Games.

## How it works

When a match starts, one player is picked at random to be "It". A triangle above their head shows who holds the tag. If "It" touches the other player, the tag passes over and the triangle moves with it. The newly tagged player is frozen for a moment so the other one has time to run.

The match ends when the timer hits zero, and the screen shows who lost. Meanwhile the walls keep moving inward, so the space gets smaller the longer you play.

## Controls

- Player 1: W to jump, A to move left, D to move right
- Player 2: Up arrow to jump, Left and Right arrows to move

You can only jump while standing on the ground or a platform.

## How we built it

The game came together in stages, each one adding something new.

1. **Window and first player (Saim Rashid).** A basic SFML window, a CMake setup that works in both VS Code and Visual Studio, and a rectangle player moving with W, A and D inside the window. Update and Draw functions were added to keep logic and rendering separate.
2. **Second player (Ammad-uddin).** A second, differently coloured rectangle controlled with the arrow keys, also kept inside the window.
3. **Gravity (Narmeen Bajwa).** Gravity was added to both players, so jumps follow a smooth arc. Jumping is only allowed from the ground.
4. **The "It" marker (Ammad-uddin).** A triangle showing who is "It", chosen randomly at the start and switched whenever the players touch.
5. **Code cleanup (Saim Rashid).** Player data was grouped into a single struct, with a MakePlayer function to set each one up. This removed duplicated code without changing how the game plays.
6. **Levels and platforms (Ammad-uddin).** Levels are loaded from a text file and contain platforms that players can land on.
7. **Timer and freeze (Narmeen Bajwa).** A countdown timer at the top, an end screen showing the loser, and the freeze after a tag. An Arial font was added for the text.
8. **Collapsing walls (Ammad-uddin).** Walls that close in over time, with players unable to move past them.

## Tech

C++, SFML and CMake. It builds in both VS Code and Visual Studio.

## Building

You need a C++ compiler, CMake and SFML installed.

```
git clone https://github.com/saimcostan/Collapse-Chase.git
cd Collapse-Chase
cmake -S . -B build
cmake --build build
```

Then run the executable from the build folder.
