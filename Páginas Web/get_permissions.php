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

if(!empty($_GET['id'])){

    $id = $_GET['id'];

    $get_permissions = $connection->prepare("select * from user where id=:id");
    $get_permissions->bindParam(':id', $id);

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
    $nrows = $get_permissions->rowCount();

    if($nrows == 0){
        echo("The search doesn't match our database");
    }
    else{
        $permissions = $result[0]['permissions'];
        echo("<p>$permissions</p>");
    }
}



?>