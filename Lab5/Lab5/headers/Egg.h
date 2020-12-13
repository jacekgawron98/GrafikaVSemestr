// Copyright 2020 Jacek Gawron
#pragma once
#define EGG_DOT 1
#define EGG_WIRE 2
#define EGG_TRIANGLE 3
typedef float vector3[3];
typedef float vector2[2];

namespace eggType {
    enum eggType {
        triangle,
        wire,
        dot
    };
}

class Egg {
 private:
    int size;
    float height;
    vector3** points;
    vector3** normals;
    vector2** texturePoints;

    eggType::eggType currentEggType;

    void eggDrawTriangles();
    void eggDrawLines();
    void eggDrawDoted();
    void generateEgg();
    void generateNormals();
 public:
    Egg(int size, int type);
    void drawEgg();
    void changeEggType(int type);
};
