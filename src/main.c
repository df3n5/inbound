#include <math.h>

#include "cog/src/cog.h"
#include "cog/src/cog_anim.h"
#include "cog/src/cog_math.h"

static const int N_ENTITIES = 1;

typedef struct player {
    cog_sprite* s;
    cog_sprite_id sid;
} player;

static player p;

void init() {
		p.sid = cog_sprite_add("../assets/c0.png");
    cog_sprite_set(p.sid, (cog_sprite) {
        .dim=(cog_dim2) {
            .w=0.1, .h=0.1
        },
        .rot=COG_PI/2
    });
		p.s = cog_sprite_get(p.sid);
}

void update() {
}

int main(int argc, char* argv[]) {
    cog_init(.fullscreen = false);
		int moving = 0;

		init();
    while(!cog_hasquit()) {
				update();
        cog_loopstep();
        if(cog_input_key_pressed()) {
            uint32_t key = cog_input_key_code_pressed();
            cog_debugf("key is %d", key);
            if(key == 'a') {
							p.s->vel.x = -0.1;							
							moving = 1;
            }
            if(key == 'd') {
							p.s->vel.x = 0.1;
							moving = 1;
            }
        } 
				if(cog_input_key_depressed()) {
						uint32_t key = cog_input_key_code_depressed();
						if(key == 'a' || key == 'd') {
								moving = 0;
						}
				}
				if(!moving) {
						if(fabs(p.s->vel.x) > 0.01) {
								if(p.s->vel.x > 0.0) p.s->vel.x -= 0.002;
								else p.s->vel.x += 0.002;
						} else {
								p.s->vel.x = 0.0;
						}
				}
    }
}
