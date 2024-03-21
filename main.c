#include "raylib.h"
#include "raymath.h"
#include "math.h"

#define POINTBUFFERSIZE 400

Vector2 Vector2DivideValue( Vector2 p, float v ) {
    return (Vector2){ p.x / v, p.y / v };
}

Vector2 Vector2MultiplyValue( Vector2 p, float v ) {
    return (Vector2){ p.x * v, p.y * v };
}

struct Point {
    Vector2 pos, vel;
    float areaDensity;
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
    SMOOTHINGRADIUS = 1.2f,
    SMOOTHINGVOLUME = (SMOOTHINGRADIUS*SMOOTHINGRADIUS*SMOOTHINGRADIUS*SMOOTHINGRADIUS*PI) / 2.0f, // ish Wolfram says pi*s^4/2
    TARGETDENSITY = 2.75f,
    PRESSUREMULTIPLIER = 0.5f;

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

float SmoothingKernelSlope( float x ) {
    float v = 3 * fmax( 0.0, SMOOTHINGRADIUS - x );
    return v*v;
}

float CalculateDensity( Vector2 samplePoint ) {
    float density = 0;
    for ( int i = 0; i < POINTBUFFERSIZE; i++ ) {
        float distance = Vector2Distance( samplePoint, PointBuffer[i].pos );
        density += SmoothingKernel( distance ) * MASS;
    }
    return density;
}

float ConvertDensityToPressure( float density ) {
    float densityError = density - TARGETDENSITY;
    return densityError * PRESSUREMULTIPLIER; // return pressure
}

Vector2 CalculatePressureForce( int PointIndex ) {
    Vector2
        samplePoint = PointBuffer[PointIndex].pos,
        pressureForce = {0.0f};
    
    for ( int i = 0; i < POINTBUFFERSIZE; i++ ) {
        if ( PointIndex == i ) break;
        float distance = Vector2Distance( samplePoint, PointBuffer[i].pos );
        Vector2 direction = distance == 0 ? Vector2Random(-1, 1, -1, 1) : Vector2DivideValue( Vector2Subtract( PointBuffer[i].pos, samplePoint ), distance );
        float slope = SmoothingKernelSlope( distance );
        float density = PointBuffer[i].areaDensity;
        float pressure = ConvertDensityToPressure(density);

        // gradient += -pressure * direction * slope * mass / density;
        pressureForce = Vector2Add( pressureForce, Vector2DivideValue( Vector2MultiplyValue( direction, -slope * MASS * pressure ), density ) );
    }
    return pressureForce;
}


void UpdatePoints() {
    for ( int i = 0; i < POINTBUFFERSIZE; i++ ) {
        // PointBuffer[i].vel = Vector2Add( PointBuffer[i].vel, (Vector2){0.0f, GRAVITY} );
        PointBuffer[i].areaDensity = CalculateDensity( PointBuffer[i].pos ); // Cache densities

        Vector2 pressureForce = CalculatePressureForce( i );
        Vector2 pressureAcceleration = Vector2DivideValue( pressureForce, PointBuffer[i].areaDensity );
        PointBuffer[i].vel = Vector2Add( PointBuffer[i].vel, pressureAcceleration );

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

        // DrawText( TextFormat( "DENSITY: %F", CalculateDensity( GetMousePosition() ) ), 10, 10, 20, WHITE );

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

