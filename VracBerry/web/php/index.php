<?php
error_reporting(E_ALL);

set_time_limit(10);

ob_implicit_flush();

/*$address ="192.168.50.1";
$port=10000;

if (($sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) === false) {
    echo "socket_create() a échoué : raison : " . socket_strerror(socket_last_error()) . "\n";
}

if (socket_bind($sock, $address, $port) === false) {
    echo "socket_bind() a échoué : raison : " . socket_strerror(socket_last_error($sock)) . "\n";
}

if (socket_listen($sock, 5) === false) {
    echo "socket_listen() a échoué : raison : " . socket_strerror(socket_last_error($sock)) . "\n";
}

do {
    if (($msgsock = socket_accept($sock)) === false) {
        echo "socket_accept() a échoué : raison : " . socket_strerror(socket_last_error($sock)) . "\n";
        break;
    }
    ////// Send instructions.
    $msg = "\Bienvenue sur le serveur de test PHP.\n" .
        "Pour quitter, tapez 'quit'. Pour éteindre le serveur, tapez 'shutdown'.\n";
    socket_write($msgsock, $msg, strlen($msg));

    do {
        if (false === ($buf = socket_read($msgsock, 2048, PHP_NORMAL_READ))) {
            echo "socket_read() a échoué : raison : " . socket_strerror(socket_last_error($msgsock)) . "\n";
            break 2;
        }
        if (!$buf = trim($buf)) {
            continue;
        }
        if ($buf == 'quit') {
            break;
        }
        if ($buf == 'shutdown') {
            socket_close($msgsock);
            break 2;
        }
        $talkback = "PHP: You said '$buf'.\n";
        socket_write($msgsock, $talkback, strlen($talkback));
        echo "$buf\n";
    } while (true);
    socket_close($msgsock);
} while (true);

socket_close($sock);
*/
 ?>

<!DOCTYPE html>
<html>
  <head>
  <meta charset="utf-8" />
  <link rel="stylesheet" href="../css/index.css" />
  </head>
  <body>
    <div id="page">
      <div id ="entete">
      </div>
      <div id="corps">
        <div class="line">
        <?php
        $handle = @fopen("/opt/vrac-berry/backup_file.txt", "r");
        if ($handle) {
          $i = 0;
          while (($buffer = fgets($handle, 4096)) !== false) {
            echo "<button class=\"dispenser\">";
            $array_for_dispenser = explode("/", $buffer, 5);
            $generated_string = " ID : <span class=\"id_dispenser\">" . $array_for_dispenser[0] . "</span><br/>"; // ID
            $generated_string .= " Remplissage : <span class=\"filling_dispenser\">" . $array_for_dispenser[1] . "</span><br/>"; // filling
            $generated_string .= "  Batterie : <span class=\"battery_dispenser\">" . $array_for_dispenser[2] . "</span><br/>"; // battery
            $generated_string .= " Produit : <span class=\"name_dispenser\">" . $array_for_dispenser[3] . "</span></button>"; // name
            echo $generated_string;

            if($i%3 == 2){
              echo "</div><div class=\"line\">";
            }
            $i++;
          }
          if (!feof($handle)) {
              echo "Erreur: fgets() a échoué\n";
          }
          fclose($handle);
        }
         ?>
       </div>
      </div>
    </div>
  </body>
</html>
