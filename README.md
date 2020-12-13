### Сборка на Linux
    mkdir build
    cd build
    cmake ..
    make
   Возможно потребуется установить libxxf86vm-dev

    sudo apt-get install libxxf86vm-dev

### Запуск
    cd build/bin/cube_scene
    ./cube_scene
### Сборка на Windows
    mkdir build
    cd build
    cmake ..
    cmake --build .
### Запуск
    cd bin/cube_scene/Debug/cube_scene.exe
    cube_scene.exe
### Clion

    Get from VCS
    launch

### Visual Studio

    Clone a repository
    CmakeList.txt - правой кнопкой мыши: build
    вверху Select Startup Item: cube_scene.exe

### Управление
W, A, S, D, Q, E - вперёд, влево, назад, вправо, вниз, вверх  
Колёсико мыши - приближение  
F - меняет сцену  
Z/C - настройка выпуклости диванной стены  

### Реализованные эффекты
1. Отбрасывание тени на плоскость
2. Процедурные текстуры - при нажатии F появляются 3 тора
3. Кубические текстуры в режиме окружающей среды
4. Отбрасывание тени на другие объекты
5. Попиксельный расчёт освещенности по стандартным моделям
6. Нестандартная модель освещения - летающие кольца освещены по модели Кука-Торренса
7. Нечёткие тени
8. Parallax relief mapping
