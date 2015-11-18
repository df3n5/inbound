#include <math.h>

#include "cog/src/cog.h"
#include "cog/src/cog_anim.h"
#include "cog/src/cog_math.h"

static const double X_SPEED = 0.3;
static const double X_SPEED_AIR = 0.5;
static const double X_FRICTION = 0.002;

static const double JUMP_SPEED = 0.6;
static const uint32_t JUMP_MILLIS = 750;
static const double GRAVITY = -0.6;

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
    cog_init(.fullscreen = false, .window_w=600, .window_h=600);
		int jumping = 0;
		int moving = 0;
		uint32_t jump_time = 0;

		init();
    while(!cog_hasquit()) {
				update();
        cog_loopstep();
				if(jumping) {
						jump_time += cog_time_delta_millis();
						if(jump_time > JUMP_MILLIS) {
							jumping = 0;
							p.s->vel.y = GRAVITY;							
						}
				} else {
						if(p.s->pos.y < 0.0) p.s->vel.y = 0.0;
				}

        if(cog_input_key_pressed()) {
            uint32_t key = cog_input_key_code_pressed();
            if(key == 'w') {
							p.s->vel.y = JUMP_SPEED;
							jumping = 1;
							jump_time = 0;
							if(p.s->vel.x > 0.001) p.s->vel.x = X_SPEED_AIR;
							if(p.s->vel.x < -0.001) p.s->vel.x = -X_SPEED_AIR;
            }
            if(key == 'a') {
							p.s->vel.x = jumping ? -X_SPEED_AIR : -X_SPEED;
							moving = 1;
            }
            if(key == 'd') {
							p.s->vel.x = jumping ? X_SPEED_AIR : X_SPEED;
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
								if(p.s->vel.x > 0.0) p.s->vel.x -= X_FRICTION;
								else p.s->vel.x += X_FRICTION;
						} else {
								p.s->vel.x = 0.0;
						}
				}
    }
}
