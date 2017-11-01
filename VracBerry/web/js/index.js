var connection = new WebSocket('ws://192.168.50.1:9876');

$(".dispenser").each(parse_button($(this)));

$(".dispenser").click(function() {
  connection.onopen(send_asking_socket("ask_dispenser", "#data_window", "#detail_window", "" + $(this).children(".id_dispenser").text()));
});

$("#network_request").click(function() {
  connection.onopen(send_asking_socket("ask_network", "#data_window", "#network_window"));
});

$("#data_request_from_network").click(function() {
  connection.onopen(send_asking_socket("ask_full_update", "#network_window", "#data_window"));
});

$("#data_request_from_detail").click(function() {
  connection.onopen(send_asking_socket("ask_full_update", "#detail_window", "#data_window"));
});

connection.onmessage = function(event) {
  var id_receive = "";
  var filling_receive = "";
  var battery_receive = "";
  var last_wash_date = "";
  var dirty_dispenser = "";
  var sleep_time = "";
  var wake_up_time = "";
  var cycle_time = "";
  var msg = JSON.parse(event.data);
  switch (msg.type) {
    case "detail_answer":
    id_receive += msg.id;
    filling_receive += msg.filling;
    battery_receive += msg.battery;
    last_wash_date += msg.date;
    setDetailWindow(id_receive, filling_receive, battery_receive, last_wash_date);
    break;
    case "update":
    id_receive += msg.id;
    filling_receive += msg.filling;
    battery_receive += msg.battery;
    setUpdateButton(id_receive, filling_receive, battery_receive);
    break;
    case "dirty_alert":
    for (var count = 0 ; count < msg.dirty_dispenser.length ; count ++){
      window.alert("the dispenser number " + msg.dirty_dispenser[count] + " is dirty");
    }
    break;
    case "network_answer":
    sleep_time += msg.sleep_time;
    wake_up_time += msg.wake_up_time;
    cycle_time += msg.cycle_time;
    setNetworkWindow(sleep_time, wake_up_time, cycle_time);
    break;
    default:
    window.alert("c'est la merde, on recoit des types chelous");
  }
};

function send_asking_socket (type, current_window, new_window){
  var data_send = new Object();
  data_send.data.type = type;

  if(arguments[3] !== undefined) {
    data_send.data.id = "" + arguments[3];
  }
  
  var string = JSON.stringify(data_send);
  connection.send(string);
  $(current_window).css('display','none');
  $(new_window).css('display','block');
}

function setDetailWindow(id, filling, battery, date){
  window.alert("Detail Window not in beta version");
}

function setNetworkWindow(sleep, wake_up, cycle){
  window.alert("Network Window not in beta version");
}

function setUpdateButton(id_updated, new_filling, new_battery){
  var id_button = "#button_" + id_updated;
  $(id_button).children(".filling_dispenser").text(new_filling);
  $(id_button).children(".battery_dispenser").text(new_battery);
  parse_button(id_button);
}

function parse_button(current_button){
  var there_is_an_issue = false;
  var what_are_the_issue = "";
  var filling = parseInt($(current_button).children('.filling_dispenser').text());
  var battery = parseInt($(current_button).children('.battery_dispenser').text());
  if(filling < 20 || battery < 20){
    $(current_button).css('background-color','orange');
    if (filling < 5) {
      what_are_the_issue += "Dispenser "+ $(current_button).children('.id_dispenser').text() + " is nearly empty \n";
      $(current_button).css('background-color','red');
      there_is_an_issue = true;

    } else if (battery < 2) {
      what_are_the_issue += "Dispenser "+ $(current_button).children('.id_dispenser').text() + " battery is nearly empty\n";
      $(current_button).css('background-color','red');
      there_is_an_issue = true;
    }
  }
  if (there_is_an_issue){
    window.alert(what_are_the_issue);
  }
}
