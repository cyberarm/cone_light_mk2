require  "json"
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
      pause_notes << Note.new(note: -1, starts_at_ms: 0, duration_ms: n1.starts_at_ms)
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

def song_to_json(category, name, transpose, notes, durations)
  {
    category: category,
    name: name,
    transpose: transpose,
    notes: notes,
    durations: durations
}.to_json
end

failed_list = []
available_channels = 8
output = File.open("./midis.jsonl", "w") do |f|
  # Write abort "song"
  f.puts(
    song_to_json(
      0,
      "ABORT PLAYBACK",
      0,
      available_channels.times.map { [-1] },
      available_channels.times.map { [0] }
    )
  )

  Dir.glob("#{Dir.home}/Nextcloud/cone_light_midis/*/*_cone_light.mid").each do |file_path|
    unless File.exist?(file_path)
      puts "No such file: #{file_path}"
      next
    end

    puts "Processing #{file_path}..."

    waterfalled = false
    notes = []
    channels = available_channels.times.to_a.map { CyberarmSongChannel.new }

    seq = MIDI::Sequence.new
    File.open(file_path, "rb") do |file|
      seq.read(file) do |track, num_tracks, i|
        next unless track

        # Print something when each track is read.
        # puts "  read track #{i} of #{num_tracks}"
        # pp track
        # pp track.name
        # pp track.events.size
        track.events.select { |e| e.is_a?(MIDI::NoteOn) }.each do |e|
          start_time_ms = (seq.pulses_to_seconds(e.time_from_start) * 1000.0).round
          duration_ms = (seq.pulses_to_seconds(e.off.time_from_start) * 1000.0).round - start_time_ms

          # pp [e.note, note, start_time_ms, duration_ms]
          notes << Note.new(channel: i - 1, note: e.note, starts_at_ms: start_time_ms, duration_ms: duration_ms)
        end
      end
    end

    # Sequence notes across channels
    # Insert dead space (empty notes)

    failed = false
    notes.each do |note|
      channel = channels[note.channel] if channels[note.channel]&.available?(note)
      # warn "Failed to find optimal channel for note! (optimal channel: #{note.channel} [#{note.inspect}])" unless channel
      waterfalled = true unless channel

      channel ||= channels.find { |c| c.available?(note) }

      unless channel
        warn "    FAIL: Failed to find available time slot for note in any channel. #{note.inspect}"
        failed_list << [file_path, note]
        failed = true

        break
      end

      channel.add_note(note)
    end

    next if failed

    # Analyze notes to determine whether to transpose song to make notes more audible by using higher frequencies while avoiding the resonant frequency of the beeper ~2700 Mhz (E7)
    octave = 12
    transpose = 0
    peak_note = 0
    channels.each do |c|
      max_note = c.notes.map(&:note).max
      peak_note = max_note if max_note > peak_note
    end
    # Can we "boost" the song without reaching the resonant frequency?
    while(peak_note + transpose + octave < 100) # E7
      transpose += octave
    end

    # Standardize starting pause so every song will have a fixed delay before starting
    standard_delay_ms = 1_000 # 1 second
    trimmed_time_ms = 0
    time_to_first_note_ms = 10_000_000 # ten million milliseconds should be a safe bet :D
    channels.each do |c|
      time_to_first_note_ms = c.notes.first.starts_at_ms if c.notes.first.starts_at_ms < time_to_first_note_ms
    end
    # Shift note start times
    if time_to_first_note_ms != standard_delay_ms
      offset_ms = standard_delay_ms - time_to_first_note_ms

      channels.each do |c|
        c.notes.each do |n|
          n.starts_at_ms += offset_ms
        end
      end
    end

    # Insert null notes, i.e. rests or dead time where no note is played
    channels.each(&:insert_dead_times)

    puts "    WARN: Waterfalled! Node(s) may trade off melodies and baselines. Unless played in cluster, song will sound terrible." if waterfalled

    f.puts(
      song_to_json(
        File.dirname(file_path).split("/").last.split("_").first.to_i,
        File.basename(file_path, "_cone_light.mid").split("_").map(&:capitalize).join(" "),
        transpose,
        channels.map { |c| c.notes.map(&:note) },
        channels.map { |c| c.notes.map(&:duration_ms) }
      )
    )
  end
end

pp failed_list
failed_list.each do |file_path, note|
  puts "FAILED: #{file_path}. Note #{note.note} at #{note.starts_at_ms} ms in channel #{note.channel}"
end
