<?php
//your credentials - replace ... with your information
//put each credential in single quotes - as shown for $db_user

$db_user = 'root';
$db_pass = ...;
$database = ...;

/*
connect to the database; since this code is on our server, 127.0.0.1 is used as the IP address; 27.0.0.1 is the IP address for the computer you are on, also called the "localhost"
*/

$mysqli = new mysqli("127.0.0.1", $db_user, $db_pass, $database);
if ($mysqli->connect_errno) {
    echo "Failed to connect to MySQL: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
}

/*
Create and run the query, saves it in variable $q

note that we order by time_stamp in descending order, 
and then limit our results to just ten records to get the latest 10 readings.

Here, we are returning the fields "tempC", "tempF", "humidiity", and "time_stamp" from the database "esp32_dht20".  These are sorted (ORDER BY) the timestamp "time_stamp" in a (DESC) descening order.  We are only returning the top 10 rows (LIMIT 10)

The fancy footwork "date_add()" function 
shifts from GMT by -5 hours, which is Eastern Standard Time.
*/

$q=$mysqli->query("SELECT tempC, tempF, humidity, date_add(time_stamp,INTERVAL -5 HOUR) FROM esp32_dht20 ORDER BY time_stamp DESC LIMIT 10");

/*
echoing back html
in html, a table is formed with tags like
<tr> - the rows
<th> - the headers, defining 3 columns
<td> - the columns
*/

// etablishing the table and the header row

echo "<table border='1'>
<tr>
<th>Timestamp</th>
<th>Temp (C)</th>
<th>Temp (F)</th>
<th>Humidity (%)</th>
</tr>";

/*
mysqyli_fetch_array fetches a row from a table and stores it as an array of data
The loop goes through and echos back each row of data, one at a time
*/

while($row = mysqli_fetch_array($q))
{
echo "<tr>";

/*
$row[0] is the first element of a row of data from the $q.
With the above query, the first element is tempC. To echo back the time_stamp, refer to $row[3].
*/

echo "<td>" . $row[3] . "</td>";
echo "<td>" . $row[0] . "</td>";


/*you can also refer to data from the query 
using field names from the database
*/
echo "<td>" . $row['tempF'] . "</td>";
echo "<td>" . $row['humidity'] . "</td>";

echo "</tr>";

} //end the While loop

echo "</table>";

$mysqli->close();
?>
