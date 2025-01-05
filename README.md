# Physics_Simulation
This is a physics simulator project (C++, Qt, Vulcan)


# Project setup

## On linux, make the scripts executable:
chmod +x scripts/build_release.sh scripts/run.sh scripts/test.sh


## QT setup
Setup Qt environment variables: 
CMAKE_PREFIX_PATH to (for exemple): C:/Qt/6.8.1/msvc2022_64
QT_QPA_PLATFORM_PLUGIN_PATH to (for example): C:\Qt\6.8.1\msvc2022_64\plugins\platforms
Restart IDE

## cmake
DeployQt.cmake is used to deploy the Qt libraries.