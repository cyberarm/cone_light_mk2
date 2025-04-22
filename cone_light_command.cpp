#include "cone_light_command.h"

ConeLightCommand::ConeLightCommand(String name, String help, uint8_t argument_count)
{
  m_name = name;
  m_help = help;
  m_argument_count = argument_count;
}

ConeLightCommand::~ConeLightCommand()
{
}

std::vector<String> ConeLightCommand::process(const String raw_arguments)
{
  String arguments = String(raw_arguments);
  int argument_offset = arguments.indexOf(" ");

  if (arguments.length() < 1 && m_argument_count != 0) // not found
  {
    // Early exit and return empty arguments list
    return {};
  }

  // FIXME: argument counts are spotty
  std::vector<String> arguments_list = {};
  for (size_t i = 0; i < m_argument_count; i++)
  {
    String raw_value = arguments.substring(0, argument_offset);

    arguments_list.push_back(raw_value);

    arguments = arguments.substring(argument_offset + 1);
    argument_offset = arguments.indexOf(" ");

    Serial.printf("%s | %d\n", arguments.c_str(), argument_offset);

    if (argument_offset < 0)
      break;
  }

  return arguments_list;
}

/// --- Commands --- ///
void ConeLightCommand_Help::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  for(auto cone_light_command : cone_light->command_handler()->commands())
  {
    Serial.printf("%s\n%s\n\n", cone_light_command->name().c_str(), cone_light_command->help().c_str());
  }
}

void ConeLightCommand_Song::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  uint16_t song_id = arguments[0].toInt();

  cone_light->speaker()->play_song(song_id);
}

void ConeLightCommand_Tone::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  int16_t frequency = arguments[0].toInt(), duration = arguments[1].toInt();

  cone_light->speaker()->play_tone(frequency, duration);
}

void ConeLightCommand_Color::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  uint8_t red = arguments[0].toInt(),
          green = arguments[1].toInt(),
          blue = arguments[2].toInt(),
          brightness = arguments[3].toInt();

  cone_light->lighting()->set_color(red, green, blue);
  cone_light->lighting()->set_brightness(brightness);
}
