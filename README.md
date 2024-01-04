### [Отчет GitHub](https://github.com/tplaymeow/itmo-low-level-programming-lab2/blob/main/Report.pdf)
### [Отчет GitLab](https://gitlab.se.ifmo.ru/tplaymeow/low-level-programming-lab2/-/blob/main/Report.pdf)

## Сборка и запуск
```
git clone git@github.com:tplaymeow/itmo-low-level-programming-lab1.git
git submodule init
git submodule update
```

Для сборки и запуска необходимо использовать Сmake.

Тестовая программа
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
build/itmo-low-level-programming-lab2
```
