#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "bsp/board.h"
#include "tusb.h"

#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "source.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

const uint BUTTON_GPIO_PIN = BUTTON_PIN;
const uint PURPLE_GPIO_PIN = PURPLE_LED_PIN;
const uint VOLUME_GPIO_PIN = VOLUME_CTRL_PIN;

const uint8_t MIDI_CABLE = 0;
const uint8_t MIDI_CHANNEL = 0;
// Defined as "General Purpose Controller 1" in the MIDI Spec
const uint8_t MIDI_CC_CONTROLLER = 0x10;

void led_blinking_task(void);
void midi_task(void);
void cdc_task(void);

void init_app_gpio(void);

void update_button_midi(void);
void update_dial_midi(void);

//--------------------------------------------------------------------+
// Main
//--------------------------------------------------------------------+

int main() {
    board_init();
    stdio_init_all();
    tusb_init();
    init_app_gpio();

    while (true) {
        tud_task();
        led_blinking_task();
        midi_task();
        cdc_task();

        // For debugging, light an LED when the button is pushed
        gpio_put(PURPLE_GPIO_PIN, gpio_get(BUTTON_GPIO_PIN));
    }
    return 0;
}

// Initalise the GPIO pins we need
void init_app_gpio(void) {
    adc_init();

    gpio_init(BUTTON_GPIO_PIN);
    gpio_set_dir(BUTTON_GPIO_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_GPIO_PIN);

    gpio_init(PURPLE_GPIO_PIN);
    gpio_set_dir(PURPLE_GPIO_PIN, GPIO_OUT);

    adc_gpio_init(VOLUME_GPIO_PIN);
    adc_select_input(0);
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

void midi_task(void) {
    static uint32_t start_ms = 0;

    // Read and discard all MIDI input data
    uint8_t midi_in_message[4];
    while ( tud_midi_available() ) tud_midi_packet_read(midi_in_message);

    // Poll peripherals every 50ms
    if (board_millis() - start_ms < 50) return; // not enough time
    start_ms += 50;

    update_button_midi();
    update_dial_midi();
}

// Poll the button and send a midi message if the value has changed
void update_button_midi(void) {
    static bool button_last_value = false;
    uint8_t midi_out_message[3];

    bool button_current_value = gpio_get(BUTTON_GPIO_PIN);
    if (button_current_value == button_last_value) return; // No change

    if (button_current_value) {
        midi_out_message[0] = NOTE_ON_MIDI | MIDI_CHANNEL; // Note on message
    } else {
        midi_out_message[0] = NOTE_OFF_MIDI | MIDI_CHANNEL; // Note off message
    }

    midi_out_message[1] = SNARE_MIDI_NOTE;   // Note Number
    midi_out_message[2] = 127;               // Velocity

    tud_midi_n_stream_write(
        ITF_NUM_MIDI,
        MIDI_CABLE,
        midi_out_message,
        sizeof(midi_out_message)
    );

    printf("Sent MIDI packet\n");

    button_last_value = button_current_value;
}

// Poll the dial (analogue input) and send a MIDI message if the
// value has changed
void update_dial_midi(void) {
    static uint16_t volume_last_value = 0;
    uint8_t midi_out_message[3];

    uint16_t volume_current_value = adc_read() >> 5;

    if (volume_last_value == volume_current_value) return; // No change

    midi_out_message[0] = CONTROL_CHANGE_MIDI | MIDI_CHANNEL;    // CC Message
    midi_out_message[1] = MIDI_CC_CONTROLLER;                    // CC Controller
    midi_out_message[2] = volume_current_value;                  // Value

    tud_midi_n_stream_write(
        ITF_NUM_MIDI,
        MIDI_CABLE,
        midi_out_message,
        sizeof(midi_out_message)
    );

    volume_last_value = volume_current_value;
}

//--------------------------------------------------------------------+
// CDC Task
//--------------------------------------------------------------------+

void cdc_task(void)
{
  // connected() check for DTR bit
  // Most but not all terminal client set this when making connection
  // if ( tud_cdc_connected() )
  uint8_t const case_diff = 'a' - 'A';

  {
    // connected and there are data available
    if ( tud_cdc_available() )
    {
      // Read data
      char buf[64];
      uint32_t count = tud_cdc_read(buf, sizeof(buf));

      // Echo back as uppercase
      for (uint32_t i = 0; i < count; i++) {
        if (islower(buf[i])) buf[i] -= case_diff;
        tud_cdc_write_char(buf[i]);
      }

      tud_cdc_write_flush();
    }
  }
}

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
  (void) itf;
  (void) rts;

  // TODO set some indicator
  if ( dtr )
  {
    // Terminal connected
  }else
  {
    // Terminal disconnected
  }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf)
{
  (void) itf;
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
    led_state = 1 - led_state; // Toggle LED
}