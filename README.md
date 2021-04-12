Group 09
Game title: AIphaGo Away

TODO: set_up script:
under project dir run

windows with vs:

git clone https://github.com/microsoft/vcpkg

.\vcpkg\bootstrap-vcpkg.bat

.\vcpkg\vcpkg install entt
.\vcpkg\vcpkg install gl3w
.\vcpkg\vcpkg install imgui
.\vcpkg\vcpkg install nlohmann-json


.\vcpkg\vcpkg integrate install

Open the CMake Settings Editor, and under CMake toolchain file, add the path to the vcpkg toolchain file:

-DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake

mac/linux with clion:

git clone https://github.com/microsoft/vcpkg

./vcpkg/bootstrap-vcpkg.sh

./vcpkg/vcpkg install entt
./vcpkg/vcpkg install gl3w
./vcpkg/vcpkg install imgui
./vcpkg/vcpkg install nlohmann-json

Open the Toolchains settings (File > Settings on Windows and Linux, CLion > Preferences on macOS), and go to the CMake settings (Build, Execution, Deployment > CMake). Finally, in CMake options, add the following line:

-DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake

jetbrains educational license

https://www.jetbrains.com/community/education/#students

Credit:

- package manager

https://github.com/microsoft/vcpkg

- ECS:

https://github.com/skypjack/entt/wiki/Crash-Course:-entity-component-system

- GUI:

https://github.com/ocornut/imgui

- Utils:

https://github.com/nlohmann/json

- Games:

https://github.com/guillaume-haerinck/imac-tower-defense

https://github.com/Kerndog73/EnTT-Pacman

- Mesh generator:
  
https://www.taketechease.com/

- Free texture:

https://www.kenney.nl/assets/toon-characters-1

https://itch.io/game-assets/free/tag-explosions

- Shaders:

https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

https://www.youtube.com/watch?v=RP1MVD4hAJM&list=WL&index=7&t=22s

https://www.shadertoy.com/view/lsS3zc






