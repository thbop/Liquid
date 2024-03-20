#include "raylib.h"
#include "raymath.h"
#include "math.h"

#define RIGHT Vector2(0,  1)
#define LEFT  Vector2(0, -1)
#define UP    Vector2(-1, 0)
#define DOWN  Vector2( 1, 0)

#define POINTBUFFERSIZE 100

struct Point {
    Vector2 pos, vel;
} typedef Point;


Point PointBuffer[POINTBUFFERSIZE];

const float GRAVITY = 0.2f;

Point InitializePointPosition( Vector2 pos ) {
    return (Point){ pos, (Vector2){0.0f, 0.0f} };
}

void PopulatePointBuffer( Vector2 origin, float spacing ) {
    int sideLength = sqrtf( POINTBUFFERSIZE );
    Vector2 pos = {0.0f, 0.0f};

    for ( int i = 0; i < POINTBUFFERSIZE; i++ ) {
        PointBuffer[i] = InitializePointPosition( Vector2Add( origin, pos ) );
        pos.x += spacing;
        if ( pos.x >= sideLength * spacing ) {
            pos.x = 0.0f;
            pos.y += spacing;
        }
    }
}

void DrawPoints( float radius ) {
    for ( int i = 0; i < POINTBUFFERSIZE; i++ )
        DrawCircleV(PointBuffer[i].pos, radius, WHITE);
}

void UpdatePoints() {
    for ( int i = 0; i < POINTBUFFERSIZE; i++ ) {
        PointBuffer[i].vel = Vector2Add( PointBuffer[i].vel, (Vector2){0.0f, GRAVITY} );
        PointBuffer[i].pos = Vector2Add( PointBuffer[i].pos, PointBuffer[i].vel );
    }
}

void Update() {
    UpdatePoints();
}

void Draw() {
    BeginDrawing();
        ClearBackground(BLACK);

        DrawPoints( 5.0f );

    EndDrawing();
}

int main() {
    const int
        screenWidth = 1200,
        screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Fluid Simulation");

    SetTargetFPS(60);

    // Initialize
    PopulatePointBuffer( (Vector2){300.0f, 300.0f}, 15.0f );


    while ( !WindowShouldClose() ) {
        Update();
        Draw();
    }

    CloseWindow();

    return 0;
}

