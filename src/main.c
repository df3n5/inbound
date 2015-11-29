#include <math.h>
#include <SDL2/SDL_opengl.h>

#include "cog/src/cog.h"
#include "cog/src/cog_anim.h"
#include "cog/src/cog_math.h"

static const double X_SPEED = 0.5;
static const double X_SPEED_AIR = 0.7;
static const double X_FRICTION = 0.1;

static double Y_LEVEL = -0.5;
static double PLATFORM_HEIGHT = 0.05;
static const double JUMP_SPEED = 0.6;
static const uint32_t JUMP_MILLIS = 750;
static const double GRAVITY = -0.6;

static int32_t level;
static bool trap_set;
static bool conveyor_belt;
static double CONVEYOR_BELT_SPEED = 0.4;

static cog_pos2 START_POS = (cog_pos2) { .x=-0.9, .y=-0.35 };

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
    cog_clear();
    cog_list_removeall(&platforms);
    cog_list_removeall(&traps);
    cog_list_init(&platforms, sizeof(platform));
    cog_list_init(&traps, sizeof(trap));
    trap_set = false;
    //p.sid = cog_rect_add("../assets/c0.png");
    // Player
    p.sid = cog_rect_add();
    cog_rect_set(p.sid, (cog_rect) {
            .pos=START_POS,
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

    if (level >= 4 && level < 6) {
        if(level == 5) {
            CONVEYOR_BELT_SPEED = 0.2;
        }
        p.s->vel.x = CONVEYOR_BELT_SPEED;
        conveyor_belt = true;
    } else {
        conveyor_belt = false;
    }

    // Platform
    cog_rect_id pl_0 = cog_rect_add();
    cog_rect_set(pl_0, (cog_rect) {
            .dim=(cog_dim2) {
            .w=1.0, .h=PLATFORM_HEIGHT
            },
            .rot=COG_PI/2,
            .pos=(cog_pos2) {
            .x=0.0, .y=-0.5
            },
            .col=(cog_color) {
            .r=1,.g=1,.b=1,.a=1
            }
            });

    cog_list_append(&platforms, &(platform) {
            .sid=pl_0
            });

    if(level == 0) {
        // Flag
        f.sid = cog_sprite_add("../assets/flags/turkey.png");
        double w = 0.54;
        cog_sprite_set(f.sid, (cog_sprite) {
                .dim=(cog_dim2) {
                .w=w, .h=w * 0.6
                },
                .pos=(cog_pos2) {
                .x=-0.4, .y=0.5
                }
                });
        cog_sprite* s = cog_sprite_get(f.sid);
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
        cog_rect_id pl_0 = cog_rect_add();
        cog_rect_set(pl_0, (cog_rect) {
                .dim=(cog_dim2) {
                .w=0.4, .h=PLATFORM_HEIGHT
                },
                .rot=COG_PI/2,
                .pos=(cog_pos2) {
                .x=-0.6, .y=-0.5
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
                .w=0.4, .h=PLATFORM_HEIGHT
                },
                .rot=COG_PI/2,
                .pos=(cog_pos2) {
                .x=0.6, .y=-0.5
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

        cog_rect_id trap_0 = cog_rect_add();
        cog_rect_set(trap_0, (cog_rect) {
                .dim=(cog_dim2) {
                .w=0.15, .h=PLATFORM_HEIGHT*2.0
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
                .sid=trap_0
                });

    } else if(level == 1) {
        // Flag
        f.sid = cog_sprite_add("../assets/flags/greece.png");
        double w = 0.54;
        cog_sprite_set(f.sid, (cog_sprite) {
                .dim=(cog_dim2) {
                .w=w, .h=w * 0.6
                },
                .pos=(cog_pos2) {
                .x=-0.4, .y=0.5
                }
                });
        cog_sprite* s = cog_sprite_get(f.sid);
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
                .w=0.6, .h=PLATFORM_HEIGHT
                },
                .rot=COG_PI/2,
                .pos=(cog_pos2) {
                .x=0.4, .y=-0.5
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
        cog_rect_id trap_0 = cog_rect_add();
        cog_rect_set(trap_0, (cog_rect) {
                .dim=(cog_dim2) {
                .w=0.20, .h=PLATFORM_HEIGHT*5.0
                },
                .rot=COG_PI/2,
                .pos=(cog_pos2) {
                .x=(pl_0_r->pos.x) + 0.4, .y=-0.5
                },
                .col=(cog_color) {
                .r=1,.g=0,.b=0,.a=1
                }
                });
        cog_list_append(&traps, &(trap) {
                .sid=trap_0
                });
    } else if(level == 2) {
        // Flag
        f.sid = cog_sprite_add("../assets/flags/macedonia.png");
        double w = 0.54;
        cog_sprite_set(f.sid, (cog_sprite) {
                .dim=(cog_dim2) {
                .w=w, .h=w * 0.6
                },
                .pos=(cog_pos2) {
                .x=-0.4, .y=0.5
                }
                });
        cog_sprite* s = cog_sprite_get(f.sid);
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

        cog_rect_id pl_2 = cog_rect_add();
        cog_rect_set(pl_2, (cog_rect) {
                .dim=(cog_dim2) {
                .w=0.1, .h=PLATFORM_HEIGHT
                },
                .rot=COG_PI/2,
                .pos=(cog_pos2) {
                .x=0.9, .y=-0.5
                },
                .col=(cog_color) {
                .r=1,.g=1,.b=1,.a=1
                }
                });
        cog_list_append(&platforms, &(platform) {
                .sid=pl_2
                });


        cog_rect* pl_0_r = cog_rect_get(pl_0);
        cog_rect* pl_1_r = cog_rect_get(pl_1);

        // Death traps
        cog_rect_id trap_0 = cog_rect_add();
        cog_rect_set(trap_0, (cog_rect) {
                .dim=(cog_dim2) {
                .w=0.35, .h=PLATFORM_HEIGHT*2.0
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
                .sid=trap_0
                });

        cog_rect_id trap_1 = cog_rect_add();
        cog_rect_set(trap_1, (cog_rect) {
                .dim=(cog_dim2) {
                .w=0.20, .h=PLATFORM_HEIGHT*5.0
                },
                .rot=COG_PI/2,
                .pos=(cog_pos2) {
                .x=(pl_1_r->pos.x) + 0.4, .y=-0.5
                },
                .col=(cog_color) {
                .r=1,.g=0,.b=0,.a=1
                }
                });
        cog_list_append(&traps, &(trap) {
                .sid=trap_1
                });
    } else if(level == 3) {
        // Flag
        f.sid = cog_sprite_add("../assets/flags/serbia.png");
        double w = 0.54;
        cog_sprite_set(f.sid, (cog_sprite) {
                .dim=(cog_dim2) {
                .w=w, .h=w * 0.6
                },
                .pos=(cog_pos2) {
                .x=-0.4, .y=0.5
                }
                });
        cog_sprite* s = cog_sprite_get(f.sid);
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

        cog_rect_id pl_2 = cog_rect_add();
        cog_rect_set(pl_2, (cog_rect) {
                .dim=(cog_dim2) {
                .w=0.1, .h=PLATFORM_HEIGHT
                },
                .rot=COG_PI/2,
                .pos=(cog_pos2) {
                .x=0.9, .y=-0.5
                },
                .col=(cog_color) {
                .r=1,.g=1,.b=1,.a=1
                }
                });
        cog_list_append(&platforms, &(platform) {
                .sid=pl_2
                });


        cog_rect* pl_0_r = cog_rect_get(pl_0);
        cog_rect* pl_1_r = cog_rect_get(pl_1);

        // Death traps
        for(int i=0;i < 5;i++) {
            cog_rect_id t = cog_rect_add();
            cog_rect_set(t, (cog_rect) {
                    .dim=(cog_dim2) {
                    .w=0.05, .h=PLATFORM_HEIGHT*6.0
                    },
                    .rot=COG_PI/2,
                    .pos=(cog_pos2) {
                    .x=(pl_0_r->pos.x) + (i+1) * 0.3, .y=-0.5
                    },
                    .col=(cog_color) {
                    .r=1,.g=0,.b=0,.a=1
                    }
                    });
            cog_list_append(&traps, &(trap) {
                    .sid=t
                    });
        }
    } else if(level == 4) {
        // Flag
        f.sid = cog_sprite_add("../assets/flags/hungary.png");
        double w = 0.54;
        cog_sprite_set(f.sid, (cog_sprite) {
                .dim=(cog_dim2) {
                .w=w, .h=w * 0.6
                },
                .pos=(cog_pos2) {
                .x=-0.4, .y=0.5
                }
                });
        cog_sprite* s = cog_sprite_get(f.sid);
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

        cog_rect_id pl_2 = cog_rect_add();
        cog_rect_set(pl_2, (cog_rect) {
                .dim=(cog_dim2) {
                .w=0.1, .h=PLATFORM_HEIGHT
                },
                .rot=COG_PI/2,
                .pos=(cog_pos2) {
                .x=0.9, .y=-0.5
                },
                .col=(cog_color) {
                .r=1,.g=1,.b=1,.a=1
                }
                });
        cog_list_append(&platforms, &(platform) {
                .sid=pl_2
                });


        cog_rect* pl_0_r = cog_rect_get(pl_0);
        cog_rect* pl_1_r = cog_rect_get(pl_1);

        // Death traps
        for(int i=0;i < 2;i++) {
            cog_rect_id t = cog_rect_add();
            cog_rect_set(t, (cog_rect) {
                    .dim=(cog_dim2) {
                    .w=0.05, .h=PLATFORM_HEIGHT*6.0
                    },
                    .rot=COG_PI/2,
                    .pos=(cog_pos2) {
                    .x=(pl_0_r->pos.x) + (i+1) * 0.6, .y=-0.5
                    },
                    .col=(cog_color) {
                    .r=1,.g=0,.b=0,.a=1
                    }
                    });
            cog_list_append(&traps, &(trap) {
                    .sid=t
                    });
        }
    } else if(level == 5) {
        // Flag
        f.sid = cog_sprite_add("../assets/flags/austria.png");
        double w = 0.54;
        cog_sprite_set(f.sid, (cog_sprite) {
                .dim=(cog_dim2) {
                .w=w, .h=w * 0.6
                },
                .pos=(cog_pos2) {
                .x=-0.4, .y=0.5
                }
                });
        cog_sprite* s = cog_sprite_get(f.sid);
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

        cog_rect_id pl_2 = cog_rect_add();
        cog_rect_set(pl_2, (cog_rect) {
                .dim=(cog_dim2) {
                .w=0.1, .h=PLATFORM_HEIGHT
                },
                .rot=COG_PI/2,
                .pos=(cog_pos2) {
                .x=0.9, .y=-0.5
                },
                .col=(cog_color) {
                .r=1,.g=1,.b=1,.a=1
                }
                });
        cog_list_append(&platforms, &(platform) {
                .sid=pl_2
                });


        cog_rect* pl_0_r = cog_rect_get(pl_0);
        cog_rect* pl_1_r = cog_rect_get(pl_1);

        // Death traps
        for(int i=0;i < 5;i++) {
            if(i == 2) continue;
            cog_rect_id t = cog_rect_add();
            cog_rect_set(t, (cog_rect) {
                    .dim=(cog_dim2) {
                    .w=0.01, .h=PLATFORM_HEIGHT*2.0
                    },
                    .rot=COG_PI/2,
                    .pos=(cog_pos2) {
                    .x=(pl_0_r->pos.x) + (i+1) * 0.3, .y=-0.5
                    },
                    .col=(cog_color) {
                    .r=1,.g=0,.b=0,.a=1
                    }
                    });
            cog_list_append(&traps, &(trap) {
                    .sid=t
                    });
        }
    } else if(level == 6) {
        // Flag
        f.sid = cog_sprite_add("../assets/flags/germany.png");
        double w = 0.54;
        cog_sprite_set(f.sid, (cog_sprite) {
                .dim=(cog_dim2) {
                .w=w, .h=w * 0.6
                },
                .pos=(cog_pos2) {
                .x=-0.4, .y=0.5
                }
                });
        cog_sprite* s = cog_sprite_get(f.sid);
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
        cog_rect_id pl_0 = cog_rect_add();
        cog_rect_set(pl_0, (cog_rect) {
                .dim=(cog_dim2) {
                .w=1.0, .h=PLATFORM_HEIGHT
                },
                .rot=COG_PI/2,
                .pos=(cog_pos2) {
                .x=0.0, .y=-0.5
                },
                .col=(cog_color) {
                .r=1,.g=1,.b=1,.a=1
                }
                });

        cog_list_append(&platforms, &(platform) {
                .sid=pl_0
                });


        // Death traps
        cog_rect_id trap_0 = cog_rect_add();
        cog_rect_set(trap_0, (cog_rect) {
                .dim=(cog_dim2) {
                .w=0.35, .h=1.0
                },
                .rot=COG_PI/2,
                .pos=(cog_pos2) {
                .x=0.8, .y=0.0
                },
                .col=(cog_color) {
                .r=1,.g=0,.b=0,.a=1
                }
                });
        cog_list_append(&traps, &(trap) {
                .sid=trap_0
                });
    }
}

void update() {
}

void render() {
/*
    COG_LIST_FOREACH(&traps) {
        trap* t = (trap*)curr->data;
        cog_rect* trap_rect = cog_rect_get(t->sid);
        glBegin(GL_LINES);
            glVertex2f(trap_rect->pos.x,trap_rect->pos.y);
            glVertex2f(p.s->pos.x,p.s->pos.y);
        glEnd();
    }
*/
}

int main(int argc, char* argv[]) {
    cog_init(.fullscreen = false, .debug=false, .window_w=800, .window_h=600);
    cog_list_init(&platforms, sizeof(platform));
    cog_list_init(&traps, sizeof(trap));

    bool jumping = false;
    bool moving = false;
    uint32_t jump_time = 0;

    init();

    cog_graphics_set_custom_render(render);
    while(!cog_hasquit()) {
        update();
        cog_loopstep();

        if(level == 6 && p.s->pos.x > -0.3 && !trap_set) {
            trap_set = true;
            cog_rect_id trap_1 = cog_rect_add();
            cog_rect_set(trap_1, (cog_rect) {
                    .dim=(cog_dim2) {
                    .w=0.1, .h=1.0
                    },
                    .rot=COG_PI/2,
                    .pos=(cog_pos2) {
                    .x=-0.9, .y=0.0
                    },
                    .col=(cog_color) {
                    .r=1,.g=0,.b=0,.a=1
                    }
                    });
            cog_list_append(&traps, &(trap) {
                    .sid=trap_1
                    });

            cog_text_id id = cog_text_add();
            cog_text_set(id, (cog_text) {
                .scale = (cog_dim2) {.w=0.003, .h=0.003},
                .dim = (cog_dim2) {.w=2.0, .h=0.003},
                .pos = (cog_pos2) {.x=-1, .y=-1},
                .col=(cog_color) {
                    .r=1,.g=1,.b=1,.a=1
                }
            });
            cog_text_set_str(id, "Press r to restart.");
        }

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
                if(!conveyor_belt) {
                    if(p.s->vel.x > 0.001) p.s->vel.x = X_SPEED_AIR;
                    if(p.s->vel.x < -0.001) p.s->vel.x = -X_SPEED_AIR;
                }
            }
            if(key == 'r') {
                level = 0;
                init();
            }
            // Cannot move left and right on those levels
            if (! conveyor_belt) {
                if(key == 'a') {
                    p.s->vel.x = jumping ? -X_SPEED_AIR : -X_SPEED;
                    moving = true;
                }
                if(key == 'd') {
                    p.s->vel.x = jumping ? X_SPEED_AIR : X_SPEED;
                    moving = true;
                }
            }
            if(key == 'n') {
                level++;
                init();
            }
            if(key == 'p') {
                level--;
                init();
            }
        } 
        if(cog_input_key_depressed()) {
            uint32_t key = cog_input_key_code_depressed();
            if(key == 'a' || key == 'd') {
                moving = false;
            }
        }
        if(!moving) {
            if(!conveyor_belt) {
                if(fabs(p.s->vel.x) > 0.01) {
                    if(p.s->vel.x > 0.0) p.s->vel.x -= X_FRICTION;
                    else p.s->vel.x += X_FRICTION;
                } else {
                    p.s->vel.x = 0.0;
                }
            }
        }
        
        COG_LIST_FOREACH(&traps) {
            trap* t = (trap*)curr->data;
            if(cog_rect_collides_rect(t->sid, p.sid)) {
                p.s->pos = START_POS; 
                p.s->vel = (cog_vec2){.x=0,.y=0}; 
                if(conveyor_belt) {
                    p.s->vel.x = CONVEYOR_BELT_SPEED;
                }
                //init();
            }
        }
        // Win condition
        if(p.s->pos.x > 1.0) {
            //cog_clear();
            level++;
            init();
        }
    }
}
