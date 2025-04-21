#include "cone_light_command_handler.h"

ConeLightCommandHandler::ConeLightCommandHandler(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  // Serial.setTimeout(100);

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

    int seperator_offset = buf.indexOf(" ");

    // FIXME: Don't skip if no seperator (could be 'help' after all)
    if (seperator_offset < 0)
      return;

    String command = buf.substring(0, seperator_offset);
    command.toLowerCase();

    // FIXME: convert to a for loop to handle multiple commands
    if (command.compareTo("color") == 0)
    {
      String arguments = buf.substring(seperator_offset + 1);
      Serial.printf("    CommandHandler Color: %s\n", arguments.c_str());

      int argument_offset = arguments.indexOf(" ");
      uint8_t red, green, blue, brightness;

      for (size_t i = 0; i < 4; i++)
      {
        String raw_value = arguments.substring(0, argument_offset);
        int value = raw_value.toInt();

        switch (i)
        {
        case 0:
          red = value;
          break;
        case 1:
          green = value;
          break;
        case 2:
          blue = value;
          break;
        case 3:
          brightness = value;
          break;
        default:
          break;
        }

        arguments = arguments.substring(argument_offset + 1);
        argument_offset = arguments.indexOf(" ");
      }

      m_cone_light->lighting()->set_color(red, green, blue);
      m_cone_light->lighting()->set_brightness(brightness);
    }
    else if (command.compareTo("tone") == 0)
    {
      String arguments = buf.substring(seperator_offset + 1);
      Serial.printf("    CommandHandler Tone: %s\n", arguments.c_str());

      int argument_offset = arguments.indexOf(" ");
      int16_t frequency, duration;

      for (size_t i = 0; i < 2; i++)
      {
        String raw_value = arguments.substring(0, argument_offset);
        int value = raw_value.toInt();

        switch (i)
        {
        case 0:
          frequency = value;
          break;
        case 1:
          duration = value;
          break;
        default:
          break;
        }

        arguments = arguments.substring(argument_offset + 1);
        argument_offset = arguments.indexOf(" ");
      }

      m_cone_light->speaker()->play_tone(frequency, duration);
    }
    else if (command.compareTo("song") == 0)
    {
      String arguments = buf.substring(seperator_offset + 1);
      Serial.printf("    CommandHandler Song: %s\n", arguments.c_str());

      int argument_offset = arguments.indexOf(" ");
      int16_t song_id;

      for (size_t i = 0; i < 1; i++)
      {
        String raw_value = arguments.substring(0, argument_offset);
        int value = raw_value.toInt();

        switch (i)
        {
        case 0:
          song_id = value;
          break;
        default:
          break;
        }

        arguments = arguments.substring(argument_offset + 1);
        argument_offset = arguments.indexOf(" ");
      }

      m_cone_light->speaker()->play_song(song_id);
    }
    else
    {
      Serial.printf("    Commandhandler: Unknown command: %s\n", command.c_str());
    }
  }
}

void ConeLightCommandHandler::reset()
{
}
