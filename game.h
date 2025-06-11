#pragma once
#include "raylib.h"
#include <vector>

// Constantes gerais
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;
extern const char* GAME_TITLE;

const int PLAYER_INITIAL_HP = 3;
const double OBSTACLE_SPAWN_RATE = 1.5;

// Variáveis globais de assets
extern Music backgroundMusic;
extern Sound shootSound;
extern Sound hitSound;
extern Sound playerHitSound;
extern Texture2D background;

// ======================= CLASSES =======================

// Classe base para todos objetos do jogo
class GameObject {
public:
    Rectangle rect;  // posição e tamanho
    Color color;
    int speed;
    bool active;

    GameObject(Rectangle r, Color c, int s);
    virtual void Update() = 0;
    virtual void Draw() const = 0;
};

// Projétil disparado pelo jogador
class Projectile : public GameObject {
public:
    Projectile(Rectangle r, Color c, int s);
    void Update() override;
    void Draw() const override;
};

// Obstáculo com textura
class Obstacle : public GameObject {
public:
    Texture2D texture;

    Obstacle(Rectangle r, Color c, int s);
    void Update() override;
    void Draw() const override;

    void LoadTextureFromFile(const char* filename);
    void UnloadTextureData();
};

// Jogador
class Player : public GameObject {
public:
    int hp;
    Texture2D texture;

    Player(Rectangle r, Color c, int s, int initialHp);
    void Update() override;
    void Draw() const override;
    void TakeDamage(int amount);
    void LoadTextureFromFile(const char* filename);
    void UnloadTextureData();
};

// Funções principais
void InitGame();
void UpdateGame();
void DrawGame();
void ShootProjectile();

// Objetos globais
extern Player player;
extern std::vector<Projectile> projectiles;
extern std::vector<Obstacle> obstacles;
extern double lastObstacleSpawnTime;
