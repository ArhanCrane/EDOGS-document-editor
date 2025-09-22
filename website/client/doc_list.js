//console.log(localStorage.getItem('DocsList'));
const socket_chat = new WebSocket('ws://localhost:34313/docslist');

socket_chat.addEventListener('open', (event) => {
    console.log('WebSocket connection opened:', event);
    socket_chat.send(localStorage.getItem('login'));
    //console.log(localStorage.getItem('login'));
})
socket_chat.addEventListener('message', (event) => {
  console.log('Message from server:', event.data);
  localStorage.setItem('DocsList', event.data);
  drawList();
});
socket_chat.addEventListener('close', (event) => {
  console.log('WebSocket connection closed:', event);
});
socket_chat.addEventListener('error', (event) => {
  console.error('WebSocket error:', event);
});


function drawList() {
    const list = document.getElementById('DocsList');
    console.log("drawList()");
    let DocsNames = [];    
    var line = "";
    for (let i = 1; i < localStorage.getItem('DocsList').length; i++) {
        //console.log(line);
        if (localStorage.getItem('DocsList')[i] == '#') {            
            DocsNames.push(line);
            line = "";
            i++;         
        }
        line += localStorage.getItem('DocsList')[i];
    }
    DocsNames.push(line);
    
    //console.log(DocsNames);
    if (DocsNames[0] != "ull") {

        for (let i = 0; i < DocsNames.length; i++) {            
            const line = document.createElement('button');
    
            line.innerText = DocsNames[i];
            line.className = "my-button";              
    
            line.style.position = 'absolute';
    
            line.style.width = 500 + 'px';
            line.style.height = 50 + 'px';
    
            line.style.left = 0 + 'px';
            line.style.top = 200 + ((60) * i) + 'px';        
            
            line.style.backgroundColor = 'grey';
    
            list.appendChild(line);
            console.log(line.className);
        }    
    }

    const buttons = document.getElementsByClassName("my-button");

    for (let i = 0; i < buttons.length; i++) {
        buttons[i].addEventListener('click', (event) => {
            console.log("click!");
            EditDocument(event.target.innerText);
            console.log(event.target.innerText);
        });
    };
}

function EditDocument(title) {    
    localStorage.setItem('DocsName', null);
    localStorage.setItem('DocsName', title);
    window.location.href = "C:/Users/oooks/Desktop/website/client/edit.html";
}


