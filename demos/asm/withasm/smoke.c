

//Using SDL, SDL_image, standard IO, and strings
//Using SDL, SDL_image, standard IO, and strings
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define TRUE 1
#define FALSE 0

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
static SDL_Texture *smokeTexture = NULL;
static SDL_Texture *bgTexture = NULL;
static SDL_Renderer *renderer = NULL;
//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//SDL_Rect smoke;
SDL_Rect smoke;
SDL_Rect bg;
static int quit = FALSE;


//Loads media
int loadMedia();


#define PART_COUNT 25

typedef struct particle {
    double m_x;
    double m_y;
    Uint32 m_age;
    float m_xVector;
    float m_yVector;
    float m_scale;
    float m_alpha;
    int m_canRegen;
    double m_angle;
    Uint32 m_timeDie;
} Particle;

typedef struct particles {
    double m_x;
    double m_y;
    double m_dieRate;
    int m_image;
    float m_speed;
    float m_alpha;
    float m_windVelocity;
    Uint32 m_lastRender;
    Particle particles[PART_COUNT];

} Particles;

void logParticle(Particle * em)
{
    printf("emitter x %f y=%f age=%u scale=%f timeTime=%u\n", em->m_x, em->m_y, em->m_age, em->m_scale, em->m_timeDie );

}

void logEmitter(Particles * em, int parts)
{
    printf("emitter x %f y=%f dieRate=%f speed=%f alpha=%f wind=%f\n", em->m_x, em->m_y, em->m_dieRate, em->m_speed, em->m_alpha, em->m_windVelocity );
    if ( parts > 0) {
        for (int i = 0; i < PART_COUNT; i++)
        {
            logParticle( &em->particles[i]);
        }
    }
}




double rand_range(double x0, double x1)
{
    return x0 + (x1 - x0) * rand() / ((double) RAND_MAX);
}
double r01()
{
    return rand_range(0, 1);
}
void startRand(Particles *emitter, Particle* part, int partnumber)
{
    //srand(time(0));
    // smoke rises and spreads
    part->m_xVector = (r01()) * .25 - 0.25;
    part->m_yVector = -0.2  - (r01());
    part->m_timeDie = 15000 + floor(r01() * 9000);
    //printf( "partnum = %d timedie = %f rand = %f\n",  partnumber, part->m_timeDie , r01());

    double invDist = 1.0 / sqrt(part->m_xVector * part->m_xVector
                                + part->m_yVector * part->m_yVector);
    // normalise speed
    part->m_xVector = part->m_xVector * invDist * emitter->m_speed;
    part->m_yVector = part->m_yVector * invDist * emitter->m_speed;
    // starting position within a 20 pixel area
    part->m_x = (emitter->m_x + floor(r01() * 20) - 10);
    part->m_y = (emitter->m_y + floor(r01() * 20) - 10);
    // the initial age may be > 0. This is so there is already a smoke trail in
    // place at the start
    part->m_x += (part->m_xVector + emitter->m_windVelocity) * part->m_age;
    part->m_y += part->m_yVector * part->m_age;
    part->m_scale = 0.01;
    part->m_alpha = 0;
    part->m_angle = round(rand_range(0, 360));
    printf( "x = %f y =%f\n", part->m_x, part->m_y);

}


Uint32 getMilliCount() {
    return SDL_GetTicks();
}

int isAlive(Particle* part)
{
    //if( part->m_y < 20 ) return 0;
    //printf( "age %u timedie %u\n", part->m_age, part->m_timeDie);
    return part->m_age < part->m_timeDie;
}
void update(Particles *uemitter, Particle* part, int partNumber)
{
    Particles uemit = *uemitter;
    Uint32 timeElapsed;
    Uint32 curr = getMilliCount();
    timeElapsed = curr - uemit.m_lastRender;
    //printf("get -elapsed %d emitter last render %d\n", timeElapsed, emitter->m_lastRender);
    uemit.particles[partNumber].m_age += timeElapsed;
    if (!isAlive(&(uemit.particles[partNumber])))
    {
        printf("Dead\n");
        //printf( "age %u timedie %u\n", part->m_age, part->m_timeDie);
        // smoke eventually dies
        /*if (r01() > emitter.m_dieRate)
        {
          part->m_canRegen = FALSE;
        }
        if (!part->m_canRegen)
        {
          return;
        }*/
        // regenerate
        uemit.particles[partNumber].m_age = 1;
        startRand(&uemit, &(uemit.particles[partNumber]), partNumber);
        *uemitter = uemit;
        return;
    }
    // At start the particle fades in and expands rapidly (like in real life)
    double fadeIn = uemit.particles[partNumber].m_timeDie * 0.01;
    double startScale;
    double maxStartScale = 0.4;
    if (uemit.particles[partNumber].m_age < fadeIn)
    {
        uemit.particles[partNumber].m_alpha = ((uemit.particles[partNumber].m_age / fadeIn));
        //printf("fade calc = %f \n", (part->m_age/fadeIn)*10 );
        startScale = uemit.particles[partNumber].m_alpha * maxStartScale;
        // y increases quicker because particle is expanding quicker
        uemit.particles[partNumber].m_y += uemit.particles[partNumber].m_yVector * timeElapsed;

    }
    else
    {
        uemit.particles[partNumber].m_alpha = 1.0 - (uemit.particles[partNumber].m_age - fadeIn) / (uemit.particles[partNumber].m_timeDie - fadeIn);
        startScale = maxStartScale;
        uemit.particles[partNumber].m_y += uemit.particles[partNumber].m_yVector * timeElapsed;
    }
    // the x direction is influenced by wind velocity
    uemit.particles[partNumber].m_x += (uemit.particles[partNumber].m_xVector + uemit.m_windVelocity) * timeElapsed;
    uemit.particles[partNumber].m_scale = 0.001 + startScale + uemit.particles[partNumber].m_age / 4000.0;
    //printf( "x = %f y =%f\n", uemit.particles[partNumber].m_x, uemit.particles[partNumber].m_y);
    /*printf("update m_speed: %f\n", emitter->m_speed);*/
    *uemitter = uemit;
}

void renderParticle(Particle* part, int partNumber)
{
    SDL_Rect *clip = NULL;
    //if (!isAlive(part)) return;
    if ( partNumber == 0 ) return;
    SDL_Rect smokeLocal = smoke;

    smokeLocal.h = round(smoke.h * part->m_scale);
    smokeLocal.w = round(smoke.w * part->m_scale);
    smokeLocal.x = round(part->m_x); //part->m_x-smoke.h * part->m_scale/2;
    smokeLocal.y = round(part->m_y); //part->m_y+smoke.w * part->m_scale/2;

//printf("test");

    SDL_SetTextureAlphaMod(smokeTexture, part->m_alpha * 150 );
    SDL_RenderCopyEx(renderer,
                     smokeTexture,
                     clip,
                     &smokeLocal,
                     0 /*part->m_angle*/,
                     NULL,
                     SDL_FLIP_NONE);


}


void initParticles(Particles *em, int x, int y) {
    em->m_speed = 0.02;
    em->m_alpha = 1.0;
    em->m_windVelocity = 0.025;
    em->m_dieRate = 0.95;
    em->m_x = x;
    em->m_y = y;
    em->m_lastRender = getMilliCount();
    for (int i = 0; i < PART_COUNT; i++)
    {
        em->particles[i].m_x = 0;
        em->particles[i].m_canRegen = 1;
        em->particles[i].m_age = i * 50000 * em->m_speed;

        //Particle *cparticle = &emitter->particles[i];
        startRand(&*em, &((*em).particles[i]), i);
        //printf( "part = %d x = %f y =%f age = %u die = %u\n",i, em.particles[i].m_x, em.particles[i].m_y, em.particles[i].m_age, em.particles[i].m_timeDie);

    }
}








int init(SDL_Window **window, SDL_Renderer **renderer) {


    //srand(time(NULL));

    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return 1;
    }

    //start_time = SDL_GetTicks();
    //Create window
    gWindow = SDL_CreateWindow( "Smoke Example",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                SCREEN_WIDTH,
                                SCREEN_HEIGHT,
                                SDL_WINDOW_SHOWN );
    if ( gWindow == NULL ) {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return 1;
    }
    printf("Got Window\n");

    *renderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_SetRenderDrawColor(*renderer, 0xFF, 0xFF, 0xFF, 0xFF);

#ifndef EMSCRIPTEN
    int imgFlags = IMG_INIT_PNG;
    if ( !( IMG_Init( imgFlags ) & imgFlags ) ) {
        printf( "SDL_image could not initializessssss! SDL_image Error: %s\n", IMG_GetError() );
        return 1;
    }
#endif

    return 0;
}



int load_texture(SDL_Texture **smoketexture,
                 SDL_Renderer *renderer) {
    const char *bg_path = "aftermath.png";
    const char *smoke_path = "smoke.png";
    //Load image at specified path
    SDL_Surface *loaded_surface = IMG_Load(smoke_path);
    if (loaded_surface == NULL) {
        printf( "Unable to load image %s! SDL_image Error: %s\n",
                smoke_path,
                IMG_GetError());
        return 1;
    }

    //Create texture from surface pixels
    smokeTexture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
    if ( smokeTexture == NULL ) {
        printf( "Unable to create texture from %s! SDL Error: %s\n",
                smoke_path,
                SDL_GetError() );
        return 1;
    }
    smoke.w = loaded_surface->w;
    smoke.h = loaded_surface->h;
    //Get rid of old loaded surface
    SDL_FreeSurface(loaded_surface);

    //Load image at specified path
    loaded_surface = IMG_Load(bg_path);
    if (loaded_surface == NULL) {
        printf( "Unable to load image %s! SDL_image Error: %s\n",
                bg_path,
                IMG_GetError());
        return 1;
    }

    //Create texture from surface pixels
    bgTexture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
    if ( bgTexture == NULL ) {
        printf( "Unable to create texture from %s! SDL Error: %s\n",
                smoke_path,
                SDL_GetError() );
        return 1;
    }
    bg.w = loaded_surface->w;
    bg.h = loaded_surface->h;
    //Get rid of old loaded surface
    SDL_FreeSurface(loaded_surface);

    return 0;
}

void loop(void *passedEmit)
{
    //runs long enough and something is getting gced and x and y get off
    //change dead routine to use local copy of emitter
    Particles * emit = (Particles *)passedEmit;
    //make a local copy of the emitter
    Particles lemit = *emit;
    double angle = 0.0;
    SDL_Rect *clip = NULL;
    //variable maybe getting optimized out
    //printf("wind %f x = %f y = %f\n", lemit.m_windVelocity, lemit.particles[1].m_x, lemit.particles[1].m_y);
    //Event handler
    SDL_Event ev;

    //Handle events on queue
    while ( SDL_PollEvent( &ev ) != 0 )
    {
        //User requests quit
        if ( ev.type == SDL_QUIT )
        {
            quit = TRUE;
        }
    }


    //emitter->m_speed = 0.02;
    lemit.m_windVelocity += (r01() - 0.5) * 0.0015;
    if (lemit.m_windVelocity > 0.015)
    {
        lemit.m_windVelocity = 0.015;
    }
    if (lemit.m_windVelocity < 0.0)
    {
        lemit.m_windVelocity = 0.0;
    }
    SDL_RenderClear(renderer);
    SDL_RenderCopyEx(renderer,
                     bgTexture,
                     clip,
                     &bg,
                     angle,
                     NULL,
                     SDL_FLIP_NONE);

    for (int i = 0; i < PART_COUNT; i++)
    {
        //Particle *cparticle = &emitter->particles[i];
        update(&lemit, &(lemit.particles[i]), i);
        renderParticle(&(lemit.particles[i]), i);

    }
    //lastmill = getMilliCount();
    SDL_RenderPresent(renderer);
    //Update the surface
    SDL_UpdateWindowSurface( gWindow );
    lemit.m_lastRender = getMilliCount();
    //save the local copy back into the main emitter
    *((Particles *)passedEmit) = lemit;

    //logEmitter( emitter, 0 );

}

int main( int argc, char* args[] )
{
    Particles emitter;

    //Start up SDL and create window
    if ( init(&gWindow, &renderer) )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {

        if (load_texture(&smokeTexture,  renderer) == 1) {
            printf("Failed to load texture\n");
        }
        else
        {


            initParticles( &emitter, 170, 315 );

            printf("in main %f\n", emitter.m_speed);
            emitter.m_lastRender = getMilliCount();

            //Main loop flag

#ifdef EMSCRIPTEN
            emscripten_set_main_loop_arg((em_arg_callback_func)loop, (void*)&emitter, 0, 1);
#else
            while (!quit) {
                SDL_Delay(15);
                loop(&emitter);
            }
#endif

        }
    }

    //Free resources and close SDL
    SDL_DestroyTexture(smokeTexture);
    smokeTexture = NULL;

    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    IMG_Quit();
    SDL_Quit();

    return 0;
}
