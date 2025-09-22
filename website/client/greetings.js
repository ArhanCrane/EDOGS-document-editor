const logSubmission = document.getElementById('login_submission');
const regSubmission = document.getElementById('register_submission');

const socket_chat = new WebSocket('ws://localhost:34313/entry');

socket_chat.addEventListener('open', (event) => {
  //localStorage.setItem('DocsList', null);
  //localStorage.setItem('DocsList', null);
    console.log('WebSocket connection opened:', event);
})
socket_chat.addEventListener('message', (event) => {

  console.log('Message from server:', event.data);
  if (event.data == "ACCESS") {
    
    /*if (event.data != "ACCESS") {
      localStorage.setItem('DocsList', null);
      localStorage.setItem('DocsList', event.data);
    }*/
    
    window.location.href = "C:/Users/oooks/Desktop/website/client/doc_list.html";
      //console.log(login);
  }

});
socket_chat.addEventListener('close', (event) => {
  console.log('WebSocket connection closed:', event);
});
socket_chat.addEventListener('error', (event) => {
  console.error('WebSocket error:', event);
});

logSubmission.addEventListener('submit', function(event) {    
    event.preventDefault();
    submitLoginPassword();
});

regSubmission.addEventListener('submit', function(event) {    
    event.preventDefault();
    submitRegPassword();
});

function submitLoginPassword() {    
    
    localStorage.setItem('login', null);
    localStorage.setItem('login', document.getElementById("nameText").value);

    socket_chat.send(document.getElementById("nameText").value + " " + document.getElementById("passwordText").value + " " + "LOG");
    //console.log(document.getElementById("nameText").value + " " + document.getElementById("passwordText").value);
};

function submitRegPassword() {    
    localStorage.setItem('login', null);
    localStorage.setItem('login', document.getElementById("nameText").value);
    
    socket_chat.send(document.getElementById("nameText").value + " " + document.getElementById("passwordText").value + " " + "REG");
    //console.log(document.getElementById("nameText").value + " " + document.getElementById("passwordText").value);
};
