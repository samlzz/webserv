#!/usr/bin/perl
use strict;
use warnings;

my $method = $ENV{'REQUEST_METHOD'} || '';
my $query_string = $ENV{'QUERY_STRING'} || '';
my $path_info = $ENV{'PATH_INFO'} || '';
my $content_length = $ENV{'CONTENT_LENGTH'} || 0;

# Headers
print "Content-Length: 150\r\n";
print "Content-Type: text/html\r\n\r\n";

print "<html><body>\n";
print "<h1>Method: $method</h1>\n";
print "<p>Query String: $query_string</p>\n";
print "<p>Path Info: $path_info</p>\n";
print "<p>Content Length: $content_length</p>\n";

# Lecture du Body si POST
if ($method eq 'POST' && $content_length > 0) {
    my $post_data;
    # Lecture depuis STDIN
    read(STDIN, $post_data, $content_length);
    print "<p>POST Data: $post_data</p>\n";
}

print "<a title=\"test\" href=\"/\">go back</a>\n";
print "</body></html>";