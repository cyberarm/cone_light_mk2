firmware_bin_path = "./.pio/build/seeed_xiao_esp32_c6/firmware.bin"
esptool_path = "#{Dir.home}/.platformio/packages/tool-esptoolpy/esptool.py"
devices = Dir.glob("/dev/ttyACM*")

pp firmware_bin_path, esptool_path, devices
threads = []

devices.each do |device|
  command = "#{Dir.home}/.platformio/penv/bin/python /home/cyberarm/.platformio/packages/tool-esptoolpy/esptool.py "\
            "--chip esp32c6 --port #{device} --baud 460800 --before default_reset --after hard_reset write_flash -z "\
            "--flash_mode dio --flash_freq 80m --flash_size detect 0x0000 ./.pio/build/seeed_xiao_esp32_c6/bootloader.bin 0x8000 ./.pio/build/seeed_xiao_esp32_c6/partitions.bin 0xe000 "\
            "#{Dir.home}/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin 0x10000 ./.pio/build/seeed_xiao_esp32_c6/firmware.bin"

  threads << Thread.new do
    system(command)
  end
end

while (threads.find { |thread| thread.alive? })
  sleep 0.1
end

puts ""
puts "Done."
puts
