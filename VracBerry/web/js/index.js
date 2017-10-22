$(".dispenser").each(function(){
  var filling = parseInt($(this).children('.filling_dispenser').text());
  var battery = parseInt($(this).children('.battery_dispenser').text());
  if(filling < 20 || battery < 20){
    $(this).css('background-color','orange');
    if (filling < 5) {
      $("p",".popup").text("Dispenser "+ $(this).children('.id_dispenser').text() + " is nearly empty");
      $(".popup").css("display","inline");
      $(this).css('background-color','red');
    } else if (battery < 2) {
      $("p",".popup").text("Dispenser "+ $(this).children('.id_dispenser').text() + " battery is nearly empty");
      $(".popup").css("display","inline");
      $(this).css('background-color','red');
    }
  }
});
