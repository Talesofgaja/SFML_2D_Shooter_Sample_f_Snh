g++ GameObject.cpp Entity.cpp Projectile.cpp StaticObject.cpp Game.cpp main.cpp -o game.exe -I".\SFML\include" -L".\SFML\lib" -lsfl-graphics-s -lsfml-system-s -lopeng132 -lwinm -lgdi32 -DSFML_STATIC -std=c++17
.\game.exe

for linux sys such as github
g++ GameObject.cpp Entity.cpp Projectile.cpp StaticObject.cpp Game.cpp main.cpp -o game -I"./SFML/include" -L"./SFML/lib" -lsfml-graphics -lsfml-window -lsfml-system -std=c++17 -DSFML_STATIC
./game
