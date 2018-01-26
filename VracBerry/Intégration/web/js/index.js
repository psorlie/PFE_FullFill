
/*************************************** Inclusion des fichiers js nécessaires ***************************************/
function include(fileName){
  document.write("<script type='text/javascript' src='"+fileName+"'></script>" );
}

include("../js/mqttws31.js");
include("../js/jquery-3.2.1.js");
//src="jquery.min.js";
include("../js/config.js");

/*************************************** MQTT ***************************************/

var mqtt;
var reconnectTimeout = 2000;
var array_displayed_dispenser = new Array(100);

function MQTT_send(message){
    mqtt.send(TOPIC_ENVOI, message, QOS, false);
}

function MQTTconnect() {
    if (typeof path == "undefined") {
        path = "/mqtt";
    }
    mqtt = new Paho.MQTT.Client(
            HOST,
            PORT,
            path,
            "web_" + parseInt(Math.random() * 100, 10)
    );
        var options = {
            timeout: 3,
            useSSL: useTLS,
            cleanSession: CLEANSESSION,
            onSuccess: onConnect,
            onFailure: function (message) {
                console.log("Connection failed: " + message.errorMessage + "Retrying");
                setTimeout(MQTTconnect, reconnectTimeout);
            }
        };

        mqtt.onConnectionLost = onConnectionLost;
        mqtt.onMessageArrived = onMessageArrived;

        if (username != null) {
            options.userName = username;
            options.password = password;
        }
        console.log("Host="+ HOST + ", port=" + PORT + ", path=" + path + " TLS = " + useTLS + " username=" + username + " password=" + password);
        mqtt.connect(options);
    }

function onConnect() {
    console.log('Connected to ' + HOST + ':' + PORT + path);
    // Connection succeeded; subscribe to our topic
    mqtt.subscribe(TOPIC_RECEPTION, {qos: 1});
    console.log("topic : " + TOPIC_RECEPTION);

    send_request_socket("ask_full_update");
    //MQTT_send("HaHaHaHa Merlin!");

}

function onConnectionLost(response) {
    setTimeout(MQTTconnect, reconnectTimeout);
    //console.log("connection lost: " + responseObject.errorMessage + ". Reconnecting");
    console.log("connection lost: " + response.errorMessage + ". Reconnecting");
};

  function onMessageArrived(message) {

      var topic = message.destinationName;
      var payload = message.payloadString;

      console.log(topic + ' = ' + payload);

      //MQTT_send('{"type":"msjgtbmstjn", "id":145}');
      dispatch(payload);
  };


  $(document).ready(function() {
      MQTTconnect();
  });


/********************************************** Réception des JSON *******************************************/

function add_button_to_array(id){
  array_displayed_dispenser.push(id);
}

//function called everytime a socket is received
function dispatch(data) {
  var msg = JSON.parse(data);
  console.log("reçu : " + msg.type);
  switch (msg.type) {
    // is an answer to someone asking for the details of a dispenser
    case "detail":
      console.log("j'ai reçu un detail");
      setDetailWindow(msg.id, msg.filling, msg.battery, msg.date, msg.name);
    break;
    // a update sent by the server
    case "update":
      console.log("j'ai reçu un update");
      if($.inArray(msg.id , array_displayed_dispenser) == -1){
        addNewButton(msg.id, msg.filling, msg.name);
        add_button_to_array(msg.id);
      }
      else{
        //console.log("hého hého on rentre du boulot");
        setUpdateButton(msg.id, msg.filling, msg.name);
      }
    break;
    // server alert when a dispenser is dirty
    case "dirty_alert":
      console.log("j'ai reçu une dirty_alert");
      for (var count = 0 ; count < msg.dirty_dispenser.length ; count ++){
        window.alert("the dispenser number " + msg.dirty_dispenser[count] + " is dirty");
      }
    break;
    // answer when the network configuration is called
    case "network":
      console.log("j'ai reçu un network");
      setNetworkWindow(msg.sleep_time, msg.wake_up_time, msg.cycle_time, msg.cleaning_interval_day);
    break;
/*
    case "new":
      console.log("j'ai reçu un nouveau dispenser");
      addNewButton(msg.id, msg.filling, msg.battery, msg.name);
    break;
*/
    default:
      window.alert("c'est la merde, on recoit des types chelous");
  }
}

/********************************************* Envoi de JSON **********************************************/

function send_request_socket(type) {
  var data = new Object();
  data.type = type;
  var string = JSON.stringify(data);
  console.log("envoi de" + string);
  MQTT_send(string);
}

function send_socket_with_id(type, id) {
  var data = new Object();
  data.type = type;
  data.id = id;
  var string = JSON.stringify(data);
  console.log("envoi de" + string);
  MQTT_send(string);
  }

function send_socket_with_id_and_name(type, id, name){
  var data = new Object();
  data.type = type;
  data.id = id;
  data.name = name;
  var string = JSON.stringify(data);
  console.log("envoi de" + string);
  MQTT_send(string);
}

function send_configuration_socket(type, sleep, wake, freq_msg, freq_clean) {
  var data = new Object();
  data.type = type;
  data.sleep_time = sleep;
  data.wake_up_time = wake;
  data.cycle_time = freq_msg;
  data.cleaning_interval_day = freq_clean;
  var string = JSON.stringify(data);
  console.log("envoi de" + string);
  MQTT_send(string);
}

/************************************************** Gestion de l'affichage ***********************************************/

function change_window (current_window, new_window) {
  $("#" + current_window).css('display','none');
  $("#" + new_window).css('display','block');
}
// function called after receiving the detail for a dispenser : it fill the div with new data
function setDetailWindow(id, filling, battery, date, name){
  $("#id_product").val(id);
  $("#filling_data").text(filling);
  if(filling > 20) {
    $("#filling_data").css('background','#FF0000');
  } elseif (filling > 5) {
    $("#filling_data").css('background','orange');
  } else {
    $("#filling_data").css('background','red');
  }
  $("#battery_data").text(battery);
  if(filling > 20) {
    $("#battery_data").css('background','#FF0000');
  } elseif (filling > 5) {
    $("#battery_data").css('background','orange');
  } else {
    $("#battery_data").css('background','red');
  }
  $("#last_washing_date_data").text(date);
  $("#name_product").val(name);
  //console.log("ninja : " + $("#id_product").val(''+100) + " alors qu'on devrait avoir : " + id);
}

// function called after receiving the network configuration : it fill the div with new data
function setNetworkWindow(sleep, wake_up, cycle, day_between_cleaning){
  $("#sleeping_hour_data").val(sleep);
  $("#waking_hour_data").val(wake_up);
  $("#in_between_message_time_data").val(cycle);
  console.log(day_between_cleaning);
  $("#cleaning_frequency_data").val(day_between_cleaning);
}

// function called after receiving the update information for a specified dispenser : it fill the div with new data
function setUpdateButton(id_updated, new_filling, new_name){
  var id_button = "#button_" + id_updated;
  $(id_button).children(".filling_dispenser").text(new_filling);
  //$(id_button).children(".battery_dispenser").text(new_battery);
  $(id_button).children(".name_dispenser").text(new_name);
  parse_button(id_button);
}

//function used at the opening of the page : it parse all the button and change the color in function of the result inside
function parse_button(current_button){
  var filling = parseInt($(current_button).children('.filling_dispenser').text());
  var battery = parseInt($(current_button).children('.battery_dispenser').text());

  if(filling < 20 || battery < 20) {
    $(current_button).css('background-color','orange');
    if (filling < 5) {
      $(current_button).css('background-color','red');
      issue_called("Dispenser "+ $(current_button).children('.id_dispenser').text() + " is nearly empty \n");
    } else if (battery < 1) {
      $(current_button).css('background-color','red');
      issue_called("Dispenser "+ $(current_button).children('.id_dispenser').text() + " battery is nearly empty\n");
    }
  }
}

// function used to make an alert popup
function issue_called(text_for_alert){
  window.alert(text_for_alert);
}


function changePage(id){
  //console.log("heyyyy mon ami, est-ce que tu aimes les patates??");
  send_socket_with_id("ask_detail", parseInt(id));
  change_window("data_window", "detail_window");

}


function addNewButton(id, filling, name) {
  var last_line = $(".line").last();
  var new_button = ""
  //console.log(""+last_line.length);
  //if(last_line.length === 3){
    //$("#corps").append("<div class=\"line\"></div>");
    //last_line = $(".line").last();
  //}
  id_display = "n°" + id;
  filling += " %";

  var space = "<span class=\"space\"></span>";

  var new_button = " <button id = \"button_" + id + "\" class=\"dispenser\" onclick = \"changePage("+id+")\">";
  new_button += " <span class=\"id_dispenser\">" + id_display + "</span><br/>"; // ID
  new_button += " <span class=\"name_dispenser\">" + name + "</span><br/>"; // name
  new_button += " <span class=\"filling_dispenser\">" + filling + "</span></button>";//<span class=\"space\"> </span>"; // filling
  //new_button += "  Batterie : <span class=\"battery_dispenser\">" + battery + "</span><br/>"; // battery
  last_line.append(space);
  last_line.append(new_button);
}

//
// Main function
//
$('.dispenser').each(function(){
  var $this = $(this);
  parse_button($this);
});


/***************************************************** Gestion des clicks ******************************************/

// called when the user click on a dispenser button
/*
$('.dispenser').click(function() {
    send_socket_with_id("ask_detail", parseInt($(this).children(".id_dispenser").text()));
    change_window("data_window", "detail_window");
});
*/
// called when the user click on the network configuration button
$("#network_request").click(function() {
    send_request_socket("ask_network");
    change_window("data_window", "network_window");
});

// called when the user wants to come back to the data window from the network window
$("#data_request_from_network").click(function() {
    send_request_socket("ask_full_update");
    change_window("network_window", "data_window");
});

// called when the user wants to come back to the data window from the detail window
$("#data_request_from_detail").click(function() {
    send_request_socket("ask_full_update");
    change_window("detail_window", "data_window");
});

// function called when the user want to send a new configuration
$("#send_new_configuration").click(function() {
    var sleeping_hour = $("#sleeping_hour_data").val();
    var waking_hour = $("#waking_hour_data").val();
    var between_message = $("#in_between_message_time_data").val();
    var frequency = $("#cleaning_frequency_data").val();
    send_configuration_socket("network_update", parseInt(sleeping_hour), parseInt(waking_hour), parseInt(between_message), parseInt(frequency));
    change_window("network_window", "data_window");
    //TODO verify we get the value
    //TODO change sending function with MQTT
});

// function called when the user want to send a cleaning update
$("#send_cleaning_update").click(function() {
    send_socket_with_id("cleaning_update", parseInt($("#id_product").val()));
    //change_window("detail_window", "data_window");
});

// function called when the user want to send the new name of a product
$("#send_name_update").click(function() {
  send_socket_with_id_and_name("name_update", parseInt($("#id_product").val()), $("#name_product").val());
  //change_window("detail_window", "data_window");
});
