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
    MASS = 1.0f,
    STARTSPACING = RADIUS * 3.0f,
    BOUNCEDAMP = 0.7f,
    SMOOTHINGRADIUS = 200.0f,
    SMOOTHINGVOLUME = (SMOOTHINGRADIUS*SMOOTHINGRADIUS*SMOOTHINGRADIUS*PI) / 2.0f; // ish Wolfram says pi*s^4/2

// int mode
Vector2 Vector2Random( int minX, int maxX, int minY, int maxY ) {
    return (Vector2){ (float)GetRandomValue( minX, maxX ), (float)GetRandomValue( minY, maxY ) };
}

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

void ScatterPointBuffer() {
    for ( int i = 0; i < POINTBUFFERSIZE; i++ )
        PointBuffer[i].pos = Vector2Random( 0, SCREENWIDTH, 0, SCREENHEIGHT );
}

void DrawPoints( float radius ) {
    for ( int i = 0; i < POINTBUFFERSIZE; i++ )
        DrawCircleV(PointBuffer[i].pos, radius, WHITE);
}

void PointCollisions( Point* v ) {
    if ( v->pos.x < RADIUS ) {
        v->pos.x = RADIUS;
        v->vel.x *= -BOUNCEDAMP;
    } else if ( v->pos.x > SCREENWIDTH-RADIUS ) {
        v->pos.x = SCREENWIDTH-RADIUS;
        v->vel.x *= -BOUNCEDAMP;
    }

    if ( v->pos.y < RADIUS ) {
        v->pos.y = RADIUS;
        v->vel.y *= -BOUNCEDAMP;
    } else if ( v->pos.y > SCREENHEIGHT-RADIUS ) {
        v->pos.y = SCREENHEIGHT-RADIUS;
        v->vel.y *= -BOUNCEDAMP;
    }

}

float SmoothingKernel( float x ) {
    float v = fmax( 0.0, SMOOTHINGRADIUS - x );
    return v*v*v / SMOOTHINGVOLUME;
}

float CalculateDensity( Vector2 samplePoint ) {
    float density = 0;
    for ( int i = 0; i < POINTBUFFERSIZE; i++ ) {
        float distance = Vector2Distance( samplePoint, PointBuffer[i].pos );
        density += SmoothingKernel( distance ) * MASS;
    }
    return density;
}

void UpdatePoints() {
    for ( int i = 0; i < POINTBUFFERSIZE; i++ ) {
        PointBuffer[i].vel = Vector2Add( PointBuffer[i].vel, (Vector2){0.0f, GRAVITY} );
        PointBuffer[i].pos = Vector2Add( PointBuffer[i].pos, PointBuffer[i].vel );

        PointCollisions( PointBuffer+i );
    }
}

void Update() {
    // UpdatePoints();
}

void Draw() {
    BeginDrawing();
        ClearBackground(BLACK);

        DrawPoints( RADIUS );

        DrawText( TextFormat( "DENSITY: %F", CalculateDensity( GetMousePosition() ) ), 10, 10, 20, WHITE );

    EndDrawing();
}

int main() {
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Fluid Simulation");

    SetTargetFPS(60);

    // Initialize
    // PopulatePointBuffer( (Vector2){300.0f, 300.0f}, STARTSPACING );
    ScatterPointBuffer();


    while ( !WindowShouldClose() ) {
        Update();
        Draw();
    }

    CloseWindow();

    return 0;
}

