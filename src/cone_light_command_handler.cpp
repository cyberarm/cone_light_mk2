#include "include/cone_light_command_handler.h"

ConeLightCommandHandler::ConeLightCommandHandler(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  // Serial.setTimeout(100);
  m_commands.push_back(new ConeLightCommand_Help);
  m_commands.push_back(new ConeLightCommand_Song);
  m_commands.push_back(new ConeLightCommand_NetSong);
  m_commands.push_back(new ConeLightCommand_Songs);
  m_commands.push_back(new ConeLightCommand_Tone);
  m_commands.push_back(new ConeLightCommand_NetTone);
  m_commands.push_back(new ConeLightCommand_Color);
  m_commands.push_back(new ConeLightCommand_NetColor);
  m_commands.push_back(new ConeLightCommand_Config);
  m_commands.push_back(new ConeLightCommand_Configure);
  m_commands.push_back(new ConeLightCommand_MemoryUsage);

  Serial.println("    Command Handler initialized successfully.");
}

ConeLightCommandHandler::~ConeLightCommandHandler()
{
}

void ConeLightCommandHandler::update()
{
  if (Serial.available() > 0)
  {
    uint8_t buffer[64] = {0};

    // Unexpectedly large Serial input, drain and discard buffer of unexpected size
    if (Serial.available() > 63)
    {
      while (Serial.available())
        Serial.read();

      Serial.println("Unexpectedly large input (greater than 63 characters). Discarded.");
      return;
    }

    Serial.read(buffer, Serial.available());

    auto buffer_to_char_ptr = reinterpret_cast<char*>(buffer);
    String buf = String(buffer_to_char_ptr);
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

        // Serial.printf("      Found: %s\n", cone_light_command->name().c_str());

        std::vector<String> args = cone_light_command->process(buf.substring(separator_offset + 1, buf.length()));
        // Serial.printf("      Note: number of arguments provided. Got %d, expected %d.\n", args.size(), cone_light_command->argument_count());

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
