Group 09
Game title: AIphaGo Away

TODO: set_up script:
under project dir run

windows with vs:

git clone https://github.com/microsoft/vcpkg

.\vcpkg\bootstrap-vcpkg.bat

.\vcpkg\vcpkg install entt

.\vcpkg\vcpkg integrate install

Open the CMake Settings Editor, and under CMake toolchain file, add the path to the vcpkg toolchain file:

-DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake

mac/linux with clion:

git clone https://github.com/microsoft/vcpkg

./vcpkg/bootstrap-vcpkg.sh

./vcpkg/vcpkg install entt

Open the Toolchains settings (File > Settings on Windows and Linux, CLion > Preferences on macOS), and go to the CMake settings (Build, Execution, Deployment > CMake). Finally, in CMake options, add the following line:

-DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake

jetbrains educational license

https://www.jetbrains.com/community/education/#students

Credit:

https://github.com/microsoft/vcpkg

https://github.com/skypjack/entt/wiki/Crash-Course:-entity-component-system

https://github.com/guillaume-haerinck/imac-tower-defense

https://github.com/carlfindahl/wacman



