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

if(!empty($_POST['id'])){

    $id = $_POST['id'];

    $post_user = $connection->prepare("insert into user values(null,:id,null)");

    $post_user->bindParam(':id',$id);

    if($post_user->execute()){
        echo("added");
        $connection->commit();
    }
    else{
        $connection->rollBack();
    }

}
?>