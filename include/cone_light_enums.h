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
  CLOCK = 10
};

enum ConeLightDirection
{
  UP = 1,
  NONE = 0,
  DOWN = -1
};
