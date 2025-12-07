#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace GameConstants {
// Dimensiones de la escena
constexpr int SCENE_WIDTH = 500;
constexpr int SCENE_HEIGHT = 600;
constexpr int SCENE_X = 10;
constexpr int SCENE_Y = 10;

// Velocidades
constexpr int BACKGROUND_SPEED = 2;
constexpr int NAVE_SPEED = 13;
constexpr int METEORITO_SPEED = 10;

// Timers (en milisegundos)
constexpr int FRAME_UPDATE_MS = 16;         // ~60 FPS update interval
constexpr int METEORITO_SPAWN_MS = 2000;    // Cada 2 segundos
constexpr int METEORITO_MOVE_MS = 50;       // Actualizar posicion cada 50ms
constexpr int ACCELERATION_MS = 7000;       // Acelerar spawn cada 7 segundos
constexpr int GOAL_TIME_MS = 30000;         // Meta a los 30 segundos
constexpr int TIEMPO_INTRO_NIVEL = 3000;
constexpr int TIEMPO_PANTALLA_VICTORIA = 3000;

// --- DIFFICULTY/SPAWN LIMITS ---
constexpr int METEORITO_SPAWN_MIN = 900;    // Intervalo minimo
constexpr int METEORITO_SPAWN_REDUCTION = 250; // Cantidad para reducir el intervalo de spawn en cada aceleracion (50ms)

// Tamaños de sprites
constexpr int NAVE_SIZE = 100;
constexpr int METEORITO_SIZE = 80;
constexpr int GEMA_SIZE = 50;

// Posición inicial de la nave
constexpr int NAVE_INITIAL_X = 260;
constexpr int NAVE_INITIAL_Y = 506;

// Vidas iniciales
constexpr int VIDAS_INICIALES = 3;

// Rutas de recursos

namespace Resources {
constexpr const char* BACKGROUND = ":/new/imagen/images/Space.png";
constexpr const char* NAVE = ":/new/imagen/images/voyager.png";
constexpr const char* METEORITO = ":/new/imagen/images/spriteMe.png";
constexpr const char* LUNA = ":/new/imagen/images/luna.png";
constexpr const char* CORAZON = ":/new/imagen/images/corazon.png";
constexpr const char* MENU_BACKGROUND = ":/new/imagen/images/portadaVoyager.png";
constexpr const char* GEMA = ":/new/imagen/images/gema.png";
constexpr const char* NIVEL1_INTRO = ":/new/imagen/images/nivel1.png";
constexpr const char* NIVEL2_INTRO = ":/new/imagen/images/nivel2.png";
constexpr const char* GAME_OVER = ":/new/imagen/images/gameover.png";
constexpr const char* NIVEL3_FONDO = ":/new/imagen/images/fo.png";
constexpr const char* GALAXIA = ":/new/imagen/images/galaxia.png";

//SONIDOS

constexpr const char* MUSICA_MENU = ":/new/audios/sounds/menu.mp3";
constexpr const char* MUSICA_NIVEL1 = ":/new/audios/sounds/nivel1.mp3";
constexpr const char* MUSICA_NIVEL2 = ":/new/audios/sounds/nivel2.mp3";

constexpr const char* SFX_COLISION = ":/new/audios/sounds/colision.wav";
constexpr const char* SFX_GEMA = ":/new/audios/sounds/gema.wav";
constexpr const char* SFX_VICTORIA = ":/new/audios/sounds/victoria.wav";
constexpr const char* SFX_GAMEOVER = ":/new/audios/sounds/gameover.wav";
}

// Nivel 2 - Constantes
constexpr int GEMAS_PARA_GANAR_NIVEL2 = 5;     // Gemas necesarias
constexpr int GEMA_SPAWN_MS = 3000;             //  Cada 3 segundos
constexpr int GEMA_PUNTOS = 10;                 // Puntos por gema


// Tamaño del HUD
constexpr int CORAZON_SIZE = 20;            // Tamano de cada corazon
constexpr int CORAZON_SPACING = 5;         // Espacio entre corazones
constexpr int HUD_MARGIN_X = 20;            // Margen izquierdo
constexpr int HUD_MARGIN_Y = 20;
}

#endif // CONSTANTS_H
