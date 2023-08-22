<img src="https://img.shields.io/badge/C-A8B9CC?style=flat&logo=C&logoColor=white"/> <img src="https://img.shields.io/badge/C++-00599C?style=flat&logo=C++&logoColor=white"/> <img src="https://img.shields.io/badge/OpenGL-5586A4?style=flat&logo=OpenGL&logoColor=white"/>
# firefighter_simulator
This repository contains the team project of team '팀원이 세명이면 좋았을 걸' in SKKU's 2021 Introduction to Computer Graphics. The project developed a simple firefighter simulator game using OpenGL. The purpose of the game is to put out all the fires and save people in a given time.
![cg5](https://github.com/dipreez/firefighter_simulator/assets/50349104/057987a2-8383-41f3-8bd2-78020e591b68)

# Implemented features
Our game is firefighter simulator 2D. The goal of this game is to turn off all the fires and
save all rescue targets in limited time.
### Title screen, graphical help with ‘F1’ key
Our game has its own title screen and graphical help.
### In-game reset
You can reset the game and return to the title screen anytime using ‘R’ key.
### Multiple difficulty levels
After the title screen, you can select from 3 difficulty levels(easy, normal, hard). Difficulty
level will decide the number of fires, number of rescue targets, and the time limit.
### 3D shading
Walls, and water bombs that character throws are implemented in 3D. They are shaded
by the light and the light source position will be always same as the camera position.

![cg6](https://github.com/dipreez/firefighter_simulator/assets/50349104/ecea86c9-fb0e-4761-8c9f-12fda28fc938)
### Resizable window and its constant aspect-ratio viewport
Our game always allow window resizing and constant aspect-ratio viewport.
### Text rendering
Difficulty selection, and information provided on the top left corner are implemented
with text rendering.
![cg7](https://github.com/dipreez/firefighter_simulator/assets/50349104/c256ee99-a5a8-4265-88bf-eba2d03d5518)
![cg8](https://github.com/dipreez/firefighter_simulator/assets/50349104/60ef14e0-ae83-4852-8bd5-26aea1cc9564)
### Sound rendering
There is a theme song that keeps playing as soon as the game starts. Sound effects
occur when fires are extinguished, water bombs explode, or people are rescued.
### Textured 3D skysphere
You can see the skysphere in the game.
![cg9](https://github.com/dipreez/firefighter_simulator/assets/50349104/2e655ce3-7df5-4f31-8da8-5ded88b931fa)
### Dynamic 3D camera movement
The camera follows your character movement by default and you can use zoom
function with mouse right button. Also, you can activate/deactivate free-camera mode
by ‘F’ key. In free-camera mode, you will be able to use panning, rotating function of
camera just like previous assignments.
### Hand-drawn 2D images
These characters are hand-drawn.
### Sound clips created on our own
The main theme song is created on our own(bin/sounds/theme.mp3).
### Particle system
Particle systems are applied to the fires.
### Physics on gravity and acceleration
The water bomb that character throws is affected by gravitational acceleration. It is
thrown diagonally upwards and starts to fall slowly.
### Moving 2D NPC
There are NPCs(rescue targets) moving around the map. They try to avoid the fire.

# Discussions
As a result of implementation, the amount of declared global variables has become too
large and it is not quite good to see. Several global variables seem be reusable without
having to be newly defined, but there was not enough time to modify the code. There may
be a way to refactor code more effectively.

# Demo video
https://www.dropbox.com/s/kvt57c04uoo6q69/2021_CG_T1_%ED%8C%80%EC%9B%90%EC%9D%B4-%EC%84%B8%EB%AA%85%EC%9D%B4%EB%A9%B4-%EC%A2%8B%EC%95%98%EC%9D%84-%EA%B1%B8.mp4?dl=0

# How to run
You can simply execute /bin/cgtrackball.exe file to run the game. Please enjoy!
