// Simple citro2d sprite drawing example
// Images borrowed from:
//   https://kenney.nl/assets/space-shooter-redux
#include <3ds.h>
#include <citro2d.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

// Simple sprite struct
typedef struct
{
	C2D_Sprite spr;
	float dx, dy;
} Sprite;

static C2D_SpriteSheet spriteSheet;
static Sprite sprite;

static void initSprite(){
    C2D_SpriteFromSheet(&sprite.spr, spriteSheet, 0);
    C2D_SpriteSetCenter(&sprite.spr, 0.5f, 0.5f);
    C2D_SpriteSetPos(&sprite.spr, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    sprite.dx = rand()*4.0f/RAND_MAX - 2.0f;
    sprite.dy = rand()*4.0f/RAND_MAX - 2.0f;
}

int main(int argc, char* argv[]){

    srand(time(NULL));
    
    // Init libs
    romfsInit();
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
	consoleInit(GFX_TOP, NULL);

    // Create screens
	C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    // Load graphics
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);

    // Initialize sprite
	initSprite();

    int points = 0;

    // Main Loop
    while (aptMainLoop()){

        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        touchPosition touch;

		//Read the touch screen coordinates
		hidTouchRead(&touch);

        // Respond to user input
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START){
            break;
        }
            //break; // break in order to return to hbmenu

        // Move Sprite
        C2D_SpriteMove(&sprite.spr, sprite.dx, sprite.dy);
		C2D_SpriteRotateDegrees(&sprite.spr, 1.0f);

		// Check for collision with the screen boundaries
		if ((sprite.spr.params.pos.x < sprite.spr.params.pos.w / 2.0f && sprite.dx < 0.0f) ||
			(sprite.spr.params.pos.x > (SCREEN_WIDTH-(sprite.spr.params.pos.w / 2.0f)) && sprite.dx > 0.0f))
			sprite.dx = -sprite.dx;

		if ((sprite.spr.params.pos.y < sprite.spr.params.pos.h / 2.0f && sprite.dy < 0.0f) ||
			(sprite.spr.params.pos.y > (SCREEN_HEIGHT-(sprite.spr.params.pos.h / 2.0f)) && sprite.dy > 0.0f))
			sprite.dy = -sprite.dy;

        // Check if touch is touching sprite
        if ((touch.px >= sprite.spr.params.pos.x-30 && touch.px <= sprite.spr.params.pos.x+30) &&
            (touch.py >= sprite.spr.params.pos.y-30  && touch.py <= sprite.spr.params.pos.y+30) &&
            (touch.px != 0 && touch.py != 0)){
                points++;
                initSprite();
        }
        
        printf("\x1b[1;1HHatsuGame\x1b[K");
        printf("\x1b[2;1HPontos: %d\x1b[K", points);
        printf("\x1b[3;1HTouchX: %d; TouchY: %d\x1b[K", touch.px, touch.py);
        
        // Render Scene
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(bottom, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
		C2D_SceneBegin(bottom);
		C2D_DrawSprite(&sprite.spr);
		C3D_FrameEnd(0);
    }

    // Delete graphics
	C2D_SpriteSheetFree(spriteSheet);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();

    return 0;
}