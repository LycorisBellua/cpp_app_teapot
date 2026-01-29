#!/usr/bin/perl

use strict;
use warnings;

# CGI scripts must output HTTP headers first
print "Content-Type: text/html\r\n";
print "\r\n";  # Blank line separates headers from body

# HTML content
print "<html>\n";
print "<head><title>Perl CGI Test</title></head>\n";
print "<body>\n";
print "<h1>Hello from Perl CGI!</h1>\n";
print "<p>This is a basic CGI script.</p>\n";

# Display some environment variables
print "<h2>Environment Variables:</h2>\n";
print "<ul>\n";
print "<li>REQUEST_METHOD: $ENV{REQUEST_METHOD}</li>\n";
print "<li>QUERY_STRING: $ENV{QUERY_STRING}</li>\n";
print "<li>CONTENT_TYPE: " . ($ENV{CONTENT_TYPE} // "not set") . "</li>\n";
print "<li>CONTENT_LENGTH: " . ($ENV{CONTENT_LENGTH} // "not set") . "</li>\n";
print "</ul>\n";

print "</body>\n";
print "</html>\n";