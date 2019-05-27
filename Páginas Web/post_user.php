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

if(!empty($_POST['name']) && !empty($_POST['permissions']) && !empty($_POST['id'])){
    $name = $_POST['name'];

    $permissions = $_POST['permissions'];

    $id = $_POST['id'];

    $post_user = $connection->prepare("update user set name=:name, permissions=:permissions where id=:id");

    $post_user->bindParam(':id',$id);
    $post_user->bindParam(':name',$name);
    $post_user->bindParam(':permissions',$permissions);

    if($post_user->execute()){
        echo("ok");
        $connection->commit();
    }
    else{
        $connection->rollBack();
    }

}
?>