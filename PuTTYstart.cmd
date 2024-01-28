@echo off
goto start
--------------------------------------
Скрипт для запуска telnet или serial терминала PuTTY
Для использования скрипта необходимо прописать пути нахождения утилиты PuTTY, создать и сохранить сессию со следующими параметрами:
(Connection: Telnet, port: 19021, host: localhost), Local echo: *Force off*. Local line editing: *Force off*.
https://documentation.help/PuTTY/using-cmdline-session.html
--------------------------------------
:start

rem запуск PuTTY без сохранённой сессии:
rem start "putty" "C:\Program Files\PuTTY\putty.exe" -telnet -P 19021  localhost

rem запуск сохранённой сессии PuTTY:
start "putty" "C:\Program Files\PuTTY\putty.exe" -load "GameBox"