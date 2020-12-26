![Alt Text](Gifs/shadows.gif)
![Alt Text](Gifs/lights.gif)
![Alt Text](Gifs/torus.gif)

### Build on Linux
    mkdir build
    cd build
    cmake ..
    make
    
   There is a chance you need to install libxxf86vm-dev  

    sudo apt-get install libxxf86vm-dev

### Launch
    cd build/bin/cube_scene
    ./cube_scene
### Build on Windows
    mkdir build
    cd build
    cmake ..
    cmake --build .
### Launch
    cd bin/cube_scene/Debug/cube_scene.exe
    cube_scene.exe
### Clion

    Get from VCS
    launch

### Visual Studio

    Clone a repository
    CmakeList.txt - mouse right click: build
    In the upper menu: Select Startup Item: cube_scene.exe

### Controls
W, A, S, D, Q, E - forward, left, back, right, down, up  
Mouse scroll - zoom  
F - change scene
Z/C - configure wall bump

### Effects
1. Casting a shadow onto a plane
2. Procedural textures - pressing F produces 3 toruses
3. Cubic textures in a skybox mode
4. Casting shadows on other objects
5. Per-pixel calculation of illumination using standard models
6. Non-standard lighting model - flying rings are lit according to the Cook-Torrance model
7. Fuzzy shadows
8. Parallax relief mapping
