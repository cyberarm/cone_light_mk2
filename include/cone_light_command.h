#pragma once

#include <Arduino.h>
#include "cone_light.h"

// Forward declarations...
class ConeLight;

class ConeLightCommand
{
protected:
  String m_name;
  String m_help;
  uint8_t m_argument_count;

public:
  ConeLightCommand();
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
  ConeLightCommand_Help()
  {
    m_name = "help";
    m_help = "prints out this help info";
    m_argument_count = 0;
  };
  void handle(ConeLight *cone_light, std::vector<String> arguments);
};

class ConeLightCommand_Song : public ConeLightCommand
{
public:
  ConeLightCommand_Song()
  {
    m_name = "song";
    m_help = "song <song_id> | song 0";
    m_argument_count = 1;
  };
  void handle(ConeLight *cone_light, std::vector<String> arguments);
};

class ConeLightCommand_Tone : public ConeLightCommand
{
public:
  ConeLightCommand_Tone()
  {
    m_name = "tone";
    m_help = "tone <frequency> <duration milliseconds> | tone 440 100";
    m_argument_count = 2;
  };
  void handle(ConeLight *cone_light, std::vector<String> arguments);
};

class ConeLightCommand_Color : public ConeLightCommand
{
public:
  ConeLightCommand_Color()
  {
    m_name = "color";
    m_help = "color <red> <green> <blue> <brightness> | color 210 21 2 255";
    m_argument_count = 4;
  };
  void handle(ConeLight *cone_light, std::vector<String> arguments);
};

class ConeLightCommand_Config : public ConeLightCommand
{
public:
  ConeLightCommand_Config()
  {
    m_name = "config";
    m_help = "prints out nodes configuration information";
    m_argument_count = 0;
  };
  void handle(ConeLight *cone_light, std::vector<String> arguments);
};

class ConeLightCommand_Configure : public ConeLightCommand
{
public:
  ConeLightCommand_Configure()
  {
    m_name = "configure";
    m_help = "configure <node_id> <node_group_id> <node_name> <node_grandmaster_clock> | configure 0 1 HOBBIT false | configures node properties";
    m_argument_count = 4;
  };
  void handle(ConeLight *cone_light, std::vector<String> arguments);
};
