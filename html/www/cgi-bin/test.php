#!/usr/bin/php
<?php

$method = getenv('REQUEST_METHOD') ?: '';
$query_string = getenv('QUERY_STRING') ?: '';
$path_info = getenv('PATH_INFO') ?: '';
$content_length = (int)getenv('CONTENT_LENGTH');


echo "Content-Length: 150\r\n";
echo "Content-Type: text/html\r\n\r\n";

echo "<html><body>\n";
echo "<h1>Method: $method</h1>\n";
echo "<p>Query String: $query_string</p>\n";
echo "<p>Path Info: $path_info</p>\n";
echo "<p>Content Length: $content_length</p>\n";

// Lecture du Body si POST
if ($method === 'POST' && $content_length > 0) {
    // Lecture brute depuis l'entrée standard
    $post_data = file_get_contents("php://input");
    // Ou limitation exacte à la taille : $post_data = fread(STDIN, $content_length);
    echo "<p>POST Data: " . htmlspecialchars($post_data) . "</p>\n";
}

echo "<a title=\"test\" href=\"/\">go back</a>\n";
echo "</body></html>";
?>