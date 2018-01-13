<?php
error_reporting(E_ALL);

set_time_limit(10);

ob_implicit_flush();
 ?>

<!DOCTYPE html>
<html>
  <head>
  <meta charset="utf-8" />
  <link rel="stylesheet" href="../css/index.css" />
  </head>
  <body>
    <div id="data_window">
      <div id ="entete">
      </div>
      <div id="corps">
        <div class="line">
        <?php
        $handle = @fopen("/opt/vrac-berry/backup_file.txt", "r");
        if ($handle) {
          $i = 0;
          while (($buffer = fgets($handle, 4096)) !== false) {
            echo "<span class=\"space\"> </span><button id=\"button_". $i . "\" class=\"dispenser\">";
            $array_for_dispenser = explode("/", $buffer, 5);
            $generated_string = " ID : <span class=\"id_dispenser\">" . $array_for_dispenser[0] . "</span><br/>"; // ID
            $generated_string .= " Remplissage : <span class=\"filling_dispenser\">" . $array_for_dispenser[1] . "</span><br/>"; // filling
            $generated_string .= "  Batterie : <span class=\"battery_dispenser\">" . $array_for_dispenser[2] . "</span><br/>"; // battery
            $generated_string .= " Produit : <span class=\"name_dispenser\">" . $array_for_dispenser[3] . "</span></button><span class=\"space\"> </span>"; // name
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
      <div id="footer">
        <span class="network">
          <button id="network_request"> Open Network Configuration </button>
        </span>
      </div>
    </div>
    <div id="detail_window">
      <form action="index.php" method="post">
        <p> Nouveau nom du produit : </p> <input type="text" id = "name_product" name = "name" value = "ERROR" />
        <input  id = "id_product" type="hidden" value="0"/>
      </form>
      <table>
        <tr>
          <td><div class="current_data" id="filling_data">NULL</div></td>
          <td><div class="current_data" id="battery_data">NULL</div></td>
        </tr>
        <tr>
          <td>Remplissage</td><td>Batterie</td>
        </tr>
      </table>
        <p> Dernier nettoyage le : <span id="last_washing_date_data"></span></p>
        <button id="data_request_from_detail"> Close </button>
        <button id="send_cleaning_update"> Nettoyage à ce jour </button>
        <button id="send_name_update"> Mise à jour Produit </button>
    </div>
    <div id="network_window">
        <h3> Configuration du réseau </h3>
        <form>
          <p> Heure de mise en veille : </p>
          <input type="text" id="sleeping_hour_data" value="0"/>
          <p> Heure de redémarrage : </p>
          <input type="text" id="waking_hour_data" value="0"/>
          <p> Temps inter-émission : </p>
          <input type="text" id="in_between_message_time_data" value="0"/>
          <p> Fréquence de nettoyage des distributeurs : </p>
          <input type="text" id="cleaning_frequency_data" value="0"/>

        </form>
        <button id="data_request_from_network"> Close </button>
        <button id="send_new_configuration"> Confirme </button>
    </div>
    <script src="../js/jquery-3.2.1.js"></script>
     <script src="../js/index.js"></script>
  </body>
</html>
