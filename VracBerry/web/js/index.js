// connection created at the opening of the page
var connection = new WebSocket('ws://192.168.50.1:80');


//function called everytime a socket is received
function on_receive(data);
  var msg = JSON.parse(data);
  switch (msg.type) {
    // is an answer to someone asking for the details of a dispenser
    case "detail_answer":
    setDetailWindow(msg.id, msg.filling, msg.battery, msg.date);
    break;
    // a update sent by the server
    case "update":
    setUpdateButton(msg.id, msg.filling, msg.battery);
    break;
    // server alert when a dispenser is dirty
    case "dirty_alert":
    for (var count = 0 ; count < msg.dirty_dispenser.length ; count ++){
      window.alert("the dispenser number " + msg.dirty_dispenser[count] + " is dirty");
    }
    break;
    // answer when the network configuration is called
    case "network_answer":
    setNetworkWindow(msg.sleep_time, msg.wake_up_time, msg.cycle_time, msg.day_between_cleaning);
    break;
    default:
    window.alert("c'est la merde, on recoit des types chelous");
  }
}

// function tu prepare the sending : depending of the transition, it will hide the old div and make visible the new one
function send_asking_socket (type, current_window, new_window){
  //JSON generation
  var data_send = new Object();
  var data_in_data = {};
  data_in_data.type = type;

  if(arguments[3] !== undefined) {
    data_in_data.id = "" + arguments[3];
  }
  data_send.data = data_in_data;
  var string = JSON.stringify(data_send);
  console.log("before really sending");
  //send
  connection.send(string);
  console.log("before changing window");
  //window changing
  $("#" + current_window).css('display','none');
  $("#" + new_window).css('display','block');
}
// function called after receiving the detail for a dispenser : it fill the div with new data
function setDetailWindow(id, filling, battery, date){
  $("#filling_data").text(filling);
  $("#battery_data").text(battery);
  $("#last_washing_date_data").text(date);
}

// function called after receiving the network configuration : it fill the div with new data
function setNetworkWindow(sleep, wake_up, cycle, day_between_cleaning){
  $("#sleeping_hour_data").val(sleep);
  $("#waking_hour_data").val(wake_up);
  $("#in_between_message_time_data").val(cycle);
  $("#cleaning_frequency_data").val(day_between_cleaning);
}

// function called after receiving the update information for a specified dispenser : it fill the div with new data
function setUpdateButton(id_updated, new_filling, new_battery){
  var id_button = "#button_" + id_updated;
  $(id_button).children(".filling_dispenser").text(new_filling);
  $(id_button).children(".battery_dispenser").text(new_battery);
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
    send_asking_socket("ask_dispenser", "data_window", "detail_window", "" + $(this).children(".id_dispenser").text());
  }
});
// called when the user click on the network configuration button
$("#network_request").click(function() {
  connection.onopen = function(event) {
    send_asking_socket("ask_network", "data_window", "network_window");
  }
});

// called when the user wants to come back to the data window from the network window
$("#data_request_from_network").click(function() {
  connection.onopen = function(event) {
    send_asking_socket("ask_full_update", "network_window", "data_window");
  }
});

// called when the user wants to come back to the data window from the detail window
$("#data_request_from_detail").click(function() {
  connection.onopen = function(event) {
    send_asking_socket("ask_full_update", "detail_window", "data_window");
  }
});

// function called when the user want to send a new configuration
$("#send_new_configuration").click(function() {
  issue_called("not in beta version");
});

// function called when the user want to send a cleaning update
$("#send_cleaning_update").click(function() {
  issue_called("not in beta version");
});

// function called when a socket is received
connection.onmessage = function(event) {
  on_receive(event.data);
};
