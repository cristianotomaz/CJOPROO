#include "game.h"

int main() {
    // Cria a janela principal do jogo com título e resolução definida em game.h
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
    SetTargetFPS(60);  // Define que o jogo vai rodar a 60 quadros por segundo

    InitGame();  // Inicializa o estado do jogo (player, inimigos, etc.)

    // Loop principal do jogo - roda até o jogador fechar a janela
    while (!WindowShouldClose()) {

        // Se o jogador morreu (vida zerada)
        if (!player.active) {
            if (IsKeyPressed(KEY_R)) {
                InitGame();  // Reinicializa o jogo ao apertar R
                continue;    // Volta para o início do loop com jogo reiniciado
            }

            // Desenha a tela de Game Over
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("GAME OVER!", SCREEN_WIDTH / 2 - MeasureText("GAME OVER!", 60) / 2, SCREEN_HEIGHT / 2 - 30, 60, RED);
            DrawText("Pressione R para Reiniciar ou ESC para Sair", SCREEN_WIDTH / 2 - MeasureText("Pressione R para Reiniciar ou ESC para Sair", 20) / 2, SCREEN_HEIGHT / 2 + 50, 20, WHITE);
            EndDrawing();
            continue;  // Pula o restante do loop
        }

        // Controles do jogador (setas movimentam, espaço atira)
        if (IsKeyDown(KEY_RIGHT)) player.rect.x += player.speed;
        if (IsKeyDown(KEY_LEFT))  player.rect.x -= player.speed;
        if (IsKeyDown(KEY_UP))    player.rect.y -= player.speed;
        if (IsKeyDown(KEY_DOWN))  player.rect.y += player.speed;
        if (IsKeyPressed(KEY_SPACE)) ShootProjectile();

        // Atualiza lógica do jogo e desenha os elementos na tela
        UpdateGame();
        DrawGame();
    }

    // Libera memória usada por texturas e sons
    for (auto& o : obstacles) {
        o.UnloadTextureData();
    }
    player.UnloadTextureData();
    UnloadTexture(background);
    UnloadMusicStream(backgroundMusic);
    UnloadSound(playerHitSound);
    UnloadSound(shootSound);
    UnloadSound(hitSound);
    CloseAudioDevice();  // Encerra sistema de áudio
    CloseWindow();       // Fecha a janela

    return 0;
}
