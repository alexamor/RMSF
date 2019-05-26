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


$get_permissions = $connection->prepare("select * from user order by id asc;");

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
    $id = $row['id'];

    $name = $row['name'];

    $permissions = $row['permissions'];

    echo("Name:$name|Id:$id|Permissions:$permissions;" );

}






?>