#include <bn_core.h>
#include <bn_backdrop.h>
#include <bn_keypad.h>
#include <bn_sprite_ptr.h>
#include <bn_display.h>
#include <bn_random.h>
#include <bn_vector.h>
#include <bn_log.h>

#include "bn_sprite_items_dot.h"

// Set max/min x position to be the edges of the display
static constexpr int HALF_SCREEN_WIDTH = bn::display::width() / 2;
static constexpr bn::fixed MIN_X = -HALF_SCREEN_WIDTH;
static constexpr bn::fixed MAX_X = HALF_SCREEN_WIDTH;
static constexpr int HALF_SCREEN_HEIGHT = bn::display::height() / 2;
static constexpr bn::fixed MIN_Y = -HALF_SCREEN_HEIGHT;
static constexpr bn::fixed MAX_Y = HALF_SCREEN_HEIGHT;

// Starting speed of a bouncer
static constexpr bn::fixed BASE_SPEED = 2;

// Maximum number of bouncers on screen at once
static constexpr int MAX_BOUNCERS = 20;

class Bouncer {
    public:
        bn::sprite_ptr sprite = bn::sprite_items::dot.create_sprite();
        bn::fixed x_speed = BASE_SPEED;
        bn::fixed y_speed = BASE_SPEED;

        // Timer to control flashing of the sprite
        int flash_timer = 0;
        int flash_frame = 30;

        // Random sprite generator
        Bouncer(bn::random& random) {
            sprite.set_x(random.get_int(int(MIN_X), int(MAX_X)));
            sprite.set_y(random.get_int(int(MIN_Y), int(MAX_Y)));

            x_speed = random.get_fixed(0.5, 1.5);
            y_speed = random.get_fixed(0.5, 1.5);
            
            if(random.get_int(100) < 50) x_speed *= -1;
            if(random.get_int(100) < 50) y_speed *= -1;

            flash_timer = random.get_int(30, 60);
        }

        void update() {
            bn::fixed x = sprite.x();
            bn::fixed y = sprite.y();


            // Update x position by adding speed
            x += x_speed;
            y += y_speed;

            // If we've gone off the screen on the right
            if(x > MAX_X) {
                // Snap back to screen and reverse direction
                x = MAX_X;
                x_speed *=-1;
            }
            // If we've gone off the screen on the left
            if(x < MIN_X) {
                // Snap back to screen and reverse direction
                x = MIN_X;
                x_speed *= -1;
            }
            // If we've gone off the screen on the bottom
            if(y > MAX_Y) {
                // Snap back to screen and reverse direction
                y = MAX_Y;
                y_speed *= -1;
            }
            // If we've gone off the screen on the top
            if(y < MIN_Y) {
                // Snap back to screen and reverse direction
                y = MIN_Y;
                y_speed *= -1;
            }

            flash_timer++;
            if(flash_timer >= flash_frame) {
                flash_timer = 0;
                sprite.set_visible(!sprite.visible());
            }

            sprite.set_x(x);
            sprite.set_y(y);
        }
};

bn::fixed average_x(bn::vector<Bouncer, MAX_BOUNCERS>& bouncers) {
    // Add all x positions together
    bn::fixed x_sum = 0;
    for(Bouncer& bouncer : bouncers) {
        x_sum += bouncer.sprite.x();
    }
    
    bn::fixed x_average = 0;
    
    // Only divide if we have 1 or more
    // Prevents division by 0
    if(!bouncers.empty()) {
        x_average = x_sum / bouncers.size();
    }

    return x_average;
}

bn::fixed average_y(bn::vector<Bouncer, MAX_BOUNCERS>& bouncers) {
    // Add all y positions together
    bn::fixed y_sum = 0;
    for(Bouncer& bouncer : bouncers) {
        y_sum += bouncer.sprite.y();
    }

    bn::fixed y_average = 0;

    if(!bouncers.empty()) {
        y_average = y_sum / bouncers.size();
    }

    return y_average;
}

void add_bouncer(bn::vector<Bouncer, MAX_BOUNCERS>& bouncers, bn::random& random) {
    // Only add if we're below the maximum
    if(bouncers.size() < bouncers.max_size()) {
        bouncers.push_back(Bouncer(random));
    }
}

int main() {
    bn::core::init();

    bn::vector<Bouncer, MAX_BOUNCERS> bouncers = {};
    bn::random random;

    while(true) {
        random.update();
        // if A is pressed add a new bouncer
        if(bn::keypad::a_pressed()) {
            add_bouncer(bouncers, random);
        }

        if(bn::keypad::b_pressed()) {
            BN_LOG("Average x: ", average_x(bouncers));
            BN_LOG("Average y: ", average_y(bouncers));
        }

        // for each bouncer
        for(Bouncer& bouncer : bouncers) {
            bouncer.update();
        }

        bn::core::update();
    }
}