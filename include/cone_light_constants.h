#pragma once

#include <Arduino.h>

static constexpr bool CONE_LIGHT_DEBUG = true;
static constexpr bool CONE_LIGHT_DEBUG_WITH_SILENT_SPEAKER = false;

static constexpr bool CONE_LIGHT_ALLOW_PHOTOSENSITIVE_HAZARDS = true;

static const char *CONE_LIGHT_PRODUCT_NAME = "Cone Light mk. II";
// Whether buttons cause beeps and chirps
static constexpr bool CONE_LIGHT_MUTED = false;
// Play 4 notes on boot up
static constexpr bool CONE_LIGHT_BOOT_UP_TUNE = false;
// Whether node's attempt to sync with the grand master clock node
static constexpr bool CONE_LIGHT_GRAND_MASTER_CLOCK_ENABLED = false;

static constexpr uint8_t CONE_LIGHT_NODE_ID_UNSET = 255;
static constexpr uint8_t CONE_LIGHT_NODE_GROUP_ID_UNSET = 255;
static const char *CONE_LIGHT_NODE_NAME_UNSET = "NONAME";
static constexpr bool CONE_LIGHT_NODE_GRAND_MASTER_CLOCK_UNSET_OR_FALSE = false;

static constexpr uint8_t CONE_LIGHT_NODE_GROUP_0 = 0;
static const char *CONE_LIGHT_NODE_GROUP_0_NAME = "BACKSTOP";
static constexpr uint8_t CONE_LIGHT_NODE_GROUP_1 = 1;
static const char *CONE_LIGHT_NODE_GROUP_1_NAME = "STOPSIGN";
static constexpr uint8_t CONE_LIGHT_NODE_GROUP_255 = 255;
static const char *CONE_LIGHT_NODE_GROUP_255_NAME = "!NOT SET";

static const char *CONE_LIGHT_PREFERENCES_ID = "cone_light";
static const char *CONE_LIGHT_PREFERENCES_NODE_ID = "node_id";
static const char *CONE_LIGHT_PREFERENCES_NODE_GROUP = "node_group";
static const char *CONE_LIGHT_PREFERENCES_NODE_NAME = "node_name";
static const char *CONE_LIGHT_PREFERENCES_NODE_GRANDMASTER_CLOCK = "node_clock";

static const char *CONE_LIGHT_NETWORKING_PROTOCOL_ID = "CONE";
static constexpr uint8_t CONE_LIGHT_NETWORKING_MAX_NODES = 12;

// Updated whenever changes are made. YYYY.MM.DD
static const char *CONE_LIGHT_FIRMWARE_VERSION_NAME = "2025.11.23";
// Used for backward incompatible changes
static constexpr uint8_t CONE_LIGHT_FIRMWARE_VERSION = 3;

static constexpr uint8_t BTN_A_PIN = D10;
static constexpr uint8_t BTN_B_PIN = D9;
static constexpr uint8_t BTN_C_PIN = D8;
static constexpr uint8_t SPEAKER_PIN = D2;
static constexpr uint8_t LID_PIN = D1;
static constexpr uint8_t VOLTAGE_PIN = A0; // On D0

static constexpr uint32_t BTN_HELD_AFTER_MS = 500;
static constexpr uint32_t BTN_DEBOUNCE_MS = 50;

static constexpr uint32_t LID_DEBOUNCE_MS = 500;

static constexpr uint8_t BTN_PRESSED_SPEAKER_CHIRP_NOTE = 69;
static constexpr uint8_t BTN_HELD_SPEAKER_CHIRP_NOTE = 93;
static constexpr uint8_t BTN_RELEASED_SPEAKER_CHIRP_NOTE = 45;
static constexpr uint16_t BTN_SPEAKER_CHIRP_DURATION = 50;

static constexpr uint8_t CONE_LIGHT_SONG_CHANNELS = 8;
static constexpr int8_t CONE_LIGHT_SONG_END_NOTE = -64;

// 1.4v * 4
static constexpr float VOLTAGE_MAX = 5.6f;
// 1.15v * 4
static constexpr float VOLTAGE_MIN = 4.6f;

static constexpr uint16_t SCREENSAVER_TIMEOUT_MS = 60'000;