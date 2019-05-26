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


$get_permissions = $connection->prepare("select * from record order by entry_time desc;");

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

    $get_user_name = $connection->prepare("select name from user where id=:id");

    $get_user_name->bindParam(':id', $id);

    try{
        $get_user_name->execute();

    }
    catch(PDOException $exception){
        echo("<p>Error: ");
        echo($exception->getMessage());
        echo("</p>");
        exit();
    }

    $name_result = $get_permissions->fetchAll();
    $name = $name_result[0]['name'];

    $local = $row['local'];
    $entry_time = $row['entry_time'];
    $exit_time = $row['exit_time'];

    echo("Name:$name|Entry time:$entry_time|Exit time:$exit_time|Local:$local|Id:$id;" );

}






?>