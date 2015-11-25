#include <math.h>

#include "cog/src/cog.h"
#include "cog/src/cog_anim.h"
#include "cog/src/cog_math.h"

static const double X_SPEED = 0.5;
static const double X_SPEED_AIR = 0.7;
static const double X_FRICTION = 0.0002;

static double Y_LEVEL = -0.5;
static double PLATFORM_HEIGHT = 0.05;
static const double JUMP_SPEED = 0.6;
static const uint32_t JUMP_MILLIS = 750;
static const double GRAVITY = -0.6;

typedef struct player {
    //cog_sprite* s;
    cog_rect* s;
    cog_rect_id sid;
} player;

typedef struct flag {
    cog_sprite_id sid;
    cog_rect_id pole_id;
} flag;

typedef struct platform {
    cog_rect_id sid;
} platform;

typedef struct trap {
    cog_rect_id sid;
} trap;

static player p;
static flag f;
static cog_list platforms;
static cog_list traps;

void init() {
    //p.sid = cog_rect_add("../assets/c0.png");
    // Player
    p.sid = cog_rect_add();
    cog_rect_set(p.sid, (cog_rect) {
            .pos=(cog_pos2) {
            .x=-0.9, .y=-0.35
            },
            .dim=(cog_dim2) {
            .w=0.05, .h=0.1
            },
            .col=(cog_color) {
            .r=0.5,.g=1,.b=1,.a=1
            },
            .rot=COG_PI/2,
            .layer = 5
            });
    p.s = cog_rect_get(p.sid);

    // Flag
    f.sid = cog_sprite_add("../assets/flags/greece.png");
    double w = 0.54;
    cog_sprite_set(p.sid, (cog_sprite) {
            .dim=(cog_dim2) {
            .w=w, .h=w * 0.6
            },
            .pos=(cog_pos2) {
            .x=-0.4, .y=0.5
            }
            });
    cog_sprite* s = cog_sprite_get(p.sid);
    f.pole_id = cog_rect_add();
    cog_rect_set(f.pole_id, (cog_rect) {
            .dim=(cog_dim2) {
            .w=0.01, .h=0.65
            },
            .pos=(cog_pos2) {
            .x=s->pos.x - s->dim.w, .y=s->pos.y - s->dim.h
            },
            .col=(cog_color) {
            .r=0.5450980392156862,.g=0.27,.b=0.07,.a=1
            },
            .rot=COG_PI/2,
            .layer=0
            });

    // Platforms
    cog_list_init(&platforms, sizeof(platform));

    cog_rect_id pl_0 = cog_rect_add();
    cog_rect_set(pl_0, (cog_rect) {
            .dim=(cog_dim2) {
            .w=0.1, .h=PLATFORM_HEIGHT
            },
            .rot=COG_PI/2,
            .pos=(cog_pos2) {
            .x=-0.9, .y=-0.5
            },
            .col=(cog_color) {
            .r=1,.g=1,.b=1,.a=1
            }
            });

    cog_list_append(&platforms, &(platform) {
            .sid=pl_0
            });

    cog_rect_id pl_1 = cog_rect_add();
    cog_rect_set(pl_1, (cog_rect) {
            .dim=(cog_dim2) {
            .w=0.1, .h=PLATFORM_HEIGHT
            },
            .rot=COG_PI/2,
            .pos=(cog_pos2) {
            .x=0.1, .y=-0.5
            },
            .col=(cog_color) {
            .r=1,.g=1,.b=1,.a=1
            }
            });
    cog_list_append(&platforms, &(platform) {
            .sid=pl_1
            });

    cog_rect* pl_0_r = cog_rect_get(pl_0);
    cog_rect* pl_1_r = cog_rect_get(pl_1);

    // Death traps
    cog_list_init(&traps, sizeof(trap));

    cog_rect_id trap_0 = cog_rect_add();
    cog_rect_set(trap_0, (cog_rect) {
            .dim=(cog_dim2) {
            .w=0.35, .h=PLATFORM_HEIGHT*6
            },
            .rot=COG_PI/2,
            .pos=(cog_pos2) {
            .x=(pl_0_r->pos.x + pl_1_r->pos.x) / 2.0, .y=-0.5
            },
            .col=(cog_color) {
            .r=1,.g=0,.b=0,.a=1
            }
            });

    cog_list_append(&traps, &(trap) {
            .sid=pl_0
            });
}

void update() {
}

int main(int argc, char* argv[]) {
    cog_init(.fullscreen = false, .debug=false, .window_w=800, .window_h=600);
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
                if((p.s->pos.y - p.s->dim.h - PLATFORM_HEIGHT) < Y_LEVEL) {
                    jumping = false;
                    p.s->vel.y = 0.0;
                }
            }
        }

        if(cog_input_key_pressed()) {
            uint32_t key = cog_input_key_code_pressed();
            //cog_debugf("Key w <%d> and jumping <%d>", key=='w', jumping);
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

        COG_LIST_FOREACH(&traps) {
            trap* t = (trap*)curr->data;
            if(cog_rect_collides_rect(t->sid, p.sid)) {
                cog_debugf("COLLISON");
            }
        }
    }
}
