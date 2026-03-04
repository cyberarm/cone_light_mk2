require "sass-embedded"
require "slim"
require "terser"

root_path = File.expand_path("../data/single_page_app", __dir__)

# native color pickers suck :'(
coloris_css = File.read("#{root_path}/coloris.min.css")
coloris_js = File.read("#{root_path}/coloris.min.js")

css = Sass.compile("#{root_path}/style.sass", style: :compressed).css
js = Terser.compile(File.read("#{root_path}/application.js")) # minify js
html = Tilt.new("#{root_path}/index.slim").render(nil, coloris_css: coloris_css, coloris_js: coloris_js, css: css, js: js)

File.write("#{root_path}/../cone_light.html", html)

if ARGV[0] == "--packed"
  require "zlib"

  blob = Zlib.gzip(html, level: Zlib::BEST_COMPRESSION)
  puts "Input: #{html.size}, Compressed: #{blob.size}, Space saved: #{html.size - blob.size} (#{((html.size.to_f / blob.size) * 100.0).round(2)}%)"

  File.open("#{root_path}/../../include/cone_light_web_spa.h", "w") do |f|
    f.puts("#pragma once")
    f.puts
    f.puts("const unsigned char data_cone_light_html[] = {")
    blob.bytes.each_slice(12).each do |bytes|
      f.print("  ")
      f.print(bytes.map { |b| format("0x%02x", b) }.join(", "))
      f.puts(",")
    end
    f.puts("};")
    f.puts
    f.puts("uint32_t data_cone_light_html_len = #{blob.size};")
  end
end
