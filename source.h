#include <stdio.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"

#include "pico/stdlib.h"

// Setup LED pin mappings

#ifndef PICO_DEFAULT_LED_PIN
    #warning Default PICO LED pin not set
#endif

#ifndef BUTTON_PIN
    #define BUTTON_PIN 13
#endif

#ifndef PURPLE_LED_PIN
    #define PURPLE_LED_PIN 14
#endif
