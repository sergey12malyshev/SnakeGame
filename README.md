# Pac-ManGame 
Pac-Man game project on STM32 controller and ILI9341 display

Проект игры Pac-Man на контроллере STM32 и дисплее ILI9341

*Обязательно проверять состояние вывода BOOT0 на отладочной плате!*

 <img src="https://github.com/sergey12malyshev/Pac-ManGame/blob/develop/image/db08b1eb-7112-4140-b000-8f4098d6f0e6.jpg" width=26% height=26%>   <img src="https://github.com/sergey12malyshev/Pac-ManGame/blob/develop/image/b94825aa-9aec-4773-a235-eaeea46141b8.jpg" width=20% height=20%>

 <img src="https://github.com/sergey12malyshev/Pac-ManGame/blob/develop/image/1688404908245.jpg" width=26% height=26%> <img src="https://github.com/sergey12malyshev/Pac-ManGame/blob/develop/image/1688404908269.jpg" width=24% height=24%>
 
## Tools

### Compiler
*GCC* (gcc-arm-none-eabi) version 5.4.1 20160919

### Setting up the build system in WINDOWS10 

1. Скачать GNU Arm Embedded Toolchain 
https://developer.arm.com/downloads/-/gnu-rm#:~:text=The%20GNU%20Arm%20Embedded%20Toolchain,Arm%20Cortex%2DR%20processor%20families
or
https://launchpad.net/gcc-arm-embedded/+download

2. Скачать Windows Build Tools binaries (Make, cp, rm, echo, sh...)
https://github.com/xpack-dev-tools/windows-build-tools-xpack/releases/

3. Прописать пути в PATH к утилитам

Подробная статья: https://habr.com/ru/articles/673522/

### RTOS
Prototreads AD v1.4

### Code editor
*VS Code* файл Pac-ManGame.code-workspace

### Static code analyzer
Cppcheck 2.10 https://cppcheck.sourceforge.io/

Запустить RunStaticAnalysisCODE.cmd

Возможен вывод результата анализа в файл

## Build (Сборка) 
Для сборки **makeProject.bat**

Для очистки **makeClean.bat**

## Programm (Прошивка ПО)
Запустить **programFlash.bat**

## Peripheral setup
STM32 CubeMX
(Установленны галочки *Enable Full Assert* и *Set All Free Pins as Analog*)

## Command Line Interface

UART 115200 Baud rate

Connect: PA10 - RX / PA9 - TX

Enter *help* command

## Hardware
- МК - STM32F103C8T6 (ARM Cortex-M3; 72 МГц; FLASH 64 кБ; ОЗУ 20 кБ)
- Дисплей - 2.8 TFT SPI 240x320 display (ILI9341)
- Кнопки - К1116КП2 (на основе датчиков Холла)
- Зарядка - TP4056 charge module with protection
- 
## Case
К проекту добавлены модели корпуса

## gcc-arm-none-eabi

Магическое название компилятора gcc-arm-none-eabi означает следующее:

gcc - название компилятора;

arm - архитектура процессора;

none - компилятор не вносит никакого дополнительного bootstrap кода от себя;

eabi - код соответствует спецификации EABI.

https://tuxotronic.org/post/arm-none-eabi/
