@echo off
chcp 65001 > nul
title Повторный запуск тестов CalcGTest

:loop
cls
echo ==== Сборка проекта ====
cmake --build .

echo ==== Запуск тестов ====
cd Debug
calculate_test.exe

cd ..
echo.
echo ==== Тесты завершены ====
echo Нажмите ENTER, чтобы запустить снова, или CTRL+C для выхода...
pause > nul
goto loop
