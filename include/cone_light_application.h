#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include <format>
#include "cone_light.h"
#include "cone_light_lighting.h"
#include "cone_light_networking.h"

// Forward declarations... 💔 you C 😿
class ConeLight;
class ConeLightDisplay;
typedef struct cone_light_network_packet cone_light_network_packet_t;

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
  virtual void focus() { m_needs_redraw = true; };
  virtual void blur() {};
  virtual void reset() {};
  // returns true if the app has handled the button event(s)
  // currently used for triggering chirps on button press/held/release when an action has taken place
  virtual bool button_down(ConeLightButton btn) { return false; };
  virtual bool button_held(ConeLightButton btn) { return false; };
  virtual bool button_up(ConeLightButton btn) { return false; };
  virtual void lid_closed() {};
  virtual void lid_opened() {};
  virtual void espnow_recv(cone_light_network_packet_t packet) {};
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
  bool button_down(ConeLightButton btn);
};

class ConeLight_App_MainMenu : public ConeLightApplication
{
public:
  ConeLight_App_MainMenu(ConeLight *cone_light);
  void draw();
  void update();
  bool button_down(ConeLightButton btn);

  uint8_t m_app_index = 2;
  uint8_t m_max_app_index = 0;
};

class ConeLight_App_ManualControl : public ConeLightApplication
{
private:
  bool m_selected = false;
  size_t m_index = 0;
  const std::string m_labels[4] = {"R", "G", "B", "L"};

public:
  ConeLight_App_ManualControl(ConeLight *cone_light) : ConeLightApplication(cone_light)
  {
    m_cone_light = cone_light;
    m_app_name = "Manual Control";
  };
  void draw();
  void update();
  bool button_down(ConeLightButton btn);
  bool button_held(ConeLightButton btn);
  // void focus();
  // void blur();
};

class ConeLight_App_SyncedControl : public ConeLightApplication
{
public:
  ConeLight_App_SyncedControl(ConeLight *cone_light) : ConeLightApplication(cone_light)
  {
    m_cone_light = cone_light;
    m_app_name = "Synced Control";
  };
  void draw();
  // void update();
  bool button_down(ConeLightButton btn);
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
  bool button_down(ConeLightButton btn);
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
  bool button_down(ConeLightButton btn);

  float m_last_battery_voltage_percentage = 0.0f;
  float m_voltage_percentage = 0.0f;
  float m_voltage_ratio = 0.0f;
};

class ConeLight_App_Debug_ESPNow_Sender : public ConeLightApplication
{
private:
  uint16_t m_packet_id = 0;
  unsigned long m_last_transmit_ms = 0;

public:
  ConeLight_App_Debug_ESPNow_Sender(ConeLight *cone_light) : ConeLightApplication(cone_light)
  {
    m_cone_light = cone_light;
    m_app_name = "D: ESPNOW SEND";
    m_fullscreen = false;
  };
  void draw();
  void update();
  bool button_down(ConeLightButton btn);
};

class ConeLight_App_Debug_ESPNow_Receiver : public ConeLightApplication
{
private:
  uint16_t m_last_packet_id = 0;
  unsigned int m_packets_lost = 0;
  unsigned int m_total_packets_lost = 0;

public:
  ConeLight_App_Debug_ESPNow_Receiver(ConeLight *cone_light) : ConeLightApplication(cone_light)
  {
    m_cone_light = cone_light;
    m_app_name = "D: ESPNOW RECV";
    m_fullscreen = false;
  };
  void draw();
  void update();
  bool button_down(ConeLightButton btn);
  void espnow_recv(cone_light_network_packet_t packet);
};
