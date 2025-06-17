#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include <format>
#include "cone_light.h"
#include "cone_light_lighting.h"

class ConeLight;
class ConeLightDisplay;

class ConeLightApplication
{
protected:
  ConeLight *m_cone_light = nullptr;
  String m_app_name = "";
  bool m_fullscreen = false;
  bool m_needs_redraw = true;
  unsigned int m_last_updated_ms = 0;
  unsigned int m_update_interval_ms = 3'000;
  ConeLightDisplay *display();
  Adafruit_SSD1306 *oled();

public:
  ConeLightApplication(ConeLight *cone_light);
  virtual ~ConeLightApplication() {};
  String name() { return m_app_name; };
  bool fullscreen() { return m_fullscreen; };
  bool needs_redraw() { return m_needs_redraw; };
  virtual void draw() {};
  virtual void update() {};
  void focus() { m_needs_redraw = true; };
  virtual void blur() {};
  virtual void reset() {};
  virtual void button_down(ConeLightButton btn) {};
  virtual void button_up(ConeLightButton btn) {};
  virtual void lid_closed() {};
  virtual void lid_opened() {};
};

class ConeLight_App_BootScreen : public ConeLightApplication
{
protected:
  unsigned long m_last_milliseconds = millis();
  CRGB m_boot_color = CRGB(83, 151, 152);
  CRGB m_group_color = CRGB(0, 0, 0);

public:
  ConeLight_App_BootScreen(ConeLight *cone_light);
  void draw();
  void update();
  void button_down(ConeLightButton btn);
};

class ConeLight_App_MainMenu : public ConeLightApplication
{
public:
  ConeLight_App_MainMenu(ConeLight *cone_light);
  void draw();
  void update();
  void button_down(ConeLightButton btn);

  uint8_t m_app_index = 2;
  uint8_t m_max_app_index = 0;
};

class ConeLight_App_ManualControl : public ConeLightApplication
{
public:
  ConeLight_App_ManualControl(ConeLight *cone_light) : ConeLightApplication(cone_light)
  {
    m_cone_light = cone_light;
    m_app_name = "Manual Control";
  };
  void draw();
  // void update();
  // void button_down(ConeLightButton btn);
  // void focus();
  // void blur();
};

class ConeLight_App_SmartControl : public ConeLightApplication
{
public:
  ConeLight_App_SmartControl(ConeLight *cone_light) : ConeLightApplication(cone_light)
  {
    m_cone_light = cone_light;
    m_app_name = "Smart Control";
  };
  void draw();
  // void update();
  // void button_down(ConeLightButton btn);
  // void focus();
  // void blur();
};

class ConeLight_App_NodeInfo : public ConeLightApplication
{
public:
  ConeLight_App_NodeInfo(ConeLight *cone_light) : ConeLightApplication(cone_light)
  {
    m_cone_light = cone_light;
    m_app_name = "Node Info";
    m_fullscreen = true;
  };
  void draw();
  void button_down(ConeLightButton btn);
};

class ConeLight_App_BatteryInfo : public ConeLightApplication
{
public:
  ConeLight_App_BatteryInfo(ConeLight *cone_light) : ConeLightApplication(cone_light)
  {
    m_cone_light = cone_light;
    m_app_name = "Battery Info";
    m_fullscreen = false;
  };
  void draw();
  void update();
  void button_down(ConeLightButton btn);

  float m_last_battery_voltage_percentage = 0.0f;
  float m_voltage_percentage = 0.0f;
  float m_voltage_ratio = 0.0f;
};
