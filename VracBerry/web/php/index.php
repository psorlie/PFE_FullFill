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
        <p> </p>
    </div>
    <div id="network_window">
        <p> </p>
    </div>
    <script src="../js/jquery-3.2.1.js"></script>
    <script src="../js/socket.io.js"></script>
     <script src="../js/index.js"></script>
  </body>
</html>
