#pragma once

#include "cone_light.h"

class ConeLight;

class ConeLightInputHandler
{
private:
  ConeLight *m_cone_light = nullptr;

  ConeLightEvent m_last_btn_a_state = BUTTON_RELEASED,
                 m_last_btn_b_state = BUTTON_RELEASED,
                 m_last_btn_c_state = BUTTON_RELEASED,
                 m_last_lid_state = LID_OPENED;

  ConeLightEvent m_btn_a_state = BUTTON_RELEASED,
                 m_btn_b_state = BUTTON_RELEASED,
                 m_btn_c_state = BUTTON_RELEASED,
                 m_lid_state = LID_OPENED;

  long m_btn_a_state_changed_ms = 0,
       m_btn_b_state_changed_ms = 0,
       m_btn_c_state_changed_ms = 0,
       m_lid_state_changed_ms = 0;

  void handle_button(ConeLightButton btn, int raw_state, ConeLightEvent &state, ConeLightEvent &previous_state, long &state_changed_ms);
  void handle_lid(int raw_state);

public:
  ConeLightInputHandler(ConeLight *cone_light);
  ~ConeLightInputHandler();
  void update();
  void reset();
  bool button_down(ConeLightButton btn);
  bool lid_open() { return false; }; // FIXME
};
