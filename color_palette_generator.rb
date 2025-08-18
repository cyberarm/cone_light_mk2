require "gosu"

class Window < Gosu::Window
  def initialize(*args)
    super

    @octave = 12
    @offset = 0

    @radius = 128
    @square = 64
    @export = false
  end

  def update
    self.caption = "OFFSET: #{@offset}"
  end

  def draw
    @octave.times do |i|
      hue = ((360.0 / @octave) * i + @offset) % 360

      origin = [self.width / 2, self.height / 2]
      color = Gosu::Color.from_hsv(hue, 1.0, 1.0)

      Gosu.draw_rect(
        origin[0] + Gosu.offset_x(hue, @radius), origin[1] + Gosu.offset_y(hue, @radius),
        @square,
        @square,
        color
      )

      if @export
        puts "CRGB(#{color.to_i}),"
      end
    end

    @export = false
  end

  def button_down(id)
    case id
    when Gosu::KB_UP
      @offset += 1
    when Gosu::KB_DOWN
      @offset -= 1
    when Gosu::KB_E
      @export = true
    end
  end
end

Window.new(1280, 800, fullscreen: false, resizable: true).show
