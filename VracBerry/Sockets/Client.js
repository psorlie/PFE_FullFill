// Adresse et port dela connexion
//var ws = new WebSocket('ws://192.168.50.1:12345');
var ws = new WebSocket('ws://127.0.0.1:12345');

//Envoi d'une donnée structurée
/*
var data = { "type" : "texte", "message" : "Hello" };
var message = JSON.stringify(data);
ws.send(message);
*/

console.log("Bouhh!!");

//Si la connexion est établie on envoie la chaine de caractères
ws.onopen = function () {      
  //Envoi d'une simple chaine de caractères
	//ws.send("Hello serveur...");   
  console.log("Ouverture de la connexion");
	//console.log('<p class="event">Socket Status: '+ws.readyState+' (open)');
};

// SI on reçoit un message on l'affiche
ws.onmessage=function(event) { 
    /*var data = JSON.parse(event.data);
    switch(data.type)
    {
       case "text":
           document.getElementById("message").innerHTML = data.message;
           break;
    }*/
    console.log("Réception du message") //: " + event.data);
    //document.write('<p class="message">Received: '+msg.data);
    console.log("Fin de communication après réception du message");
};

//Si la connexion est fermée on prévient
ws.onclose = function (event) {      
  console.log("Fin de communication");
  //document.write('<p class="event">Socket Status: '+ws.readyState+' (Closed)');
  ws.close();
};

ws.onerror = function(event){
  console.log("Erreur sur la websocket"); 
  ws.close();
} /*on traite les cas d'erreur*/