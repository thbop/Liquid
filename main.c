#include "raylib.h"
#include "raymath.h"
#include "math.h"


#define POINTBUFFERSIZE 100



struct Point {
    Vector2 pos, vel;
} typedef Point;


Point PointBuffer[POINTBUFFERSIZE];

const int
    SCREENWIDTH = 1200,
    SCREENHEIGHT = 800;

const float
    GRAVITY = 0.2f,
    RADIUS = 5.0f,
    STARTSPACING = RADIUS * 3.0f,
    BOUNCEDAMP = 0.99f;

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

void PointCollisions( Point* v ) {
    if      ( v->pos.x < 0.0f || v->pos.x > SCREENWIDTH  ) v->vel = Vector2Multiply( v->vel, (Vector2){-BOUNCEDAMP, 0} );
    else if ( v->pos.y < 0.0f || v->pos.y > SCREENHEIGHT ) v->vel = Vector2Multiply( v->vel, (Vector2){0, -BOUNCEDAMP} );
}

void UpdatePoints() {
    for ( int i = 0; i < POINTBUFFERSIZE; i++ ) {
        PointBuffer[i].vel = Vector2Add( PointBuffer[i].vel, (Vector2){0.0f, GRAVITY} );
        PointBuffer[i].pos = Vector2Add( PointBuffer[i].pos, PointBuffer[i].vel );

        PointCollisions( PointBuffer+i );
    }
}

void Update() {
    UpdatePoints();
}

void Draw() {
    BeginDrawing();
        ClearBackground(BLACK);

        DrawPoints( RADIUS );

    EndDrawing();
}

int main() {
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Fluid Simulation");

    SetTargetFPS(60);

    // Initialize
    PopulatePointBuffer( (Vector2){300.0f, 300.0f}, STARTSPACING );


    while ( !WindowShouldClose() ) {
        Update();
        Draw();
    }

    CloseWindow();

    return 0;
}

