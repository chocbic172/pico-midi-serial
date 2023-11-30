#include "source.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
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

enum {
    SNARE_MIDI_NOTE = 6,
    HIHAT_MIDI_NOTE = 11,
    KICK_MIDI_NOTE = 34,
};

enum {
    NOTE_ON_MIDI = 0x90,
    NOTE_OFF_MIDI = 0x80,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

const uint BUTTON_GPIO_PIN = BUTTON_PIN;
const uint PURPLE_GPIO_PIN = PURPLE_LED_PIN;

void led_blinking_task(void);
void midi_task(void);

//--------------------------------------------------------------------+
// Main
//--------------------------------------------------------------------+

int main() {
    board_init();
    tusb_init();
    stdio_init_all();

    gpio_init(BUTTON_GPIO_PIN);
    gpio_set_dir(BUTTON_GPIO_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_GPIO_PIN);

    gpio_init(PURPLE_GPIO_PIN);
    gpio_set_dir(PURPLE_GPIO_PIN, GPIO_OUT);

    while (true) {
        tud_task();
        led_blinking_task();
        midi_task();

        if (gpio_get(BUTTON_GPIO_PIN)) {
            gpio_put(PURPLE_GPIO_PIN, 1);
        } else {
            gpio_put(PURPLE_GPIO_PIN, 0);
        }
    }
    return 0;
}

//--------------------------------------------------------------------+
// Device Callbacks
//--------------------------------------------------------------------+

// Invoked on device mount
void tud_mount_cb(void) {
    blink_interval_ms = BLINK_MOUNTED;
}

// Invoked on device unmount
void tud_unmount_cb(void) {
    blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked on usb bus suspend
void tud_suspend_cb(bool remote_wakeup_en) {
    (void) remote_wakeup_en;
    blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked on usb bus resume
void tud_resume_cb(void) {
    blink_interval_ms = BLINK_MOUNTED;
}

//--------------------------------------------------------------------+
// MIDI Task
//--------------------------------------------------------------------+

static bool button_last_value = false;

void midi_task(void) {
    static uint32_t start_ms = 0;
    uint8_t msg[3];

    // Read and discard all MIDI input data
    uint8_t packet[4];
    while ( tud_midi_available() ) tud_midi_packet_read(packet);

    // Poll button every 50 ms
    if (board_millis() - start_ms < 50) return; // not enough time
    start_ms += 50;

    bool button_current_value = gpio_get(BUTTON_GPIO_PIN);
    if (button_current_value == button_last_value) return;

    if (button_current_value) {
        // Send Note On for current position at full velocity (127) on channel 1
        msg[0] = NOTE_ON_MIDI;            // Note On - Channel 1
        msg[1] = SNARE_MIDI_NOTE;         // Note Number
        msg[2] = 127;                     // Velocity
        tud_midi_n_stream_write(0, 0, msg, 3);
        printf("Sent Snare Hit\n");
    }

    button_last_value = button_current_value;
}

//--------------------------------------------------------------------+
// Blinking Task
//--------------------------------------------------------------------+
void led_blinking_task(void) {
    static uint32_t start_ms = 0;
    static bool led_state = false;

    // Blink every interval ms
    if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
    start_ms += blink_interval_ms;

    board_led_write(led_state);
    led_state = 1 - led_state; // toggle
}