#pragma once

#include <Arduino.h>
#include "cone_light.h"

class ConeLight;
class ConeLightDisplay;

class ConeLightApplication
{
protected:
  ConeLight *m_cone_light = nullptr;
  String m_app_name = "";
  bool m_fullscreen = false;
  unsigned int m_last_updated_ms = 0;
  unsigned int m_update_interval_ms = 3'000;
  ConeLightDisplay *display();
  Adafruit_SSD1306 *oled();

public:
  ConeLightApplication(ConeLight *cone_light);
  virtual ~ConeLightApplication() {};
  String name() { return m_app_name; };
  bool fullscreen() { return m_fullscreen; };
  virtual void draw() {};
  virtual void update() {};
  virtual void reset() {};
  virtual void button_down(ConeLightButton btn) {};
  virtual void button_up(ConeLightButton btn) {};
};

class ConeLight_App_BootScreen : public ConeLightApplication
{
public:
  ConeLight_App_BootScreen(ConeLight *cone_light) : ConeLightApplication(cone_light)
  {
    m_cone_light = cone_light;
    m_app_name = "BootScreen";
  };
  void draw();
  void update();
  void button_down(ConeLightButton btn);
};

class ConeLight_App_MainMenu : public ConeLightApplication
{
public:
  ConeLight_App_MainMenu(ConeLight *cone_light) : ConeLightApplication(cone_light)
  {
    m_cone_light = cone_light;
    m_app_name = "MainMenu";
  };
  void draw();
  void update();
};
