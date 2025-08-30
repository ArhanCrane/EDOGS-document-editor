console.log(localStorage.getItem('DocsList'));



function drawList() {
    const list = document.getElementById('DocsList');

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
        }    
    }
}

drawList();

const buttons = document.getElementsByClassName("my-button");
//console.log(buttons.length);

for (let i = 0; i < buttons.length; i++) {
    buttons[i].addEventListener('click', (event) => {
        EditDocument(event.target.innerText);
        console.log(event.target.innerText);
    });
};

function EditDocument(title) {
    
    localStorage.setItem('DocsName', null);
    localStorage.setItem('DocsName', title);
    window.location.href = "C:/Users/oooks/Desktop/JavaProject/client/edit.html";
    
}


