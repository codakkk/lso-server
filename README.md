# lso-server

Message types:

0: Room List

- n utenti (client) possono accedere ad un sistema di chat
- gli utenti devono potersi registrare e loggare al server
- registrazione può essere sviluppata tramite database sql o file
- gli utenti possono create stanze e possono chattare solo con gli utenti presenti nella stanza
- gli utenti possono accedere a stanze già create
- il creatore della stanza deve poter accettare un utente che si vuole aggiungere alla stanza
- [Opzionale] Per una complessità maggiore, e quindi punti extra, si può:
  Usare un database Mysql or PostgreSQL
