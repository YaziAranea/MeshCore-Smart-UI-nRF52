#pragma once
#include <cstdint> // For uint8_t, uint32_t

#define TELEM_MODE_DENY            0
#define TELEM_MODE_ALLOW_FLAGS     1     // use contact.flags
#define TELEM_MODE_ALLOW_ALL       2

#define ADVERT_LOC_NONE       0
#define ADVERT_LOC_SHARE      1

#define GPS_SOURCE_HW         0
#define GPS_SOURCE_PHONE      1

#define NOTIFY_MODE_SILENT    0x00
#define NOTIFY_MODE_GPIO      0x01
#define NOTIFY_MODE_TONE      0x02
#define NOTIFY_MODE_VIBE      0x04
#define NOTIFY_MODE_ALL       (NOTIFY_MODE_GPIO | NOTIFY_MODE_TONE | NOTIFY_MODE_VIBE)

#define NOTIFY_TONE_COUNT     31
#define DEFAULT_NOTIFY_TONE_RESONANCE_HZ 3000

#define SMART_PROFILE_CUSTOM   0
#define SMART_PROFILE_QUIET    1
#define SMART_PROFILE_OUTDOOR  2
#define SMART_PROFILE_NIGHT    3

#define SMART_FAVORITE_NOTIFY_MODE       1
#define SMART_FAVORITE_IMPORTANT_NOTIFY  2
#define SMART_FAVORITE_SYSTEM_TONE       3
#define SMART_FAVORITE_DM_TONE           4
#define SMART_FAVORITE_MENTION_TONE      5
#define SMART_FAVORITE_UI_FONT           6
#define SMART_FAVORITE_UI_THEME          7
#define SMART_FAVORITE_BLUETOOTH         8
#define SMART_FAVORITE_AUTO_ADVERT       9
#define SMART_FAVORITE_GPS              10
#define SMART_FAVORITE_BOARD_LEDS       11
#define SMART_FAVORITE_LOW_BATTERY      12
#define SMART_FAVORITE_ADC              13
#define SMART_FAVORITE_PROFILE          14
#define SMART_FAVORITE_MAX              SMART_FAVORITE_PROFILE

#define CH2_MODE_OFF          0
#define CH2_MODE_RELAY        1
#define CH2_MODE_LISTEN       2
#define CH2_MODE_BATCH        3

struct NodePrefs {  // persisted to file
  float airtime_factor;
  char node_name[32];
  float freq;
  uint8_t sf;
  uint8_t cr;
  uint8_t multi_acks;
  uint8_t manual_add_contacts;
  float bw;
  int8_t tx_power_dbm;
  uint8_t telemetry_mode_base;
  uint8_t telemetry_mode_loc;
  uint8_t telemetry_mode_env;
  float rx_delay_base;
  uint32_t ble_pin;
  uint8_t  advert_loc_policy;
  uint8_t  buzzer_quiet;
  uint8_t  gps_enabled;      // GPS enabled flag (0=disabled, 1=enabled)
  uint32_t gps_interval;     // GPS read interval in seconds
  uint8_t autoadd_config;    // bitmask for auto-add contacts config
  uint8_t rx_boosted_gain; // SX126x RX boosted gain mode (0=power saving, 1=boosted)
  uint8_t radio_fem_rxgain;  // LoRa FEM RX gain setting
  uint8_t client_repeat;
  uint8_t path_hash_mode;    // which path mode to use when sending
  uint8_t autoadd_max_hops;  // 0 = no limit, 1 = direct (0 hops), N = up to N-1 hops (max 64)
  char default_scope_name[31];
  uint8_t default_scope_key[16];
  float adc_multiplier;      // 0 = default board ADC multiplier
  uint8_t notify_mode;        // NOTIFY_MODE_* bitmask for message alerts
  int8_t notify_gpio_pin;      // runtime-selected LED alert GPIO, -1 = build default/disabled
  int8_t notify_tone_pin;      // runtime-selected tone/buzzer GPIO, -1 = build default/disabled
  uint8_t notify_tone_id;      // selected message tone/melody
  uint8_t notify_tone_volume;  // message tone volume, 1..10
  uint16_t auto_advert_interval_mins; // automatic self-advert interval, 0 = disabled
  uint8_t ch2_mode;            // CH2_MODE_* experimental local channel bridge
  uint8_t board_leds_enabled;   // onboard/status/TX LEDs, 0=off, 1=on
  uint8_t ui_font;              // display font profile, 0=default
  uint8_t ui_theme;             // display color/background theme, 0=default
  uint8_t unread_led_enabled;    // status LED long blink for unread messages, 0=off, 1=on
  uint8_t msg_popup_enabled;     // auto-open unread message preview, 0=off, 1=on
  uint8_t important_notify_mode; // NOTIFY_MODE_* for DMs and channel mentions
  uint8_t notifications_muted;    // global UI notification mute, 0=off, 1=on
  uint8_t ui_top_color;           // semantic color index for top/accent text
  uint8_t ui_bottom_color;        // semantic color index for lower/detail text
  uint8_t backlight_timeout_idx;   // 0=15s, 1=30s, 2=60s
  int8_t notify_vibe_pin;          // runtime-selected vibration GPIO, -1 = disabled/unset
  uint8_t offline_dm_led_enabled;  // important DM LED alert while BLE is disconnected, 0=off, 1=on
  uint8_t ble_dm_led_enabled;      // important DM LED alert while BLE is connected, 0=off, 1=on
  uint8_t low_battery_shutdown_enabled; // low battery auto-shutdown protection, 0=off, 1=on
  uint8_t notify_tone_bridge_enabled;   // passive piezo bridge drive, 0=GPIO-GND, 1=GPIO-GPIO
  uint8_t notify_tone_8bit_enabled;     // chiptune arpeggio renderer, 0=normal, 1=8-bit
  uint8_t notify_tone_high_drive_enabled; // nRF GPIO drive, 0=S0S1, 1=H0H1
  uint16_t notify_tone_resonance_hz;    // passive piezo resonance target
  uint8_t notify_tone_dm_id;            // melody for direct messages
  uint8_t notify_tone_mention_id;       // melody for mentions
  uint8_t notify_tone_system_id;        // melody for regular/system notifications
  uint8_t smart_profile_id;             // SMART_PROFILE_*
  uint8_t favorite_setting_1;           // SMART_FAVORITE_*
  uint8_t favorite_setting_2;           // SMART_FAVORITE_*
  uint8_t favorite_setting_3;           // SMART_FAVORITE_*
  uint32_t night_prompt_day;            // local epoch day when the 23:30 prompt was shown
  uint8_t night_quiet_active;           // mute was enabled by the nightly prompt, not manually
  uint8_t gps_source;                    // GPS_SOURCE_*; PHONE means BLE-fed phone location
};
