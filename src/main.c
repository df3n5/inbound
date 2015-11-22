#include <math.h>

#include "cog/src/cog.h"
#include "cog/src/cog_anim.h"
#include "cog/src/cog_math.h"

static const double X_SPEED = 0.5;
static const double X_SPEED_AIR = 0.7;
static const double X_FRICTION = 0.0002;

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
    cog_init(.fullscreen = false, .debug=false, .window_w=600, .window_h=600);
		bool jumping = false;
		bool moving = false;
		uint32_t jump_time = 0;

		init();
    while(!cog_hasquit()) {
				update();
        cog_loopstep();
				if(jumping) {
						// Jump lasts an amount of time.
						jump_time += cog_time_delta_millis();
						if(jump_time > JUMP_MILLIS) {
							p.s->vel.y = GRAVITY;							
							// Collide with ground
							if(p.s->pos.y < 0.0) {
									jumping = false;
									p.s->vel.y = 0.0;
									cog_debugf("hi");
							}
						}
				}

        if(cog_input_key_pressed()) {
            uint32_t key = cog_input_key_code_pressed();
						cog_debugf("Key w <%d> and jumping <%d>", key=='w', jumping);
            if(!jumping && key == 'w') {
							p.s->vel.y = JUMP_SPEED;
							jumping = true;
							jump_time = 0;
							if(p.s->vel.x > 0.001) p.s->vel.x = X_SPEED_AIR;
							if(p.s->vel.x < -0.001) p.s->vel.x = -X_SPEED_AIR;
            }
            if(key == 'a') {
							p.s->vel.x = jumping ? -X_SPEED_AIR : -X_SPEED;
							moving = true;
            }
            if(key == 'd') {
							p.s->vel.x = jumping ? X_SPEED_AIR : X_SPEED;
							moving = true;
            }
        } 
				if(cog_input_key_depressed()) {
						uint32_t key = cog_input_key_code_depressed();
						if(key == 'a' || key == 'd') {
								moving = false;
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
