file_path = ARGV.shift
song_name = ARGV.shift || File.basename(file_path, ".midi")
available_channels = 8

unless File.exist?(file_path)
  puts "No such file: #{file_path}"
end

require "midilib"

Note = Struct.new(:channel, :frequency, :starts_at_ms, :duration_ms)

NOTE_TO_FREQUENCY = [
  8.662,
  8.176,
  9.723,
  9.177,
  10.913,
  10.301,
  12.250,
  11.562,
  13.750,
  12.978,
  15.434,
  14.568,
  17.324,
  16.352,
  19.445,
  18.354,
  21.827,
  20.602,
  24.500,
  23.125,
  27.500,
  25.957,
  30.868,
  29.135,
  34.648,
  32.703,
  38.891,
  36.708,
  43.654,
  41.203,
  48.999,
  46.249,
  55.000,
  51.913,
  61.735,
  58.270,
  69.296,
  65.406,
  77.782,
  73.416,
  87.307,
  82.407,
  97.999,
  92.499,
  110.000,
  103.826,
  123.471,
  116.541,
  138.591,
  130.813,
  155.563,
  146.832,
  174.614,
  164.814,
  195.998,
  184.997,
  220.000,
  207.652,
  246.942,
  233.082,
  277.183,
  261.626,
  311.127,
  293.665,
  349.228,
  329.628,
  391.995,
  369.994,
  440.000,
  415.305,
  493.883,
  466.164,
  554.365,
  523.251,
  622.254,
  587.330,
  698.456,
  659.255,
  783.991,
  739.989,
  880.000,
  830.609,
  987.767,
  932.328,
  1108.731,
  1046.502,
  1244.508,
  1174.659,
  1396.913,
  1318.510,
  1567.982,
  1479.978,
  1760.000,
  1661.219,
  1975.533,
  1864.655,
  2217.461,
  2093.005,
  2489.016,
  2349.318,
  2793.826,
  2637.020,
  3135.963,
  2959.955,
  3520.000,
  3322.438,
  3951.066,
  3729.310,
  4434.922,
  4186.009,
  4978.032,
  4698.636,
  5587.652,
  5274.041,
  6271.927,
  5919.911,
  7040.000,
  6644.875,
  7902.133,
  7458.620,
  8869.844,
  8372.018,
  9956.063,
  9397.273,
  11175.300,
  10548.080,
  12543.850,
  11839.820
].freeze

def note_to_frequency(note)
  NOTE_TO_FREQUENCY[note].round
end

class CyberarmSongChannel
  attr_reader :notes

  def initialize
    @notes = []
  end

  def available?(note)
    @notes.each do |n|
      return false if note.starts_at_ms.between?(n.starts_at_ms, n.starts_at_ms + n.duration_ms - 14) # subtract 14ms so perfectly timed notes don't cause notes to be passed off to other channels
      return false if (note.starts_at_ms + note.duration_ms).between?(n.starts_at_ms, n.starts_at_ms + n.duration_ms)
    end

    true
  end

  def add_note(note)
    @notes << note
  end

  def insert_dead_times
    return if @notes.empty?

    # Correct for notes inserted out of order, temporally speaking
    @notes.sort_by! { |n| n.starts_at_ms }

    offset = 1
    pause_notes = []
    n1 = notes.first

    if n1.starts_at_ms > 0
      pause_notes << Note.new(frequency: -1, starts_at_ms: 0, duration_ms: n1.starts_at_ms)
      offset = 0
    end

    @notes[(pause_notes.empty? ? 1 : 0)..].each do |n|
      unless n.starts_at_ms - n1.starts_at_ms <= 0
        pause_notes << Note.new(
          channel: nil,
          frequency: -1,
          starts_at_ms: n1.starts_at_ms + n1.duration_ms,
          duration_ms: n.starts_at_ms - (n1.starts_at_ms + n1.duration_ms)
        )
      end

      n1 = n
    end

    c = 0
    pause_notes.each_with_index do |n, i|
      if n.duration_ms > 0
        @notes.insert(offset + c, n)
        c += 1
      end

      offset += 1
    end

    @notes.freeze
  end
end

notes = []
channels = available_channels.times.to_a.map { CyberarmSongChannel.new }

seq = MIDI::Sequence.new
File.open(file_path, "rb") do |file|
  seq.read(file) do |track, num_tracks, i|
    next unless track

    # Print something when each track is read.
    puts "read track #{i} of #{num_tracks}"
    # pp track
    pp track.name
    pp track.events.size
    track.events.select { |e| e.is_a?(MIDI::NoteOn) }.each do |e|
      frequency = note_to_frequency(e.note)
      start_time_ms = (seq.pulses_to_seconds(e.time_from_start) * 1000.0).round
      duration_ms = (seq.pulses_to_seconds(e.off.time_from_start) * 1000.0).round - start_time_ms

      # pp [e.note, frequency, start_time_ms, duration_ms]
      notes << Note.new(channel: i - 1, frequency: frequency, starts_at_ms: start_time_ms, duration_ms: duration_ms)
    end
  end
end

# Sequence notes across channels
# Insert dead space (empty notes)

notes.each do |note|
  channel = channels[note.channel] if channels[note.channel]&.available?(note)
  warn "Failed to find optimal channel for note! (optimal channel: #{note.channel} [#{note.inspect}])" unless channel

  channel ||= channels.find { |c| c.available?(note) }

  unless channel
    raise "Failed to find available time slot for note in any channel. #{note.inspect}"
  end

  channel.add_note(note)
end

puts notes.size

channels.each(&:insert_dead_times)

puts 
puts "Output:"
puts
puts  "    ConeLightSong("
puts  "        \"#{song_name}\","
print "        {"
frequency_rows = []
channels.each do |c|
  frequency_rows << "{#{c.notes.map { |n| n.frequency }.join(', ')}}"
end
first_line = true
frequency_rows.join(",\n").lines.each_with_index do |line, i|
  print first_line ? "#{line}" : "         #{line}"

  first_line = false
end
puts "},"
print "        {"
duration_rows = []
channels.each do |c|
  duration_rows << "{#{c.notes.map { |n| n.duration_ms }.join(', ')}}"
end
first_line = true
duration_rows.join(",\n").lines.each_with_index do |line, i|
  print first_line ? "#{line}" : "         #{line}"

  first_line = false
end
puts "})"

puts

#-------------------------------------------------------#
# --- OLD TESTING CODE BELOW ---------------------------#
#-------------------------------------------------------#

# snames = ("A".."Z").to_a
# puts
# puts
# puts "/* Song cake */"
# channels.each_with_index do |c, i|
#   puts "int16_t notes_#{snames[i]}[#{c.notes.size}] = {#{c.notes.map { |n| n.frequency }.join(', ')}};"
#   puts "uint16_t durations_#{snames[i]}[#{c.notes.size}] = {#{c.notes.map { |n| n.duration_ms }.join(', ')}};"
#   puts
# end
# puts
# puts "/* Song cake */"
# puts "song->reset();"
# puts
# channels.each_with_index do |c, i|
#   puts "song->channel(SPEAKER_#{snames[i]}, #{i}, #{c.notes.size}, notes_#{snames[i]}, durations_#{snames[i]});"
# end
# puts

# puts
# puts
# channels.each_with_index do |c, i|
#   duration_seconds = c.notes.sum(&:duration_ms) / 1000.0
#   puts "channel #{snames[i]} duration: #{(duration_seconds / 60.0).round(2)} minutes (#{duration_seconds} seconds)"
# end
# 