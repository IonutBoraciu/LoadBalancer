**Nume: Boraciu Ionuț Sorin**
**Grupă: 315CA**

## Load Balancer: Tema 2

### Descriere:

In cazul în care este adăugat un server nou, dupa ce se creează hashtable-urile pentru server și replicile sale se verifică
dacă trebuie mutate date deja existente pe alte servere pe acestea, sau dacă trebuie mutate elemente de pe primul server
pe ultimul, întrucât acesta este un hash-ring. Apoi elementele sunt sortate crescator dupa key fiecărui server in parte, sau dupa
id-ul serverului daca au key egală. La final se mai verifică o dată dacă se pot muta date de pe primul element din vector pe ultimul.

La eliminarea unui server, se caută mai întâi server-ul original care trebuie mutat, se calculeaza pozitia pe care trebuie
mutate elementele, și se mută, apoi se aplică aceași strategie asupra replicilor acestuia, urmând ca la final fiecare
hashtable in parte sa fie eliberat si eliminant din vector.

De asemenea, vectorul de hashtable-uri este alocat dinamic cu puteri ale lui 2 începând cu 2 la puterea a 3 a, iar dacă
sunt eliminate prea multe elemente din vector, atunci acesta este realocat la o putere mai mică a lui 2.

### Comentarii asupra temei:

* Crezi că ai fi putut realiza o implementare mai bună?
As fi putut fi mai eficient, mai ales la eliminarea unui server, unde căutările sunt reduntante.
* Ce ai invățat din realizarea acestei teme?
Am învatat sa stăpânesc mai bine conceptele de hashtable, si m-am familiarizat au acestea.
