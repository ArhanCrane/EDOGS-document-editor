EDOGS document editor

---
Components:
- Website - html/css, js, Socket.io
- API - Userver, postgresql, websockets
---

### Website

Three pages:
1. greetings
	<p align="center"><img src="pictures/greetings.html.png"></p>
2. doc_list
	<p align="center"><img src="pictures/doc_list.html.png"></p>
3. edit
	<p align="center"><img src="pictures/edit.html.png"></p>
### API

Two websocket handlers:
1. websocket-edit
	path: /char
	method: GET
	Puts values into json file. (login, DocsName, text) and then inserts it into EDOGS.docs or updates the line
2. websocket-entry
	path: /entry
	method: GET
	Stores the login, password, entry_type (login or register) into a json file.
