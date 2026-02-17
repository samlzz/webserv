#!/usr/bin/ruby

method = ENV['REQUEST_METHOD'] || ''
query_string = ENV['QUERY_STRING'] || ''
path_info = ENV['PATH_INFO'] || ''
content_length = (ENV['CONTENT_LENGTH'] || 0).to_i

# Headers
print "Content-Length: 150\r\n"
print "Content-Type: text/html\r\n\r\n"

puts "<html><body>"
puts "<h1>Method: #{method}</h1>"
puts "<p>Query String: #{query_string}</p>"
puts "<p>Path Info: #{path_info}</p>"
puts "<p>Content Length: #{content_length}</p>"

# Lecture du Body si POST
if method == 'POST' && content_length > 0
  # Lecture depuis STDIN ($stdin)
  post_data = $stdin.read(content_length)
  puts "<p>POST Data: #{post_data}</p>"
end

puts "<a title=\"test\" href=\"/\">go back</a>"
puts "</body></html>"