<?php

/*You'll want to edit the following.  From AWS lightsail, your mysql 
username shouuld be 'root', you can find your password by typing
cat bitnami_application_password 
at the SSH prompt
*/
$db_user = "root";
$db_pass = "*****";

/*
The following code will take the data you send via http 
From your ESP32 and parse the values.  
Sending via http will be accomplished from the ESP32 by writing 
http://ipaddressofserver/example/esp32_hellowworld/esp32_add2database.php?variable1=value1&variable2=value2

The text after the ? is called a "query string." 
The below code extracts out the value of the variables sent that are named temp_php_C, temp_php_F, and humidity_php.
It defines them as php variables $theTempC, $theTempF, and $theHumidity.

Variables in PHP are prefaced with '$'
you can name your php variables whatever you want to
*/

$theTempC=$_GET['temp_php_C'];
echo "$theTempC\n = ";
$theTempF=$_GET['temp_php_F'];
echo "$theTempF\n = ";
$theHumidity=$_GET['humidity_php'];
echo "$theHumidity\n = ";


//name of the database
//mine is called helloesp32
//change below if yours has a different name

$database = "helloesp32";

/*create the connection to mySQL database
db_user, db_pass, and database just point to the particular
database we want  as well as the credentials
127.0.0.1 is just a "loopback" - as 127.0.0.1 is the IP address for
the localhost, meaning your AWS server this will run on.
 The script lives on the machine that's making the query, so it loops back to itself.
*/
$connection = mysqli_connect("127.0.0.1", $db_user, $db_pass, $database);

//on failure, throw an error
if ($mysqli->connect_errno) {
    echo "Failed to connect to MySQL: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
}


/*create the query with syntax
INSERT INTO <table> VALUES <value1, value2,...>
note that the order has to be the same as they are defined
in the table

My table is called “esp32_dht20” - update the below code
To match your table name.

Any fields in your table that autopopulate
such as a unique id (likely your primary key) and a timestamp,
will be added automatically when you add data to the table.  

In my case, the database has 5 fields: id, time_stamp, tempC, tempF, humidity.  
'id' is an auto-incrementing unique id
'time_stamp' is an auto-populating timestamp.
so I only need to insert the tempC, tempF,  and humidity fields 
with the 3 php variables $theTempC, $theTempF, and $theHumidity
*/

$query="INSERT INTO esp32_dht20 (tempC, tempF, humidity) VALUES ('$theTempC', '$theTempF', '$theHumidity')";

$result = mysqli_query($connection, $query);

//on success, run the query 
if ($result) {
    //and let us know if it succeeds
    printf("Insert successful.\n");
}

//don't forget to close out
mysqli_close($connection);
?>

