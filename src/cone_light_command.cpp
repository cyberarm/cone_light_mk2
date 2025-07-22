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

// NET_SONG
void ConeLightCommand_NetSong::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  uint16_t song_id = arguments[0].toInt();
  uint8_t group_id = arguments[1].toInt();

  cone_light_network_packet_t packet = {};
  packet.command_id = cone_light->networking()->next_command_id();
  packet.command_type = ConeLightNetworkCommand::PLAY_SONG;
  packet.command_parameters = song_id;
  packet.command_parameters_extra = group_id;

  cone_light->networking()->broadcast_packet(packet);
  // inject packet into the commanding node to make it handle the command itself too
  cone_light->espnow_event(packet);
}

// SONGS
void ConeLightCommand_Songs::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  Serial.println("Songs Catalog:");

  uint16_t i = 0;
  for (auto song : cone_light_songs)
  {
    Serial.printf("    %d: %s\n", i++, song.name().c_str());
  }
}

// TONE
void ConeLightCommand_Tone::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  int16_t frequency = arguments[0].toInt(), duration = arguments[1].toInt();

  cone_light->speaker()->play_tone(frequency, duration);
}

// NET_TONE
void ConeLightCommand_NetTone::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  uint16_t frequency = arguments[0].toInt(), duration = arguments[1].toInt();
  uint8_t group_id = arguments[2].toInt();

  uint32_t packed_freq_duration = uint32_t{frequency} << 16 | uint32_t{duration};

  cone_light_network_packet_t packet = {};
  packet.command_id = cone_light->networking()->next_command_id();
  packet.command_type = ConeLightNetworkCommand::PLAY_TONE;
  packet.command_parameters = packed_freq_duration;
  packet.command_parameters_extra = group_id;

  cone_light->networking()->broadcast_packet(packet);
  // inject packet into the commanding node to make it handle the command itself too
  cone_light->espnow_event(packet);
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

// NET_COLOR
void ConeLightCommand_NetColor::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  uint8_t red = arguments[0].toInt(),
          green = arguments[1].toInt(),
          blue = arguments[2].toInt(),
          brightness = arguments[3].toInt(),
          group_id = arguments[4].toInt();

  uint32_t packed_color = uint32_t{brightness} << 24 |
                          (uint32_t{red} << 16) |
                          (uint32_t{green} << 8) |
                          (uint32_t{blue});

  cone_light_network_packet_t packet = {};
  packet.command_id = cone_light->networking()->next_command_id();
  packet.command_type = (group_id == 255) ? ConeLightNetworkCommand::SET_COLOR : ConeLightNetworkCommand::SET_GROUP_COLOR;
  packet.command_parameters = packed_color;
  packet.command_parameters_extra = group_id;

  cone_light->networking()->broadcast_packet(packet);
  // inject packet into the commanding node to make it handle the command itself too
  cone_light->espnow_event(packet);
}

// CONFIG
void ConeLightCommand_Config::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  Serial.printf("Firmware Info: %s v%s (internal v%d)\n", CONE_LIGHT_PRODUCT_NAME, CONE_LIGHT_FIRMWARE_VERSION_NAME, CONE_LIGHT_FIRMWARE_VERSION);
  Serial.printf("Node Info: %s (id: %d, group: %d, grandmaster clock: %s)\n", cone_light->node_name().c_str(), cone_light->node_id(), cone_light->node_group_id(), (cone_light->node_grandmaster_clock() ? "true" : "false"));
}

// CONFIGURE
void ConeLightCommand_Configure::handle(ConeLight *cone_light, std::vector<String> arguments)
{
  uint8_t node_id = arguments[0].toInt(),
          node_group = arguments[1].toInt();

  String node_name = arguments[2];
  bool node_grandmaster_clock = arguments[3][0] == 't';

  cone_light->reconfigure_node(node_id, node_group, node_name, node_grandmaster_clock);

  Serial.println("Reconfigured node.");
  Serial.printf("Node Info: %s (id: %d, group: %d, grandmaster clock: %s)\n", cone_light->node_name().c_str(), cone_light->node_id(), cone_light->node_group_id(), (cone_light->node_grandmaster_clock() ? "true" : "false"));
}
