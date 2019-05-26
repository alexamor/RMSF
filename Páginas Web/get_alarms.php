<?php

// Connect to database
$host = "db.tecnico.ulisboa.pt";
$user = "ist425485";
$pass = "gntw7047";
$dsn = "mysql:host=$host;dbname=$user";

// Initiate connection
try
{
    $connection = new PDO($dsn, $user, $pass);
}
catch(PDOException $exception)
{
    echo("<p>Error: ");
    echo($exception->getMessage());
    echo("</p>");
    exit();
}


$get_permissions = $connection->prepare("select * from alarm order by alarm_time desc;");

try{
    $get_permissions->execute();

}
catch(PDOException $exception){
    echo("<p>Error: ");
    echo($exception->getMessage());
    echo("</p>");
    exit();
}

$result = $get_permissions->fetchAll();

foreach($result as $row){
    //get id and then access table "user" to get the name
    $local = $row['local'];

    $alarm_time = $row['alarm_time'];


    echo("Name:$local|Alarm time:$alarm_time;" );

}






?>