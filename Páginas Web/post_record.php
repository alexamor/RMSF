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

if( !empty($_POST['id']) && !empty($_POST['local'])){

    $id = $_POST['id'];
    $local = $_POST['local'];
    // get records
    $get_records = $connection->prepare("select * from record where id=:id and local=:local order by entry_time desc");

    $get_records->bindParam(':id', $id);
    $get_records->bindParam(':local', $local);

    try{
        $get_records->execute();

    }
    catch(PDOException $exception){
        echo("<p>Error: ");
        echo($exception->getMessage());
        echo("</p>");
    }

    $result = $get_records->fetchAll();
    $nrows = $get_records->rowCount();

    $timestamp=date("Y-m-d H:i:s");

    if($nrows != 0){

        if($result[0]['exit_time'] == "0000-00-00 00:00:00"){

            $entry = $result[0]['entry_time'];
            echo($entry);

            $update_record = $connection->prepare("update record set id=:id, exit_time=:exit_time, entry_time=:entry_time where id=:id and entry_time=:entry_time");

            $update_record->bindParam(':exit_time', $timestamp);
            $update_record->bindParam(':id', $id);
            $update_record->bindParam(':entry_time', $entry);
            echo("here");
        }
        else{
            $set_new_record = true;
        }

    }
    else{
        $set_new_record = true;
    }

    if($set_new_record == true){

        echo("here");
        $new_record = $connection->prepare("insert into record values(:local, :entry_time, \"0000-00-00 00:00:00\", :id)");

        $new_record->bindParam(':local', $local);
        $new_record->bindParam(':entry_time', $timestamp);
        $new_record->bindParam(':id', $id);


        if($new_record->execute()) {
            echo("ok");
            $connection->commit();
        }
        else{
            $connection->rollBack();
        }

    }
    else{
        if($update_record->execute()){
            echo("ok");
            $connection->commit();
        }
        else{
            $connection->rollBack();
        }
    }

}
?>