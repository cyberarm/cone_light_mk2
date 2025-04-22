#include "cone_light_command_handler.h"

ConeLightCommandHandler::ConeLightCommandHandler(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  // Serial.setTimeout(100);
  m_commands.push_back(new ConeLightCommand_Help("help", "prints out this help info", 0));
  m_commands.push_back(new ConeLightCommand_Song("song", "song <song_id> | song 0", 1));
  m_commands.push_back(new ConeLightCommand_Tone("tone", "tone <frequency> <duration milliseconds> | tone 440 100", 2));
  m_commands.push_back(new ConeLightCommand_Color("color", "color <red> <green> <blue> <brightness> | color 210 21 2 255", 4));

  Serial.println("    Command Handler initialized successfully.");
}

ConeLightCommandHandler::~ConeLightCommandHandler()
{
}

void ConeLightCommandHandler::update()
{
  if (Serial.available() > 0)
  {
    String buf = Serial.readStringUntil('\n');
    buf.trim();

    Serial.printf("    CommandHandler buffer: %s\n", buf.c_str());

    int separator_offset = buf.indexOf(" ");
    if (separator_offset < 0)
      separator_offset = buf.length();

    String command = buf.substring(0, separator_offset);
    command.toLowerCase();

    for (size_t i = 0; i < m_commands.size(); i++)
    {
      if (command.compareTo(m_commands[i]->name()) == 0)
      {
        ConeLightCommand *cone_light_command = m_commands[i];

        Serial.printf("      Found: %s\n", cone_light_command->name().c_str());

        std::vector<String> args = cone_light_command->process(buf.substring(separator_offset + 1, buf.length()));
        Serial.printf("      Note: number of arguments provided. Got %d, expected %d.\n", args.size(), cone_light_command->argument_count());

        if (args.size() == cone_light_command->argument_count())
        {
          cone_light_command->handle(m_cone_light, args);
        }
        else
        {
          Serial.printf("      Error: invalid number of arguments provided. Got %d, expected %d.\n", args.size(), cone_light_command->argument_count());
        }

        return;
      }
    }

    Serial.printf("      Error: no such command found: %s\n", command.c_str());
  }
}

void ConeLightCommandHandler::reset()
{
}
