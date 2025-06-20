#include "cone_light_command.h"

ConeLightCommand::ConeLightCommand()
{
}

ConeLightCommand::~ConeLightCommand()
{
}

std::vector<String> ConeLightCommand::process(const String raw_arguments)
{
  String arguments = String(raw_arguments);
  int argument_offset = arguments.indexOf(" ");

  if (arguments.length() < 1 && m_argument_count != 0) // no arguments found
  {
    // Early exit and return empty arguments list
    return {};
  }

  std::vector<String> arguments_list = {};
  for (size_t i = 0; i < m_argument_count; i++)
  {
    String raw_value = arguments.substring(0, argument_offset);

    arguments_list.push_back(raw_value);

    // Accept trailing argument without <SPACE> once then loop around, append the argument and break.
    if (argument_offset < 0)
      break;

    arguments = arguments.substring(argument_offset + 1);
    argument_offset = arguments.indexOf(" ");
  }

  return arguments_list;
}

/// --- Commands --- ///
// HELP
void ConeLightCommand_Help::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  for (auto cone_light_command : cone_light->command_handler()->commands())
  {
    Serial.printf("%s\n%s\n\n", cone_light_command->name().c_str(), cone_light_command->help().c_str());
  }
}

// SONG
void ConeLightCommand_Song::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  uint16_t song_id = arguments[0].toInt();

  cone_light->speaker()->play_song(song_id);
}

// TONE
void ConeLightCommand_Tone::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  int16_t frequency = arguments[0].toInt(), duration = arguments[1].toInt();

  cone_light->speaker()->play_tone(frequency, duration);
}

// COLOR
void ConeLightCommand_Color::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  uint8_t red = arguments[0].toInt(),
          green = arguments[1].toInt(),
          blue = arguments[2].toInt(),
          brightness = arguments[3].toInt();

  cone_light->lighting()->set_color(red, green, blue);
  cone_light->lighting()->set_brightness(brightness);
}

// CONFIG
void ConeLightCommand_Config::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  Serial.printf("Firmware Info: %s v%s (internal v%d)\n", CONE_LIGHT_PRODUCT_NAME, CONE_LIGHT_FIRMWARE_VERSION_NAME, CONE_LIGHT_FIRMWARE_VERSION);
  Serial.printf("Node Info: %s (id: %d, group: %d)\n", cone_light->node_name().c_str(), cone_light->node_id(), cone_light->node_group_id());
}

// CONFIGURE
void ConeLightCommand_Configure::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  uint8_t node_id = arguments[0].toInt(),
          node_group = arguments[1].toInt();

  String node_name = arguments[2];

  cone_light->reconfigure_node(node_id, node_group, node_name);

  Serial.println("Reconfigured node.");
  Serial.printf("Node Info: %s (id: %d, group: %d)\n", cone_light->node_name().c_str(), cone_light->node_id(), cone_light->node_group_id());
}
