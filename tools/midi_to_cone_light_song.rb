file_path = ARGV.shift
song_name = ARGV.shift || File.basename(file_path, ".midi")
available_channels = 8

puts "No such file: #{file_path}" unless File.exist?(file_path)

require "midilib"

Note = Struct.new(:channel, :note, :starts_at_ms, :duration_ms)

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
          note: -1,
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
      start_time_ms = (seq.pulses_to_seconds(e.time_from_start) * 1000.0).round
      duration_ms = (seq.pulses_to_seconds(e.off.time_from_start) * 1000.0).round - start_time_ms

      # pp [e.note, note, start_time_ms, duration_ms]
      # +24 to push up a couple octaves
      notes << Note.new(channel: i - 1, note: e.note + 24, starts_at_ms: start_time_ms, duration_ms: duration_ms)
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
note_rows = []
channels.each do |c|
  note_rows << "{#{c.notes.map { |n| n.note }.join(', ')}}"
end
first_line = true
note_rows.join(",\n").lines.each_with_index do |line, i|
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
