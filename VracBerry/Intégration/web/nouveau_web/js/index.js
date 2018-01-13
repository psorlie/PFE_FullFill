// connection created at the opening of the page
var connection = new WebSocket( "ws://192.168.50.86:80", "example-protocol" );


//function called everytime a socket is received
function on_receive(data) {
  var msg = JSON.parse(data);
  switch (msg.type) {
    // is an answer to someone asking for the details of a dispenser
    case "detail":
    setDetailWindow(msg.id, msg.filling, msg.battery, msg.date, msg.name);
    break;
    // a update sent by the server
    case "update":
    setUpdateButton(msg.id, msg.filling, msg.battery, msg.name);
    break;
    // server alert when a dispenser is dirty
    case "dirty_alert":
    for (var count = 0 ; count < msg.dirty_dispenser.length ; count ++){
      window.alert("the dispenser number " + msg.dirty_dispenser[count] + " is dirty");
    }
    break;
    // answer when the network configuration is called
    case "network":
    setNetworkWindow(msg.sleep_time, msg.wake_up_time, msg.cycle_time, msg.day_between_cleaning);
    break;

    case "new":
    addNewButton(msg.id, msg.filling, msg.battery, msg.name);
    break;

    default:
    window.alert("c'est la merde, on recoit des types chelous");
  }
}

// function tu prepare the sending : depending of the transition, it will hide the old div and make visible the new one
function send_socket (text) {
  console.log("before really sending");
  connection.send(text);
  console.log("after_sending");
}

function send_request_socket(type) {
  var data = new Object();
  data.type = type;
  var string = JSON.stringify(data);
  send_socket(string);
}

function send_socket_with_id(type, id) {
var data = new Object();
data.type = type;
data.id = id;
var string = JSON.stringify(data);
send_socket(string);
}

function send_socket_with_id_and_name(type, id, name){
  var data = new Object();
  data.type = type;
  data.id = id;
  data.name = name;
  var string = JSON.stringify(data);
  send_socket(string);
}

function send_configuration_socket(sleep, wake, freq_msg, freq_clean) {
  var data = new Object();
  data.type = type;
  data.sleep_time = sleep_time;
  data.wake_up_time = wake;
  data.cycle_time = freq_msg;
  data.cleaning_interval_day = freq_clean;
  var string = JSON.stringify(data);
  send_socket(string);
}

function change_window (current_window, new_window) {
  $("#" + current_window).css('display','none');
  $("#" + new_window).css('display','block');
}
// function called after receiving the detail for a dispenser : it fill the div with new data
function setDetailWindow(id, filling, battery, date, name){
  $("#filling_data").text(filling);
  $("#battery_data").text(battery);
  $("#last_washing_date_data").text(date);
  $("#name_product").val(name);
}

// function called after receiving the network configuration : it fill the div with new data
function setNetworkWindow(sleep, wake_up, cycle, day_between_cleaning){
  $("#sleeping_hour_data").val(sleep);
  $("#waking_hour_data").val(wake_up);
  $("#in_between_message_time_data").val(cycle);
  $("#cleaning_frequency_data").val(day_between_cleaning);
}

// function called after receiving the update information for a specified dispenser : it fill the div with new data
function setUpdateButton(id_updated, new_filling, new_battery, new_name){
  var id_button = "#button_" + id_updated;
  $(id_button).children(".filling_dispenser").text(new_filling);
  $(id_button).children(".battery_dispenser").text(new_battery);
  $(id_button).children(".name_dispenser").text(new_name);
  parse_button(id_button);
}

//function used at the opening of the page : it parse all the button and change the color in function of the result inside
function parse_button(current_button){
  var filling = parseInt($(current_button).children('.filling_dispenser').text());
  var battery = parseInt($(current_button).children('.battery_dispenser').text());

  if(filling < 20 || battery < 20){
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

function addNewButton(id, filling, battery, name) {
  var last_line = $(".line").last();
  var new_button = ""
  if(last_line.length() === 3)
    $("#corps").append("<div class=\"line\"></div>");
    last_line = $(".line").last();
  }
  var new_button = "<span class=\"space\"></span>";
  new_button += "<button id = \"button_id" + id + "\" class=\"dispenser\">";
  new_button += " ID : <span class=\"id_dispenser\">" + id + "</span><br/>"; // ID
  new_button += " Remplissage : <span class=\"filling_dispenser\">" + filling + "</span><br/>"; // filling
  new_button += "  Batterie : <span class=\"battery_dispenser\">" + battery + "</span><br/>"; // battery
  new_button += " Produit : <span class=\"name_dispenser\">" + name + "</span></button><span class=\"space\"> </span>"; // name
  last_line.append(new_button);
}

//
// Main function
//
$(".dispenser").each(function(){
  var $this = $(this);
  parse_button($this);
});


// called when the user click on a dispenser button
$(".dispenser").click(function() {
  connection.onopen = function(event) {
    send_socket_with_id("ask_detail", "" + $(this).children(".id_dispenser").text());
    change_window("data_window", "detail_window");
  }
});
// called when the user click on the network configuration button
$("#network_request").click(function() {
  connection.onopen = function(event) {
    send_request_socket("ask_network");
    change_window("data_window", "network_window");
  }
});

// called when the user wants to come back to the data window from the network window
$("#data_request_from_network").click(function() {
  connection.onopen = function(event) {
    send_request_socket("ask_full_update");
    change_window("network_window", "data_window");
  }
});

// called when the user wants to come back to the data window from the detail window
$("#data_request_from_detail").click(function() {
  connection.onopen = function(event) {
    send_request_socket("ask_full_update");
    change_window("detail_window", "data_window");
  }
});

// function called when the user want to send a new configuration
$("#send_new_configuration").click(function() {
  connection.onopen = function(event) {
    var sleeping_hour = $("#sleeping_hour_data").val();
    var waking_hour = $("#waking_hour_data").val();
    var between_message = $("#in_between_message_time_data").val();
    var frequency = $("#cleaning_frequency_data").val();
    send_configuration_socket("network_update", sleeping_hour, waking_hour, between_message, frequency);
    change_window("network_window", "data_window");
    //TODO verify we get the value
    //TODO change sending function with MQTT
  }
});

// function called when the user want to send a cleaning update
$("#send_cleaning_update").click(function() {
  connection.onopen = function(event) {
    send_socket_with_id("cleaning_update", $("#id_product").val());
    change_window("detail_window", "data_window");
  }
});

// function called when the user want to send the new name of a product
$("#send_name_update").click(function() {
  //TODO   verify we get the value
  send_socket_with_id_and_name("name_update", $("#id_product").val(), $("#name_product").val(););
  change_window("detail_window", "data_window");
});

// function called when a socket is received
connection.onmessage = function(event) {
  on_receive(event);
};
