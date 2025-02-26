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


    define('API_ACCESS_KEY', 'AAAAxqderzY:APA91bG572asI7wetvQP0ykLQ1MOLqJ5RxGFauu5T6u_jR2IbqAL7L7bX26eiTFb6MfdPVmPJwMVdBY7uMAP_nep8H-awbkCGbQ-bnLaxvWrSdyh3efMyDq2tR8hyZqBigfPJyAMijOU');
    $fcmUrl = 'https://fcm.googleapis.com/fcm/send';
    $token = [
        "Alex" => 'cJyBzB4toC0:APA91bEHIaReA_tJUzdsoh6uS7jJAOnybc_SSNSxgG3KP7-a3UJ0dcDRRzrIP3hUMCv4tuHEh0MbrnzfGM2uhZOor-LnVLPF8s9ju3JdLDsuTsZSWuCkC8ZPva1-KbTsKE4AhzT4hsZr',
        "Sofia" => 'd-rFrdfRDZE:APA91bGN4FKWDbSZHui2jf9u3d6MP_S5Nf8BNuFaA5mkQ2BQ7dUXAKw3DEBhkGkdLP8zI0NWdAxJOWYNixkWsGSz8zjThyvIBAvKrF-I_6r4kI-BUEnz92cTN8QETNIMSSDBBOuh6kzF'
    ];

    $notification = [
        'title' => 'Novo alarme',
        'body' => 'Foi detectada uma nova intrusão!'
    ];

    $fcmNotification = [
        'to' => $token['Alex'],
        'notification' => $notification
    ];

    $headers = [
        'Authorization: key=' . API_ACCESS_KEY,
        'Content-Type: application/json'
    ];

    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL,$fcmUrl);
    curl_setopt($ch, CURLOPT_POST, true);
    curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($fcmNotification));
    $result = curl_exec($ch);
    curl_close($ch);



}
?>
