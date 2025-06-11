#include "game.h"
#include <algorithm> // Para usar std::remove_if

// Variáveis globais
Texture2D background;
Music backgroundMusic;
Sound shootSound;
Sound hitSound;
Sound playerHitSound;
bool assetsLoaded = false;  // Evita recarregar os mesmos assets mais de uma vez

const char* GAME_TITLE = "Jornada IFSP:  Atire nos Obstáculos";

// ======================= CLASSES =======================

// Construtor base para qualquer objeto do jogo
GameObject::GameObject(Rectangle r, Color c, int s) : rect(r), color(c), speed(s), active(true) {}

// ------------- Projetil (tiro) -------------
Projectile::Projectile(Rectangle r, Color c, int s) : GameObject(r, c, s) {}
void Projectile::Update() {
    if (active) {
        rect.x += speed;
        if (rect.x > SCREEN_WIDTH) active = false;
    }
}
void Projectile::Draw() const {
    if (active) DrawRectangleRec(rect, color);
}

// ------------- Obstáculo (inimigo) -------------
Obstacle::Obstacle(Rectangle r, Color c, int s) : GameObject(r, c, s) {}
void Obstacle::LoadTextureFromFile(const char* filename) {
    texture = LoadTexture(filename);
    SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);  // suaviza pixelização
}
void Obstacle::UnloadTextureData() {
    UnloadTexture(texture);
}
void Obstacle::Update() {
    if (active) {
        rect.x -= speed;
        if (rect.x + rect.width < 0) active = false;
    }
}
void Obstacle::Draw() const {
    if (active) {
        DrawTexturePro(texture, { 0, 0, (float)texture.width, (float)texture.height }, rect, { 0, 0 }, 0.0f, WHITE);
    }
}

// ------------- Jogador -------------
Player::Player(Rectangle r, Color c, int s, int initialHp) : GameObject(r, c, s), hp(initialHp) {}
void Player::LoadTextureFromFile(const char* filename) {
    texture = LoadTexture(filename);
    SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
    rect.width = texture.width;
    rect.height = texture.height;
}
void Player::UnloadTextureData() {
    UnloadTexture(texture);
}
void Player::Update() {
    if (rect.x < 0) rect.x = 0;
    if (rect.x + rect.width > SCREEN_WIDTH) rect.x = SCREEN_WIDTH - rect.width;
    if (rect.y < 0) rect.y = 0;
    if (rect.y + rect.height > SCREEN_HEIGHT) rect.y = SCREEN_HEIGHT - rect.height;
}
void Player::Draw() const {
    if (active) {
        DrawTexturePro(texture, { 0, 0, (float)texture.width, (float)texture.height }, rect, { 0, 0 }, 0.0f, WHITE);
    }
}
void Player::TakeDamage(int amount) {
    hp -= amount;
    if (hp <= 0) active = false;
}

// ======================= LÓGICA DO JOGO =======================

// Objetos principais
Player player({ SCREEN_WIDTH / 4.0f, SCREEN_HEIGHT / 2.0f - 25, 64, 64 }, WHITE, 5, PLAYER_INITIAL_HP);
std::vector<Projectile> projectiles;
std::vector<Obstacle> obstacles;
double lastObstacleSpawnTime = 0.0;

// Inicializa o jogo
void InitGame() {
    if (!assetsLoaded) {
        InitAudioDevice();
        backgroundMusic = LoadMusicStream("assets/music/bg_music.mp3");
        shootSound = LoadSound("assets/sfx/shoot.mp3");
        hitSound = LoadSound("assets/sfx/hit.mp3");
        playerHitSound = LoadSound("assets/sfx/player_hit.mp3");
        SetSoundVolume(hitSound, 5.0f);
        SetSoundVolume(playerHitSound, 1.0f);
        assetsLoaded = true;
    }

    PlayMusicStream(backgroundMusic);
    background = LoadTexture("assets/backgrounds/background.png");

    float width = 64;
    float height = width * (1536.0f / 1024.0f);
    float px = 200;
    float py = SCREEN_HEIGHT / 2.0f - height / 2;

    player = Player({ px, py, width, height }, WHITE, 5, PLAYER_INITIAL_HP);
    player.LoadTextureFromFile("assets/sprites/player.png");
    player.active = true;

    projectiles.clear();
    obstacles.clear();
    lastObstacleSpawnTime = GetTime();
}

// Atira um projétil na frente do jogador
void ShootProjectile() {
    projectiles.emplace_back(
        Rectangle{ player.rect.x + player.rect.width, player.rect.y + player.rect.height / 2 - 5, 20, 10 },
        YELLOW,
        10
    );
    PlaySound(shootSound);
}

// Lista de imagens para os obstáculos
const char* obstacleSprites[] = {
    "assets/sprites/obstacles/enemy1.png",
    "assets/sprites/obstacles/enemy2.png",
    "assets/sprites/obstacles/enemy3.png",
    "assets/sprites/obstacles/enemy4.png",
    "assets/sprites/obstacles/enemy5.png"
};
const int numObstacleSprites = sizeof(obstacleSprites) / sizeof(obstacleSprites[0]);

// Gera inimigo com imagem e posição aleatória
void GenerateObstacle() {
    Obstacle obs({ SCREEN_WIDTH, 0, 0, 0 }, WHITE, 5);
    int index = GetRandomValue(0, numObstacleSprites - 1);
    obs.LoadTextureFromFile(obstacleSprites[index]);
    obs.rect.width = (float)obs.texture.width;
    obs.rect.height = (float)obs.texture.height;
    obs.rect.y = (float)GetRandomValue(50, SCREEN_HEIGHT - (int)obs.rect.height - 50);
    obs.speed = GetRandomValue(3, 10);
    obstacles.push_back(obs);
}

// Atualiza tudo no jogo: player, tiros, inimigos, colisões
void UpdateGame() {
    UpdateMusicStream(backgroundMusic);
    player.Update();
    for (auto& p : projectiles) p.Update();
    for (auto& o : obstacles) o.Update();

    if (GetTime() - lastObstacleSpawnTime >= OBSTACLE_SPAWN_RATE) {
        GenerateObstacle();
        lastObstacleSpawnTime = GetTime();
    }

    // Verifica colisão entre tiros e obstáculos
    for (auto& p : projectiles) {
        if (p.active) {
            for (auto& o : obstacles) {
                if (o.active && CheckCollisionRecs(p.rect, o.rect)) {
                    p.active = false;
                    o.active = false;
                    PlaySound(hitSound);
                }
            }
        }
    }

    // Verifica colisão entre jogador e inimigos
    for (auto& o : obstacles) {
        if (o.active && CheckCollisionRecs(player.rect, o.rect)) {
            player.TakeDamage(1);
            o.active = false;
            PlaySound(playerHitSound);
        }
    }

    // Remove objetos inativos da memória
    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& p) { return !p.active; }), projectiles.end());
    obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(), [](const Obstacle& o) { return !o.active; }), obstacles.end());
}

// Desenha todos os elementos na tela
void DrawGame() {
    DrawTexturePro(background, { 0, 0, (float)background.width, (float)background.height }, { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT }, { 0, 0 }, 0.0f, WHITE);
    player.Draw();
    for (const auto& p : projectiles) p.Draw();
    for (const auto& o : obstacles) o.Draw();
    DrawText(TextFormat("VIDA: %d", player.hp), 10, 10, 25, WHITE);
    EndDrawing();
}
