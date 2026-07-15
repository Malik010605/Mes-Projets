<?php
$servername = "localhost";
$username = "root";
$password = "root";
$dbname = "voyagevista_db";
$port = 3306;
$charset = "utf8mb4";

$conn = @mysqli_connect($servername, $username, $password, $dbname, $port);

if ($conn) {
  mysqli_set_charset($conn, $charset);
}
?>
