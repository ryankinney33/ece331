<?php
# Fetches the previous 24 hours of data from the database
header('content-type: text/json');

# Connect to the database
$db = new SQLite3('temps.db');
$cmd = $db->prepare("SELECT * FROM temps WHERE timestamp >= datetime('now', '-24 hours');");
$result = $cmd->execute();

# Fetch data from the database
while ($row = $result->fetchArray()) {
	$data[] = ['time' => $row[0], 'room' => $row[1], 'core' => $row[2]];
}

$db->close(); # Close the database connection

# Encode the data in json format
echo json_encode($data);
?>
