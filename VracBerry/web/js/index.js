var connection = new WebSocket('ws://192.168.50.1:80');



function on_receive(data){
  var msg = JSON.parse(data);
  switch (msg.type) {
    case "detail_answer":
    setDetailWindow(msg.id, msg.filling, msg.battery, msg.date);
    break;
    case "update":
    setUpdateButton(msg.id, msg.filling, msg.battery);
    break;
    case "dirty_alert":
    for (var count = 0 ; count < msg.dirty_dispenser.length ; count ++){
      window.alert("the dispenser number " + msg.dirty_dispenser[count] + " is dirty");
    }
    break;
    case "network_answer":
    setNetworkWindow(msg.sleep_time, msg.wake_up_time, msg.cycle_time, msg.day_between_cleaning);
    break;
    default:
    window.alert("c'est la merde, on recoit des types chelous");
  }
}

function send_asking_socket (type, current_window, new_window){
  var data_send = new Object();
  var data_in_data = {};
  data_in_data.type = type;

  if(arguments[3] !== undefined) {
    data_in_data.id = "" + arguments[3];
  }
  data_send.data = data_in_data;
  var string = JSON.stringify(data_send);
  console.log("before really sending");
  connection.send(string);
  console.log("before changing window");
  $("#" + current_window).css('display','none');
  $("#" + new_window).css('display','block');
}

function setDetailWindow(id, filling, battery, date){
  $("#filling_data").text(filling);
  $("#battery_data").text(battery);
  $("#last_washing_date_data").text(date);
}

function setNetworkWindow(sleep, wake_up, cycle, day_between_cleaning){
  $("#sleeping_hour_data").val(sleep);
  $("#waking_hour_data").val(wake_up);
  $("#in_between_message_time_data").val(cycle);
  $("#cleaning_frequency_data").val(day_between_cleaning);
}

function setUpdateButton(id_updated, new_filling, new_battery){
  var id_button = "#button_" + id_updated;
  $(id_button).children(".filling_dispenser").text(new_filling);
  $(id_button).children(".battery_dispenser").text(new_battery);
  parse_button(id_button);
}

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

function issue_called(text_for_alert){
  window.alert(text_for_alert);
}


$(".dispenser").each(function(){
  var $this = $(this);
  parse_button($this);
});

$(".dispenser").click(function() {
  connection.onopen = function(event) {
    send_asking_socket("ask_dispenser", "data_window", "detail_window", "" + $(this).children(".id_dispenser").text());
  }
});

$("#network_request").click(function() {
  connection.onopen = function(event) {
    send_asking_socket("ask_network", "data_window", "network_window");
  }
});

$("#data_request_from_network").click(function() {
  connection.onopen = function(event) {
    send_asking_socket("ask_full_update", "network_window", "data_window");
  }
});


$("#data_request_from_detail").click(function() {
  connection.onopen = function(event) {
    send_asking_socket("ask_full_update", "detail_window", "data_window");
  }
});

$("#send_new_configuration").click(function() {
  issue_called("not in beta version");
});
$("#send_cleaning_update").click(function() {
  issue_called("not in beta version");
});
connection.onmessage = function(event) {
  on_receive(event.data);
};
