#include "cone_light_command_handler.h"

ConeLightInputHandler::ConeLightInputHandler(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  Serial.println("    InputHandler initialized successfully.");
}

ConeLightInputHandler::~ConeLightInputHandler()
{
}

void ConeLightInputHandler::update()
{
  handle_button(UP_BUTTON, digitalRead(BTN_A_PIN), m_btn_a_state, m_last_btn_a_state, m_btn_a_state_changed_ms);
  handle_button(MIDDLE_BUTTON, digitalRead(BTN_B_PIN), m_btn_b_state, m_last_btn_b_state, m_btn_b_state_changed_ms);
  handle_button(DOWN_BUTTON, digitalRead(BTN_C_PIN), m_btn_c_state, m_last_btn_c_state, m_btn_c_state_changed_ms);

  handle_lid(digitalRead(LID_PIN));
}

void ConeLightInputHandler::handle_button(ConeLightButton btn, int raw_state, ConeLightEvent &state, ConeLightEvent &previous_state, long &state_changed_ms)
{
  bool state_changed = false;
  ConeLightEvent current_state = raw_state == LOW ? BUTTON_PRESSED : BUTTON_RELEASED;
  if (current_state == BUTTON_PRESSED && previous_state == BUTTON_HELD)
    current_state = BUTTON_HELD;
  if (current_state == BUTTON_PRESSED && previous_state == BUTTON_PRESSED && millis() - state_changed_ms >= BTN_HELD_AFTER_MS)
    current_state = BUTTON_HELD;

  if (current_state != previous_state && millis() - state_changed_ms >= BTN_DEBOUNCE_MS)
    state_changed = true;

  if (state_changed)
  {
    state_changed_ms = millis();
    previous_state = current_state;

    String btn_name = "UNKNOWN BUTTON";
    if (btn == BUTTON_A)
      btn_name = "A";
    if (btn == BUTTON_B)
      btn_name = "B";
    if (btn == BUTTON_C)
      btn_name = "C";

    String btn_state = "UNKNOWN STATE";
    if (current_state == BUTTON_PRESSED)
      btn_state = "PRESSED";
    if (current_state == BUTTON_HELD)
      btn_state = "HELD";
    if (current_state == BUTTON_RELEASED)
      btn_state = "RELEASED";

    // TODO: Trigger an event in ConeLight to preprocess and/or forward to Application(s)
    // m_cone_light->event(btn, state);
    Serial.printf("BTN CHANGED: %s -> %s | %d\n", btn_name.c_str(), btn_state.c_str(), millis());
  }
}

void ConeLightInputHandler::handle_lid(int raw_state)
{
}

bool ConeLightInputHandler::button_down(ConeLightButton btn)
{
  switch (btn)
  {
  case UP_BUTTON:
  case MIDDLE_BUTTON:
  case DOWN_BUTTON:
    return false; // TODO: FIXME :)
    break;

  default:
    return false; // This is fine. :)
    break;
  }
}
