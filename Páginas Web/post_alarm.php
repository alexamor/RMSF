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

$connection->beginTransaction();

if(!empty($_POST['local'])){
    $local = $_POST['local'];

    $timestamp=date("Y-m-d H:i:s");

    $post_alarm = $connection->prepare("insert into alarm values(:local, :timestamp)");

    $post_alarm->bindParam(':local', $local);
    $post_alarm->bindParam(':timestamp', $timestamp);

    if($post_alarm->execute()){
        echo("ok");
        $connection->commit();
    }
    else{
        $connection->rollBack();
    }

}
?>
