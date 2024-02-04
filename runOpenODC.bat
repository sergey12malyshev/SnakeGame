rem https://github.com/ilg-archived/openocd/releases/tag/gae-0.10.0-20170124
rem https://github.com/crtv-club/crtv_club/blob/master/docs/source/nrf52_stlink_flashing.rst
rem https://eax.me/openocd/
rem run terminal: telnet localhost 4444

cd C:\Program Files\GNU ARM Eclipse\OpenOCD\0.10.0-201701241841

bin\openocd  -s share\openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg

pause