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

Three websocket handlers: <br>
1. websocket-edit <br>
	path: /char <br>
	method: GET <br>
	Puts values into json file. (login, DocsName, text) and then inserts it into EDOGS.docs or updates the line
2. websocket-entry <br>
	path: /entry <br>
	method: GET <br>
	Stores the login, password, entry_type (login or register) into a json file.
3. websocket-docslist <br>
	path: /docslist <br>
	method: GET <br>
	Returns an array of all documents' names register on a certain login.

---
RoadMap:
1. Implement **Operational Transformation (OT) or Conflict-free Replicated Data Types (CRDTs)**
2. Implement **role-based access control** (e.g., editor, viewer, owner) for documents
3. Consider storing document history for version control and undo/redo functionality
4. Design the backend with **microservices principles** in mind, even if you start as a monolith. Think about how different components (auth, document service, real-time service) could be separated
5. Search functionality for finding documents
6. Implement **caching** (e.g., Redis) for frequently accessed data like active document sessions
