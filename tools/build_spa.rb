require "sass-embedded"
require "slim"
require "terser"

root_path = File.expand_path("../data/single_page_app", __dir__)

css = Sass.compile("#{root_path}/style.sass", style: :compressed).css
js = File.read("#{root_path}/application.js") # Terser.compile(File.read("#{root_path}/application.js")) # minify js
html = Tilt.new("#{root_path}/index.slim").render(nil, css: css, js: js)

File.write("#{root_path}/../cone_light.html", html)
