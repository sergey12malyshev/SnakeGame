# Pac-ManGame 
Pac-Man game console project on STM32 controller and ILI9341 display

<img src="https://github.com/sergey12malyshev/Pac-ManGame/blob/develop/image/Screen2.jpg" width=29% height=29%> <img src="https://github.com/sergey12malyshev/Pac-ManGame/blob/develop/image/Top2.jpeg" width=25% height=25%> <img src="https://github.com/sergey12malyshev/Pac-ManGame/blob/develop/image/top3.jpeg" width=24% height=24%>
 
## Tools

### Compiler
*GCC* (gcc-arm-none-eabi) version 5.4.1 20160919

### Setting up the build system in WINDOWS10 

1. Download GNU Arm Embedded Toolchain 
https://developer.arm.com/downloads/-/gnu-rm#:~:text=The%20GNU%20Arm%20Embedded%20Toolchain,Arm%20Cortex%2DR%20processor%20families
or
https://launchpad.net/gcc-arm-embedded/+download

2. Download Windows Build Tools binaries (Make, cp, rm, echo, sh...)
https://github.com/xpack-dev-tools/windows-build-tools-xpack/releases/

3. Add paths to the PATH environment variable in Windows

Подробная статья: https://habr.com/ru/articles/673522/

### RTOS
Prototreads AD v1.4

https://dunkels.com/adam/pt/

### Code editor
*VS Code* file Pac-ManGame.code-workspace

## Build 
To make the release project , run **makeProject.bat** (OR *make -j* cmd command)

To make the debug project , run **makeProjectDebug.bat** (OR *make -j1 debug* cmd command)

To clean the project, run **makeClean.bat** (OR *make clean*)

## Programm firmware
Connect ST-Link V2 to SWD connector. Run **programFlash.bat**

## Update firmware
Use system bootloader: MENU->UPDATE or CLI-> LOAD command

### Static code analyzer
Cppcheck 2.10 https://cppcheck.sourceforge.io/

Run **RunStaticAnalysisCODE.cmd**

It is possible to output the analysis result to a file

### Unit tests
Simple unit tests are implemented in the file "unit_test.с". Unit tests are launched if the *RUN_UNIT_TEST* key is set or when building Debug

## Peripheral setup
STM32 CubeMX
(Checkboxes checked: *Enable Full Assert* и *Set All Free Pins as Analog*)

## Command Line Interface

UART 115200 Baud rate

Connect: PA10 - RX / PA9 - TX

Terminal configuration file: utils\TERATERM.INI

Enter *help* command

<img src="https://github.com/sergey12malyshev/Pac-ManGame/blob/develop/image/ShellScreen.jpg" width=20% height=20%>

## Hardware
- Black pill STM32F103C8T6 (ARM Cortex-M3; 72 МГц; FLASH 64 кБ; ОЗУ 20 кБ)
- 2.8 TFT SPI 240x320 display (ILI9341)
- Buttons К1116КП2 (Hall effect sensor) or classic buttons
- TP4056 charge module with protection
- Battery 18650 li-ion
- Active buzzer
- Printed circuit board for soldering double-sided 7*9cm
- P-MOSFET
- USB-UART module (Optional for updating firmware via USB Type-C and working with CLI)
  
<img src="https://github.com/sergey12malyshev/Pac-ManGame/blob/develop/image/Shematic.png" width=30% height=30%>  <img src="https://github.com/sergey12malyshev/Pac-ManGame/blob/develop/image/Hard2.jpg" width=24% height=24%> <img src="https://github.com/sergey12malyshev/Pac-ManGame/blob/develop/image/1688404908269.jpg" width=24% height=24%>

*See Altium directory

*Be sure to check the BOOT0 output status on the debug board!*

## Case
Case models have been added to the project in the format **.stl**

<img src="https://github.com/sergey12malyshev/Pac-ManGame/blob/develop/Case%20model/Case.jpg" width=15% height=15%>

## gcc-arm-none-eabi

The magic name of the compiler gcc-arm-none-eabi means the following:

gcc - compiler name;

arm - processor architecture;

none - the compiler does not contribute any additional bootstrap code from itself;

eabi - code conforms to the EABI specification.

https://tuxotronic.org/post/arm-none-eabi/


## About repo

Work in the repository is carried out through the Git-flow branching model
