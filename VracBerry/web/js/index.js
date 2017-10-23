var there_is_an_issue = false;
var what_are_the_issue = "";

$(".dispenser").each(function(){
  var filling = parseInt($(this).children('.filling_dispenser').text());
  var battery = parseInt($(this).children('.battery_dispenser').text());
  if(filling < 20 || battery < 20){
    $(this).css('background-color','orange');
    if (filling < 5) {
      what_are_the_issue += "Dispenser "+ $(this).children('.id_dispenser').text() + " is nearly empty \n";
      $(this).css('background-color','red');
      there_is_an_issue = true;

    } else if (battery < 2) {
      what_are_the_issue += "Dispenser "+ $(this).children('.id_dispenser').text() + " battery is nearly empty\n";
      $(this).css('background-color','red');
      there_is_an_issue = true;
    }
  }
});

if (there_is_an_issue){
  window.alert(what_are_the_issue);
}

var connection = new WebSocket('ws://192.168.50.1:9876');
$("button").click(function() {
  connection.onopen = function () {
    var data_send = "ask_dispenser" + $(this).children(".id_dispenser").text();
    connection.send(data_send); // Send the message 'Ping' to the server
    console.log(data_send);

    var io = require('socket.io').listen(80);
    io.sockets.on('connection', function (socket) {
      socket.on('join', function (data) {
        socket.join(data.email); // We are using room of socket io
      });
    });
  };
});
