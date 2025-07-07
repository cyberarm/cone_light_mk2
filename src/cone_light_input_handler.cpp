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
  handle_button(UP_BUTTON, digitalRead(BTN_A_PIN), m_fuzzy_btn_a_state, m_last_btn_a_state, m_btn_a_state_changed_ms);
  handle_button(MIDDLE_BUTTON, digitalRead(BTN_B_PIN), m_fuzzy_btn_b_state, m_last_btn_b_state, m_btn_b_state_changed_ms);
  handle_button(DOWN_BUTTON, digitalRead(BTN_C_PIN), m_fuzzy_btn_c_state, m_last_btn_c_state, m_btn_c_state_changed_ms);

  handle_lid(digitalRead(LID_PIN));
}

void ConeLightInputHandler::handle_button(ConeLightButton btn, int raw_state, ConeLightEvent &fuzzy_state, ConeLightEvent &previous_state, uint32_t &state_changed_ms)
{
  bool state_changed = false;
  ConeLightEvent current_state = raw_state == LOW ? BUTTON_PRESSED : BUTTON_RELEASED;
  if (current_state == BUTTON_PRESSED && previous_state == BUTTON_HELD)
    current_state = BUTTON_HELD;
  if (current_state == BUTTON_PRESSED && previous_state == BUTTON_PRESSED && millis() - state_changed_ms >= BTN_HELD_AFTER_MS)
    current_state = BUTTON_HELD;

  if (current_state != fuzzy_state)
  {
    // BUTTON_HELD is a special case, since its not actually a physical button state but an artificial one.
    // and setting the `state_changed_ms` here when `current_state` is BUTTON_HELD breaks the trigger for it.
    if (current_state != BUTTON_HELD)
      state_changed_ms = millis();

    fuzzy_state = current_state;
  }

  if (millis() - state_changed_ms >= BTN_DEBOUNCE_MS)
    state_changed = current_state != previous_state;

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

    m_cone_light->button_event(btn, current_state);

    Serial.printf("BTN CHANGED: %s -> %s | %d\n", btn_name.c_str(), btn_state.c_str(), millis());
  }
}

void ConeLightInputHandler::handle_lid(int raw_state)
{
  bool state_changed = false;
  ConeLightEvent current_state = raw_state == LOW ? LID_CLOSED : LID_OPENED;

  if (current_state != m_fuzzy_lid_state)
  {
    m_lid_state_changed_ms = millis();
    m_fuzzy_lid_state = current_state;
  }

  if (millis() - m_lid_state_changed_ms >= LID_DEBOUNCE_MS)
    state_changed = current_state != m_last_lid_state;

  if (state_changed)
  {
    m_lid_state_changed_ms = millis();
    m_last_lid_state = current_state;

    String lid_state = "UNKNOWN STATE";
    if (current_state == LID_CLOSED)
      lid_state = "CLOSED";
    if (current_state == LID_OPENED)
      lid_state = "OPENED";

    m_cone_light->lid_event(current_state);

    Serial.printf("LID CHANGED: %s -> %s | %d\n", "LID", lid_state.c_str(), millis());
  }
}

bool ConeLightInputHandler::button_down(ConeLightButton btn)
{
  switch (btn)
  {
  case UP_BUTTON:
    return (m_last_btn_a_state == BUTTON_PRESSED || m_last_btn_a_state == BUTTON_HELD);
    break;
  case MIDDLE_BUTTON:
    return (m_last_btn_b_state == BUTTON_PRESSED || m_last_btn_b_state == BUTTON_HELD);
    break;
  case DOWN_BUTTON:
    return (m_last_btn_c_state == BUTTON_PRESSED || m_last_btn_c_state == BUTTON_HELD);
    break;

  default:
    return false; // This is fine. :)
    break;
  }
}
