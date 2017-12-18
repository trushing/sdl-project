#include "SDL/SDL.h"
#include "SDL/SDL_Mixer.h"
#include "SDL/SDL_Image.h"
#include "SDL/SDL_TTF.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int SCREEN_BPP = 32;

const int FRAMES_PER_SECOND = 60;

const int TILE_WIDTH = 40;
const int TILE_HEIGHT = 40;

const int MAX_UNITS = 10;
const int TILE_SPRITES = 2;

const int CLIP_MOUSEUP = 0;
const int CLIP_MOUSEDOWN = 1;

const int TILE_FLOOR = 0;
const int TILE_RED = 1;

SDL_Surface *screen = NULL;

//SDL_Surface *background = NULL;
SDL_Surface *character = NULL;
SDL_Surface *floor = NULL;
SDL_Surface *selection = NULL;

SDL_Rect clips[ TILE_SPRITES ];

SDL_Event event;

TTF_Font *font = NULL;

SDL_Color textColor = {0, 162, 232};


void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{

    SDL_Rect offset;


    offset.x = x;
    offset.y = y;


    SDL_BlitSurface( source, clip, destination, &offset );
}

SDL_Surface *load_image( std::string filename ) 
{
    SDL_Surface* loadedImage = NULL;
    
    SDL_Surface* optimizedImage = NULL;
    
    loadedImage = IMG_Load( filename.c_str() );
    
    if( loadedImage != NULL )
    {
        optimizedImage = SDL_DisplayFormat( loadedImage );
        
        SDL_FreeSurface( loadedImage );
        
        if( optimizedImage != NULL )
        {
            Uint32 colorkey = SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF );
            
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, colorkey );
        }
    }
    
    return optimizedImage;
}

void clip_sprites()
{
     clips[TILE_FLOOR].x = 0;
     clips[TILE_FLOOR].y = 0;
     clips[TILE_FLOOR].w = TILE_WIDTH;
     clips[TILE_FLOOR].h = TILE_HEIGHT;
     
     clips[TILE_RED].x = 40;
     clips[TILE_RED].y = 0;
     clips[TILE_RED].w = TILE_WIDTH;
     clips[TILE_RED].h = TILE_HEIGHT;
}

int get_tile(int xcoord, int ycoord)
{
     return (ycoord / TILE_WIDTH) * 32 + (xcoord / TILE_WIDTH);
}

/*int* get_path(int start, int finish, int maxtiles)
{
     int dijkstra[maxtiles]
     
     for(int i = 0; i < maxtiles; i++)
     {
         dijkstra[i] = 999;
     }
}*/

// -----------------------------------------------------------------------------
// -------------------------------------MAIN------------------------------------
int main(int argc, char* args[])
{
    bool quit = false;
    bool charcontrol = false;
    bool updatemap = false;
    bool tileswapactive = false;
    bool erase = false;
    bool cap = true;
    int activeunit = 0;
    int unitcount = 0;
    int charx[MAX_UNITS] = {40};
    int chary[MAX_UNITS] = {40};
    int tilesx, tilesy, totaltiles;
    int xtile = 0, ytile = 0;
    int selectedtile;
    int startticks;
    
    tilesx = SCREEN_WIDTH / 40;
    tilesy = SCREEN_HEIGHT / 40;
    totaltiles = tilesx*tilesy;
    
    for(int i = 0; i<MAX_UNITS; i++){charx[i] = 40;chary[i] = 40;}
    
    int tilearray[totaltiles];
    int pathtestarray[totaltiles];
    
    clip_sprites();
    
    
    if(SDL_Init( SDL_INIT_EVERYTHING ) == -1)
    {
       return 1;
    }
    
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
    
    if(screen == NULL)
    {
       return 1;
    }
    
    if(TTF_Init() == -1)
    {
       return 1;
    }
    
    
    SDL_WM_SetCaption( "Profectus", NULL );
    
    //background = load_image("background.png");
    character = load_image("charsprite2.png");
    floor = load_image("floor.png");
    selection = load_image("selectionbox.png");
    
    font = TTF_OpenFont( "lazy.ttf", 32 );
    
    std::fstream maptest;
    maptest.open("maptest.txt");
    
    for(int tilecount = 0; tilecount < totaltiles; tilecount++)
    {
            maptest >> tilearray[tilecount];
            apply_surface(xtile,ytile,floor,screen,&clips[tilearray[tilecount]]);
            xtile += 40;
            
            if(xtile >= SCREEN_WIDTH)
            {
               xtile = 0;
               ytile += 40;
            }
    }
    xtile = 0;
    ytile = 0;
    maptest.close();
    
    
    while(quit == false)
{
    startticks = SDL_GetTicks();
    
    while(SDL_PollEvent(&event))
    {
          if(event.type == SDL_KEYDOWN)
          {
             if(event.key.keysym.sym == SDLK_n)
             {
                if(unitcount < MAX_UNITS)
                {
                   unitcount++;
                }
             }
             if(event.key.keysym.sym == SDLK_s)
             {
                maptest.open("maptest.txt", std::ios::out);
                for(int tilecount = 0; tilecount < totaltiles; tilecount++)
                {
                    maptest << tilearray[tilecount] << " ";
                }
                maptest.close();
             }
             if(event.key.keysym.sym == SDLK_r)
             {
                maptest.open("maptest.txt", std::ios::out);
                for(int tilecount = 0; tilecount < totaltiles; tilecount++)
                {
                    tilearray[tilecount] = 0;
                    maptest << 0 << " ";
                }
                maptest.close();
             }
          }
                                
          if(event.type == SDL_MOUSEBUTTONDOWN)
          {             
             if(event.button.button == SDL_BUTTON_LEFT)
             {
              for(int scount = 0; scount <= unitcount; scount++)
              {
                if((event.button.x > charx[scount]) && (event.button.x < (charx[scount] + TILE_WIDTH)) && (event.button.y > chary[scount]) && (event.button.y < (chary[scount] + TILE_HEIGHT)))
                {
                    charcontrol = true;
                    activeunit = scount;
                    break;
                }
                else{charcontrol = false;}
              }
             }
             if(event.button.button == SDL_BUTTON_RIGHT)
             {
                if(charcontrol == true)
                {
                   //get_path();
                   charx[activeunit] = (event.button.x / TILE_WIDTH) * TILE_WIDTH;
                   chary[activeunit] = (event.button.y / TILE_HEIGHT) * TILE_HEIGHT;
                }
                
                if(charcontrol == false)
                {
                   tileswapactive = true;
                   selectedtile = get_tile(event.button.x, event.button.y);
                   if(tilearray[selectedtile] != 1){tilearray[selectedtile] = 1;}
                   else{erase = true;tilearray[selectedtile] = 0;}
                   //selectedtile -= 1;
                   //tilearray[selectedtile] = 1;
                }
             }
          }
          
          if(event.type == SDL_MOUSEBUTTONUP)
          {
             if(event.button.button == SDL_BUTTON_RIGHT)
             {
                tileswapactive = false;
             }
          }
          
          if(event.type == SDL_MOUSEMOTION)
          {
             if(tileswapactive == true)
             {
                selectedtile = get_tile(event.button.x, event.button.y);
                if((tilearray[selectedtile] != 1)&&(erase == false)){tilearray[selectedtile] = 1;}
                if((erase == true)&&(tilearray[selectedtile] != 0)){tilearray[selectedtile] = 0;}
             }
          }

       if(event.type == SDL_QUIT)
       {
          quit = true;
       }
    }
    
// apply_surface(0,0,background,screen);
for(int tilecount = 0; tilecount < totaltiles; tilecount++)
    {
            apply_surface(xtile,ytile,floor,screen,&clips[tilearray[tilecount]]);
            xtile += 40;
            
            if(xtile >= SCREEN_WIDTH)
            {
               xtile = 0;
               ytile += 40;
            }
    }
    xtile = 0;
    ytile = 0;
    
    for(int chardraw = 0; chardraw <= unitcount; chardraw++)
    {
        apply_surface(charx[chardraw],chary[chardraw],character,screen);
 
        if(charcontrol == true){apply_surface(charx[activeunit],chary[activeunit],selection,screen);}
    }
    
 if( SDL_Flip( screen ) == -1 )
 {
 return 1;
 }
 
  if((cap == true)&&((SDL_GetTicks() - startticks) < 1000 / FRAMES_PER_SECOND))
  {
     SDL_Delay(( 1000 / FRAMES_PER_SECOND) - (SDL_GetTicks() - startticks));
  }

}

//SDL_FreeSurface(background);
SDL_FreeSurface(character);

TTF_CloseFont(font);
TTF_Quit();

SDL_Quit();

return 0;
}
