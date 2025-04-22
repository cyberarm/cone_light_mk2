#pragma once

#include <Arduino.h>
#include "cone_light.h"

// Forward declarations...
class ConeLight;

class ConeLightCommand
{
private:
  String m_name;
  String m_help;
  uint8_t m_argument_count;

public:
  ConeLightCommand(String name, String help, uint8_t argument_count);
  ~ConeLightCommand();
  String name() { return m_name; }
  String help() { return m_help; }
  uint8_t argument_count() { return m_argument_count; }
  virtual void handle(ConeLight *cone_light, std::vector<String> arguments) { Serial.println("sadness."); };
  std::vector<String> process(const String raw_arguments);
};

/// --- Commands --- ///
class ConeLightCommand_Help : public ConeLightCommand
{
public:
  ConeLightCommand_Help(String name, String help, uint8_t argument_count) : ConeLightCommand(name, help, argument_count) {};
  void handle(ConeLight *cone_light, std::vector<String> arguments);
};

class ConeLightCommand_Song : public ConeLightCommand
{
  public:
    ConeLightCommand_Song(String name, String help, uint8_t argument_count) : ConeLightCommand(name, help, argument_count) {};
    void handle(ConeLight *cone_light, std::vector<String> arguments);
};

class ConeLightCommand_Tone : public ConeLightCommand
{
  public:
    ConeLightCommand_Tone(String name, String help, uint8_t argument_count) : ConeLightCommand(name, help, argument_count) {};
    void handle(ConeLight *cone_light, std::vector<String> arguments);
};

class ConeLightCommand_Color : public ConeLightCommand
{
  public:
    ConeLightCommand_Color(String name, String help, uint8_t argument_count) : ConeLightCommand(name, help, argument_count) {};
    void handle(ConeLight *cone_light, std::vector<String> arguments);
};
