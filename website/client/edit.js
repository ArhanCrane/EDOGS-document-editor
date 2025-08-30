/*const socket = io("http://localhost:8080");

socket.on('connect', (response) => {
    console.log(response);
})*/

/*
socket.on('message', (data) => {
    console.log(data);

    socket.emit('key', 'elements');
});
*/

//import {login} from './greetings.js'
//const socket_login = new WebSocket('ws://localhost:8080/login');
const socket_chat = new WebSocket('ws://localhost:8080/chat');

socket_chat.addEventListener('open', (event) => {
    console.log('WebSocket connection opened:', event);
    socket_chat.send("*" + localStorage.getItem('login') + "#" + localStorage.getItem('DocsName') + "#" + document.getElementById("docs").innerHTML);
    //console.log(localStorage.getItem('entry'));
    
      //localStorage.setItem('entry', event.data);
})
socket_chat.addEventListener('message', (event) => {
  console.log('Message from server:', event.data);
  console.log(event.data);
  if (event.data[0] == '*') {    
    document.getElementById("docs").innerHTML = event.data.slice(1);
  }
});
socket_chat.addEventListener('close', (event) => {
  console.log('WebSocket connection closed:', event);
});
socket_chat.addEventListener('error', (event) => {
  console.error('WebSocket error:', event);
});

document.addEventListener('keyup', (event) => {
  if (event.key.match(/^[a-z]$/i)) {
      document.getElementById("docs").innerHTML += event.key;
      //socket_chat.send(localStorage.getItem('login') + " " + document.getElementById("docs").innerHTML);
      socket_chat.send(localStorage.getItem('login') + "#" + localStorage.getItem('DocsName') + "#" + document.getElementById("docs").innerHTML);
  }else if (event.key == "Backspace") {
      document.getElementById("docs").innerHTML = document.getElementById("docs").innerHTML.slice(0, -1);            
      //socket_chat.send(localStorage.getItem('login') + " " + document.getElementById("docs").innerHTML);
      socket_chat.send(localStorage.getItem('login') + "#" + localStorage.getItem('DocsName') + "#" + document.getElementById("docs").innerHTML);
  };
});

