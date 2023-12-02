#ifndef __COMMON_H__
#define __COMMON_H__

#include "pico/stdlib.h"

//--------------------------------------------------------------------+
// GPIO Mapping definitions
//--------------------------------------------------------------------+

#ifndef PICO_DEFAULT_LED_PIN
    #warning Default PICO LED pin not set
#endif

#define BUTTON_PIN 13
#define PURPLE_LED_PIN 14
#define VOLUME_CTRL_PIN 26

//--------------------------------------------------------------------+
// Button Status Light Definitions
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};

//--------------------------------------------------------------------+
// MIDI Definitions
//--------------------------------------------------------------------+

enum {
    SNARE_MIDI_NOTE = 6,
    HIHAT_MIDI_NOTE = 11,
    KICK_MIDI_NOTE = 34,
};

enum {
    CONTROL_CHANGE_MIDI = 0xB0,
    NOTE_ON_MIDI = 0x90,
    NOTE_OFF_MIDI = 0x80,
};

//--------------------------------------------------------------------+
// USB Interface Definitions
//--------------------------------------------------------------------+

enum
{
  ITF_NUM_MIDI = 0,
  ITF_NUM_MIDI_STREAMING,
  ITF_NUM_CDC,
  ITF_NUM_CDC_DATA,
  ITF_NUM_TOTAL
};
#endif