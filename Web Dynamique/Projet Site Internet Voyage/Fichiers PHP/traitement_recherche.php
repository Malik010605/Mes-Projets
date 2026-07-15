<?php
/* Ce fichier servira si les recherches sont centralisees plus tard. */

$type = isset($_GET["type"]) ? $_GET["type"] : "";
$recherche = isset($_GET["recherche"]) ? $_GET["recherche"] : "";

if ($type == "transport") {
  header("Location: ../transports.php?recherche=" . urlencode($recherche));
  exit();
}

header("Location: ../destinations.php?recherche=" . urlencode($recherche));
exit();
?>
