<?php
$file = "positions.json";
$data = file_exists($file) ? json_decode(file_get_contents($file), true) : [];

$mac = $_POST["mac"];
$data[$mac] = [
    "lat" => $_POST["lat"],
    "lng" => $_POST["lng"],
    "time" => time(),
];

file_put_contents($file, json_encode($data));
?>
