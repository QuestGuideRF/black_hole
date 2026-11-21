# BlackHoleSim

BlackHoleSim - C++/OpenGL-проект для визуальной симуляции черной дыры. Проект демонстрирует гравитационное линзирование, трассировку лучей, аккреционный диск и искривленную сетку пространства.

## ✨ Возможности

- 2D-демонстрация черной дыры и заготовка для трассировки лучей в `2D_lensing.cpp`.
- 3D-симуляция черной дыры в `black_hole.cpp`.
- Расчет геодезических на GPU через вычислительный шейдер `geodesic.comp`.
- Отрисовка аккреционного диска и объектов вокруг черной дыры.
- Визуализация искривленной пространственной сетки через `grid.vert` и `grid.frag`.
- Управление камерой мышью и переключение гравитационного взаимодействия клавишей `G`.

## 🚀 Быстрый старт

```bash
cd black_hole
vcpkg install
vcpkg integrate install
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<ПУТЬ_К_VCPKG>/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

После сборки CMake создает две цели:

- `BlackHole2D`;
- `BlackHole3D`.

Для одноконфигурационных генераторов исполняемые файлы обычно находятся прямо в `build`. Для Visual Studio и других многоконфигурационных генераторов они находятся в подпапке конфигурации, например `build/Debug` или `build/Release`.

Примеры запуска на Windows:

```powershell
.\build\Debug\BlackHole3D.exe
.\build\Debug\BlackHole2D.exe
```

Если используется одноконфигурационный генератор:

```powershell
.\build\BlackHole3D.exe
.\build\BlackHole2D.exe
```

## 📦 Установка зависимостей

Основной способ установки зависимостей - через `vcpkg`. Список библиотек указан в `vcpkg.json`:

- `glfw3`;
- `glm`;
- `glew`.

Команды:

```bash
vcpkg install
vcpkg integrate install
```

Команда `vcpkg integrate install` выводит путь к toolchain-файлу. Этот путь нужно подставить в параметр `-DCMAKE_TOOLCHAIN_FILE`.

Альтернатива для Debian/Ubuntu из исходного README:

```bash
sudo apt update
sudo apt install build-essential cmake \
    libglew-dev libglfw3-dev libglm-dev libgl1-mesa-dev
cmake -B build -S .
cmake --build build
```

## 📁 Структура проекта

```text
black_hole/
├── 2D_lensing.cpp      # Исходник цели BlackHole2D
├── black_hole.cpp      # Исходник цели BlackHole3D
├── CPU-geodesic.cpp    # CPU-вариант расчета геодезических, не подключен к CMake-сборке
├── ray_tracing.cpp     # Отдельный пример простого трассировщика лучей, не подключен к CMake-сборке
├── geodesic.comp       # Вычислительный GLSL-шейдер для 3D-симуляции
├── grid.vert           # Вершинный шейдер сетки
├── grid.frag           # Фрагментный шейдер сетки
├── CMakeLists.txt      # Конфигурация сборки CMake
├── vcpkg.json          # Зависимости vcpkg
├── .gitignore          # Правила игнорирования временных файлов сборки
└── README.md           # Документация проекта
```

## 🛠 Использование

`BlackHole3D` открывает окно с 3D-симуляцией черной дыры.

Управление, подтвержденное кодом:

- левая кнопка мыши или средняя кнопка мыши - вращение камеры вокруг черной дыры;
- колесо мыши - приближение и отдаление камеры;
- правая кнопка мыши - включает гравитационное взаимодействие, пока кнопка удерживается;
- `G` - переключает гравитационное взаимодействие объектов.

`BlackHole2D` открывает окно 2D-демонстрации. В текущем коде начальный луч закомментирован, поэтому цель собирается, но полноценный сценарий запуска лучей в проекте не описан.

## ⚙️ Конфигурация

Переменные окружения и `.env.example` в проекте не указаны.

Важные настройки находятся в коде и сборочных файлах:

- `CMakeLists.txt` задает стандарт C++17 и цели `BlackHole2D`, `BlackHole3D`;
- `vcpkg.json` перечисляет зависимости `glfw3`, `glm`, `glew`;
- `black_hole.cpp` запрашивает OpenGL-контекст версии 4.3 для работы вычислительного шейдера;
- `geodesic.comp`, `grid.vert` и `grid.frag` должны быть доступны рядом с исполняемым файлом `BlackHole3D`; CMake копирует шейдеры в каталог сборки после сборки этой цели.

## 🧰 Технологии

- C++17;
- CMake 3.21+;
- OpenGL;
- GLEW;
- GLFW;
- GLM;
- GLSL-шейдеры;
- vcpkg.

Подпись относится к текущей версии проекта.

---

Автор: legenda  
Telegram: [@LEGENDA_SD](https://t.me/LEGENDA_SD)
