#include "cone_light_application.h"

//--- ||| ---//
ConeLightApplication::ConeLightApplication(ConeLight *cone_light)
{
}

ConeLightDisplay *ConeLightApplication::display() { return m_cone_light->display(); };

Adafruit_SSD1306 *ConeLightApplication::oled() { return display()->oled(); };

//--- BOOT SCREEN APP ---//
void ConeLight_App_BootScreen::draw()
{
  oled()->clearDisplay();

  // Cone
  oled()->fillTriangle(12, 43, 20, 4, 43, 43, SSD1306_WHITE);
  oled()->fillTriangle(20, 4, 35, 4, 43, 43, SSD1306_WHITE);
  oled()->fillRect(4, 44, 48, 4, SSD1306_WHITE);

  // Cross
  oled()->fillRect(20, 15 + 4, 16, 4, SSD1306_BLACK);
  oled()->fillRect(26, 8 + 4, 4, 36 - 8, SSD1306_BLACK);

  // Labels
  oled()->setCursor(11, 54);
  oled()->print(m_cone_light->node_name());
  oled()->printf("  v%s", CONE_LIGHT_FIRMWARE_VERSION_NAME);

  oled()->setCursor(56, 12);
  oled()->print("Cone Light");
  oled()->setCursor(68, 24);
  oled()->print("mk. II");

  oled()->display();
}

void ConeLight_App_BootScreen::update()
{
  // Boot complete
  // if (millis() >= 3'000)
  //   m_cone_light->boot_complete();
}

void ConeLight_App_BootScreen::button_down(ConeLightButton btn)
{
  if (millis() >= 3'000 && btn == BUTTON_A)
  {
    m_cone_light->lighting()->set_color(210, 21, 2);
    m_cone_light->lighting()->set_brightness(255);
  }

  // Boot complete
  if (millis() >= 3'000 && btn == BUTTON_B)
    m_cone_light->boot_complete();

  if (millis() >= 3'000 && btn == BUTTON_C)
    m_cone_light->speaker()->play_song(0);
}

//--- MAIN MENU APP ---//
void ConeLight_App_MainMenu::draw()
{
  oled()->clearDisplay();

  display()->draw_up_arrow(7, 6);
  oled()->drawFastHLine(0, 21, 20, SSD1306_WHITE);
  display()->draw_select_icon(7 + 3, 32);
  oled()->drawFastHLine(0, 42, 20, SSD1306_WHITE);
  display()->draw_down_arrow(7, 64 - (9 + 8));

  oled()->drawFastVLine(20, 0, 64, SSD1306_WHITE);

  // Border
  oled()->drawRect(0, 0, 128, 64, SSD1306_WHITE);

  // Labels
  oled()->setCursor(32, 32 - 3);
  // oled()->print("SMART CONTROL");
  oled()->printf("%.3fv (%3.1f%%)", m_cone_light->voltage()->voltage(), m_cone_light->voltage()->voltage_percentage());

  oled()->setCursor(42, 4);
  oled()->print("manual control");
  oled()->setCursor(42, 64 - (7 + 4));
  oled()->print("airplane mode");

  oled()->display();
}

void ConeLight_App_MainMenu::update()
{
}
