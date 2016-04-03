cmake_minimum_required(VERSION 3.2)
project(CSC460-Roomba)

set(ENV{PATH} "c:\\anaconda2\\Library\\bin;C:\\Program Files\\Git\\cygdrive\\c\\Users\\denis\\.local\\bin;C:\\Program Files\\Git\\mingw64\\bin;C:\\Program Files\\Git\\usr\\local\\bin;C:\\Program Files\\Git\\usr\\bin;C:\\Program Files\\Git\\usr\\bin;C:\\Program Files\\Git\\mingw64\\bin;C:\\Program Files\\Git\\usr\\bin;C:\\Program Files\\Git\\cygdrive\\c\\Users\\denis\\.local\\bin;C:\\ProgramData\\Oracle\\Java\\javapath;C:\\Program Files\\Dell\\DW WLAN Card;C:\\Program Files (x86)\\Intel\\iCLS Client;C:\\Program Files\\Intel\\iCLS Client;C:\\Windows\\system32;C:\\Windows;C:\\Windows\\System32\\Wbem;C:\\Windows\\System32\\WindowsPowerShell\\v1.0;C:\\Program Files (x86)\\Intel\\Intel(R) Management Engine Components\\DAL;C:\\Program Files\\Intel\\Intel(R) Management Engine Components\\DAL;C:\\Program Files (x86)\\Intel\\Intel(R) Management Engine Components\\IPT;C:\\Program Files\\Intel\\Intel(R) Management Engine Components\\IPT;C:\\Program Files\\WIDCOMM\\Bluetooth Software;C:\\Program Files\\WIDCOMM\\Bluetooth Software\\syswow64;C:\\Program Files (x86)\\NVIDIA Corporation\\PhysX\\Common;C:\\WINDOWS\\system32;C:\\WINDOWS;C:\\WINDOWS\\System32\\Wbem;C:\\WINDOWS\\System32\\WindowsPowerShell\\v1.0;C:\\xampp\\php;C:\\ProgramData\\ComposerSetup\\bin;C:\\instantclient_12_1;C:\\Program Files (x86)\\QuickTime\\QTSystem;C:\\Anaconda2;C:\\Anaconda2\\Scripts;C:\\Anaconda2\\Library\\bin;C:\\Program Files\\Git\\cmd;C:\\Program Files\\Git\\mingw64\\bin;C:\\HashiCorp\\Vagrant\\bin;C:\\Program Files (x86)\\Heroku\\bin;C:\\Program Files (x86)\\git\\cmd;C:\\Program Files (x86)\\Microsoft Emulator Manager\\1.0;C:\\Users\\denis\\.babun;C:\\Users\\denis\\AppData\\Local\\Android\\sdk\\platform-tools;C:\\Program Files\\Git\\usr\\bin\\vendor_perl;C:\\Program Files\\Git\\usr\\bin\\core_perl")
set(PLATFORMIO_CMD "C:\\Anaconda2\\Scripts\\platformio.exe")

include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/variants/mega")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/cores/arduino")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/Programming/CSC460-Roomba/lib/OS")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/Programming/CSC460-Roomba/lib/uart")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/Audio/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/Bridge/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/EEPROM")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/Esplora/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/Ethernet/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/Firmata")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/GSM/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/HID")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/Keyboard/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/LiquidCrystal/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/Mouse/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/RobotIRremote/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/Robot_Control/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/Robot_Motor/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/SD/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/SPI")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/Scheduler/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/Servo/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/SoftwareSerial")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/SpacebrewYun/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/Stepper/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/TFT/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/Temboo/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/USBHost/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/WiFi/src")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/framework-arduinoavr/libraries/Wire")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/toolchain-atmelavr/avr/include")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/toolchain-atmelavr/lib/gcc/avr/4.8.1/include")
include_directories("$ENV{HOMEDRIVE}$ENV{HOMEPATH}/.platformio/packages/toolchain-atmelavr/lib/gcc/avr/4.8.1/include-fixed")

add_definitions(-DF_CPU=16000000L)
add_definitions(-DARDUINO_ARCH_AVR)
add_definitions(-DARDUINO_AVR_MEGA2560)
add_definitions(-DARDUINO=10607)
add_definitions(-DPLATFORMIO=020804)
add_definitions(-D__AVR_ATmega2560__)

add_custom_target(
    PLATFORMIO_BUILD ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_UPLOAD ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run --target upload
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_CLEAN ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run --target clean
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_PROGRAM ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run --target program
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_UPLOADFS ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run --target uploadfs
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_UPDATE_ALL ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion update
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(CSC460-Roomba
        src/main.c
        src/utils.cpp src/utils.h)
