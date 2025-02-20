# Snake


## TODOS
- [ ] Add replay feature
- [ ] Display score
- [ ] Allow user to change colors of the snake body, grid color, outline colors and save it to a config file on startup
- [ ] Modulize the game into smaller classes
## Libraries used
- SFML 3.0 (https://www.sfml-dev.org/)
- ImGUI-SFML (https://github.com/SFML/imgui-sfml)

## Learning points
- Moving the snake body across the grid
- Using std::deque to control the length of the snake. Allowed me to follow a push_front(nextpos), pop_back() algorithm for traversing the grid.
  Simply removing pop_back(), if the snake collides with a fruit
- At first I overcomplicated it by storing std::vector<sf::RectangleShape>.
  This wasn't needed because the snake solely moved based on the grid,which meant I only needed to manipulate the position
  of one sf::RectangleShape.
- Play around with ImGUI

## ImGUI-SFML Thoughts
- It's very convenient to be able to tinker with the variables using a GUI
- Easy to use
- Easy to set up
- Plug and play

## Useful tools that helped
- https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html

## Running this
- You will need SFML 3.0, imgui-sfml, compiler
- Follow instructions on SFML 3.0 (https://www.sfml-dev.org/) for your machine
- Follow instructions for imgui-sfml installation
# Windows
- Visual Studio 2019, or GCC, CLANG etc
