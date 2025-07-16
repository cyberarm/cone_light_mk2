file_path = ARGV.shift
song_name = ARGV.shift || File.basename(file_path, ".midi")
available_channels = 8

unless File.exist?(file_path)
  puts "No such file: #{file_path}"
end

require "midilib"

Note = Struct.new(:channel, :frequency, :starts_at_ms, :duration_ms)

NOTE_TO_FREQUENCY = [
  # Pad by twelve notes
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  16.351,
  17.324,
  18.354,
  19.445,
  20.601,
  21.827,
  23.124,
  24.499,
  25.956,
  27.5,
  29.135,
  30.868,

  32.703,
  34.648,
  36.708,
  38.891,
  41.203,
  43.654,
  46.249,
  48.999,
  51.913,
  55,
  58.27,
  61.735,

  65.406,
  69.296,
  73.416,
  77.782,
  82.407,
  87.307,
  92.499,
  97.999,
  103.826,
  110,
  116.541,
  123.471,

  130.813,
  138.591,
  146.832,
  155.563,
  164.814,
  174.614,
  184.997,
  195.998,
  207.652,
  220,
  233.082,
  246.942,

  261.626,
  277.183,
  293.665,
  311.127,
  329.628,
  349.228,
  369.994,
  391.995,
  415.305,
  440,
  466.164,
  493.883,

  523.251,
  554.365,
  587.33,
  622.254,
  659.255,
  698.456,
  739.989,
  783.991,
  830.609,
  880,
  932.328,
  987.767,

  1046.502,
  1108.731,
  1174.659,
  1244.508,
  1318.51,
  1396.913,
  1479.978,
  1567.982,
  1661.219,
  1760,
  1864.655,
  1975.533,

  2093.005,
  2217.461,
  2349.318,
  2489.016,
  2637.021,
  2793.826,
  2959.955,
  3135.964,
  3322.438,
  3520,
  3729.31,
  3951.066,

  4186.009,
  4434.922,
  4698.636,
  4978.032,
  5274.042,
  5587.652,
  5919.91,
  6271.928,
  6644.876,
  7040,
  7458.62,
  7902.132,

  8372.018,
  8869.844,
  9397.272,
  9956.064,
  10548.084,
  11175.304,
  11839.82,
  12543.856,
  13289.752,
  14080,
  14917.24,
  15804.264
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