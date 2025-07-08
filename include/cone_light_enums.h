#pragma once

enum ConeLightEvent
{
  BUTTON_PRESSED = 0,
  BUTTON_HELD,
  BUTTON_RELEASED,

  LID_CLOSED = 16,
  LID_OPENED,
};

enum ConeLightButton
{
  BUTTON_A = 0,
  BUTTON_B,
  BUTTON_C,
  LID,

  UP_BUTTON = BUTTON_A,
  SELECT_BUTTON = BUTTON_B,
  MIDDLE_BUTTON = BUTTON_B,
  DOWN_BUTTON = BUTTON_C
};

enum ConeLightNetworkCommand
{
  NOT_A_COMMAND = 0, // This message is not a "command" and should be forwarded to the active app for processing.

  // NETWORK TIME
  CLOCK_GRANDMASTER_SYNC = 10,
  CLOCK_NODE_DELAY_REQUEST,
  CLOCK_NODE_DELAY_RESPONSE,

  // LIGHTING
  SET_COLOR = 20,
  SET_GROUP_COLOR,

  // SPEAKER
  PLAY_TONE = 30,
  PLAY_SONG,

  // SPECIAL
  START_ANIMATION = 40
};

enum ConeLightDirection
{
  UP = 1,
  NONE = 0,
  DOWN = -1
};
