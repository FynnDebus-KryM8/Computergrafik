Solar System
============

This package contains exercise code for the lecture "Computer Graphics" by Prof. Dr. Mario Botsch, TU Dortmund.

Textures and Copyright
----------------------

All earth textures are from the [NASA Earth Observatory](http://earthobservatory.nasa.gov/Features/BlueMarble/) and have been modified by Prof. Hartmut Schirmacher, Beuth Hochschule für Technik Berlin. All other textures are from [Solarsystemscope.com](https://www.solarsystemscope.com/textures/). The ship model is from [sketchfab.com](https://sketchfab.com/3d-models/wii-super-mario-galaxy-airship-e0cbf6dba6c24e87a66231a08e3df0d3).


Documentation
-------------

A pre-built HTML documentation can be found in `doc/index.html` and can be opened via any web browser.


Prerequisites
-------------

We use [CMake](www.cmake.org) for setting up build environments. You can download it [here](https://cmake.org/download/) or by using your favorite package manager.
You will also need a C++ compiler. Depending on your OS, the typical choices are:

  * Linux - GCC/G++, usually pre-installed (terminal command: `sudo apt install build-essential`)
  * MacOS - Clang, comes with the IDE "XCode" (terminal command: `xcode-select --install`)
  * Windows - MSVC, can be installed alongside the IDE "Visual Studio Community" (see below) or separately [here](https://visualstudio.microsoft.com/downloads/#other) under "All Downloads" --> "Tools for Visual Studio" --> "Build Tools for Visual Studio 2019"

We highly recommend to use some kind of IDE. Prominent examples are:
 
 - [XCode](https://developer.apple.com/xcode/) (MacOS) 
 - [Visual Studio Community](https://visualstudio.microsoft.com/de/vs/community/) (Windows)
 - [Visual Studio Code](https://code.visualstudio.com/) (Linux, Windows, MacOS)
 - [Jetbrains CLion](https://www.jetbrains.com/de-de/clion/) (Linux, Windows, MacOS)
 
Below, we provide examples for setting up, compiling and running the project via command line, XCode, VSCommunity and VSCode.


Building on Linux via Command Line (no IDE)
-------------------------------------------

Execute the following commands in the exercise's top-level directory:

    mkdir build
    cd build
    cmake ..
    make

The last command (`make`) compiles the application. Re-run it whenever you have added/changed code in order to re-compile.

For running the code via command line use

    ./solarsystem
    
When running the `cmake ..` command, you might miss some dependencies. You can often find the missing package via a quick google search like "ubuntu missing ...".
The typical packages are given here:
    
    sudo apt-get install xorg-dev libglu1-mesa-dev libx11-dev freeglut3-dev libxmu-dev libxi-dev libgl-dev libglew-dev

Clear the build folder's content before running `cmake ..` again. 


Building on MacOS (XCode)
--------------------------

Execute the following commands in the exercise's top-level directory:

    mkdir xcode
    cd xcode
    cmake -G Xcode ..

Open the generated .xcodeproj file. Inside XCode, select the solarsystem-executable in the top bar next to the stop button. You can run the code by pressing the play button.


Building on Windows (Visual Studio Community) 
---------------------------------------------

* In order to get the MSVC C++ compiler, make sure that you check "Desktop development with C++" during installation of [VSCommunity](https://visualstudio.microsoft.com/de/vs/community/)
* Create an empty build folder inside the project's top-level directory
* Start cmake-gui.exe (located in cmake's bin folder)
* Specify the top-level directory as source directory (button Browse source...)
* Specify the previously created build folder as build directory (button Browse build...)
* Select "Configure" using your Visual Studio Version as option.
* When configuration is finished, select "Generate".
* Start Visual Studio Community
* Open the project via File -> open -> project -> .sln in build folder
* In the project explorer window on the right, right-click the project (solarsystem) and set it as startup-project
* Switch to release mode
* Hit CTRL + F5 to build and run (or CTRL + SHIFT + B to build)


Building via VSCode
-------------------

There are a lot of useful extensions for VSCode to shape it the way you like.
The required extensions for C++ development and Cmake support are "C/C++" and "Cmake Tools". Extensions can be found in the extensions tab on the left.
Once this is done, you can set up the project:

 * Start VSCode
 * Open the project via File --> Open Folder and select the exercise's top-level directory containing the `CMakeLists.txt` file and accept by clicking `OK`
 * In the bottom bar, click on `CMake`, choose your compiler and select `Release` mode
 * In the bottom bar, click on  `[all]` and select the right target (solarsystem)
 * Still in the bottom bar, there are buttons for building (Build) and running (play symbol)

Have a look at the VSCode [documentation](https://code.visualstudio.com/docs/cpp/introvideos-cpp) for further details. 


Keyboard Settings
-----------------
  * Arrow keys left/right or left mouse: 	y-rotate Camera
  * Arrow keys up/down or shift + left mouse: 	x-rotate Camera
  * 9/0:					change camera's distance to the observed object
  * 1-8:					set camera to planets/sun
  * f:                      set camera to spaceship
  * tab:					switch mono/stereo view mode
  * r:						randomize planets' positions
  * g:						toggle greyscale
  * +/-:					increase/decrease time_step
  * space:                  pause 
  * m:						toggle wireframe
  * Escape:					exit viewer
  

Recommended order for the exercises
-----------------------------------

* **Setup of an Animated Solar System:**
  * Start by placing all celestial bodies in space in `Space_Object::update()` and `Moon::update()` (`src/space_object.cpp`). The result should look similar to this (with a white background):
    ![](screenshots/planet_system_initial.png)
  * Each time the system is rendered, `Space_Object::angle_self_` and `Space_Object::angle_parent_` (`src/space_object.h`) are increased in `Solar_viewer::timer()`. Use this to animate the solar system, i.e. make each body rotate around its own axis, around the sun, and -- in case of the moon -- around the earth. 
  * Allow a kind of navigation through the solar system and for a change of the camera's angle and eye's distance to the celestial body. Details can be found in `Solar_viewer::render()` and `Solar_viewer::keyboard(...)` (`src/solar_viewer.cpp`).
A video for comparison can be found in the video directory (`videos/rotations.mp4`). If you  did not implement lighting and shaders yet, the planets will not be as colorful as in the video.  If you look at the system from above and wait some time-steps, the result should look similar to this:
  ![](screenshots/planet_system_rotating.png)

* **Phong Lighting:**
  Implement the basic Phong shader model (`src/shader/phong.vert` and `src/shader/phong.frag`). Pass the shader variables in `Solar_viewer::draw_scene(...)`. The result should look similar to this:
  ![](screenshots/venus_phong_shading.png)

* **Spaceship:**
  Navigating the solar system in a spaceship is much more fun! A spaceship is implemented in `src/ship.cpp`. It can be rendered with your phong shader in `Solar_viewer::draw_scene(...)`. Make sure that you update the viewmatrix correctly to have the camera follow the ship while you are exploring from the ship (`Solar_viewer::render()`).  The result can be seen in `videos/spaceship.mp4` and should look similar to this:
  ![](screenshots/spaceship_moving.png)

* **Earth:**
  For the earth, replace the Phong shader with the earth shader (`src/shader/earth.vert` and `src/shader/earth.frag`) that features a special night, gloss and clouds texture. Pass the shader variables in `Solar_viewer::draw_scene(...)`. The result can be seen in the video `videos/earth.mp4` and should look similar to this:
  ![](screenshots/earth_final.png)



* **Stereo Rendering (optional):**
  Add stereo rendering to `Solar_viewer::render()` (`src/solar_viewer.cpp`). 
  
* **More optional features:**
  * Try to use the sun shader for the sun, here we use a procedurally generated and animated texture for the sun.
  * Maybe you noticed, that we just draw 6 of our 8 (poor pluto) solar systems' planets. But you can find all textures (plus pluto) in you `textures` folder. So feel free to render them all.
  * In reality, a planet's orbit is not a circle but an ellipse. You can combine scaling and rotations to achieve this effect.
  * We are always interested in your own ideas!

These are intended as fun ways for you to explore computer graphics and try out your own ideas. We recommend focusing on the non-optional exercises :)