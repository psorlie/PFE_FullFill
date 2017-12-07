function update(msg) { 
    $("2").children(".filling_dispenser").text(msg);
}
  
  socket.onopen = function() { 
      console.log("socket open"); 
      //update("open"); 
    }
  
  socket.onclose = function() { 
      console.log("socket close"); 
      //update("closed"); 
    }

  socket.onmessage = function(msg) { 
      console.log("socket message: " + msg.data); 
      update(msg.data); 
    }

