#include "wasm4.h"
#include "global.h"

void start()
{
	PALETTE[0]=0x472D3C;	// dark brown
    PALETTE[1]=0x00B563;    // light green
    PALETTE[2]=0x007F46;    // dark green
    PALETTE[3]=0x000000;    // black

    global::SetupGlobals();
}

void update()
{
    global::input->Update();
    global::game->HandleInput(global::input);
    global::game->Update(1,global::game);
    global::game->Draw();
}
