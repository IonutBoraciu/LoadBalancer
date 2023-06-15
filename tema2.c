/* Copyright 2023 <> */
// Boraciu Ionut Sorin
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define REQUEST_LENGTH 1024
#define KEY_LENGTH 128
#define VALUE_LENGTH 65536
#define DIE(assertion, call_description)                                       \
    do {                                                                       \
        if (assertion) {                                                       \
            fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                 \
            perror(call_description);                                          \
            exit(errno);                                                       \
        }                                                                      \
    } while (0)
// Aici incepe codul din laborator folosit
// pentru a implementa listele si hashtable-ul
typedef struct info info;
struct info {
	void *key;
	void *value;
};
typedef struct ll_node_t
{
    void* data;
    struct ll_node_t* next;
} ll_node_t;

typedef struct linked_list_t
{
    ll_node_t* head;
    unsigned int data_size;
    unsigned int size;
} linked_list_t;

linked_list_t *ll_create(unsigned int data_size)
{
    linked_list_t* ll;

    ll = malloc(sizeof(*ll));
    DIE(!ll, "ll_create failed\n");
    ll->head = NULL;
    ll->data_size = data_size;
    ll->size = 0;

    return ll;
}

void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    ll_node_t *prev, *curr;
    ll_node_t* new_node;

    if (!list) {
        return;
    }
    if (n > list->size) {
        n = list->size;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(*new_node));
    DIE(!new_node, "new_node malloc failed\n");
    new_node->data = malloc(list->data_size);
    DIE(!new_node->data, "data for node malloc failed\n");
    memcpy(new_node->data, new_data, list->data_size);

    new_node->next = curr;
    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}
ll_node_t *ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    ll_node_t *prev, *curr;

    if (!list || !list->head) {
        return NULL;
    }

    if (n > list->size - 1) {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL) {
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    list->size--;

    return curr;
}

unsigned int ll_get_size(linked_list_t* list)
{
     if (!list) {
        return -1;
    }

    return list->size;
}

void ll_free(linked_list_t** pp_list)
{
    ll_node_t* currNode;

    if (!pp_list || !*pp_list) {
        return;
    }

    while (ll_get_size(*pp_list) > 0) {
        currNode = ll_remove_nth_node(*pp_list, 0);
        free(currNode->data);
        currNode->data = NULL;
        free(currNode);
        currNode = NULL;
    }

    free(*pp_list);
    *pp_list = NULL;
}

void ll_print_int(linked_list_t* list)
{
    ll_node_t* curr;

    if (!list) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%d ", *((int*)curr->data));
        curr = curr->next;
    }

    printf("\n");
}

void ll_print_string(linked_list_t* list)
{
    ll_node_t* curr;

    if (!list) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%s ", (char*)curr->data);
        curr = curr->next;
    }

    printf("\n");
}



typedef struct hashtable_t hashtable_t;
struct hashtable_t {
	linked_list_t **buckets;
	unsigned int size;
	unsigned int hmax;
};

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *)a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

hashtable_t *ht_create(unsigned int hmax)
{
	hashtable_t *hash;
	hash = malloc(sizeof(hashtable_t));
    DIE(!hash, "malloc for hash failed\n");
    hash->buckets = malloc(sizeof(linked_list_t*)*hmax);
    DIE(!hash->buckets, "malloc for buckets * failed\n");
    for (int i = 0; i < hmax; i++) {
        hash->buckets[i] = ll_create(sizeof(info));
        DIE(!hash->buckets[i], "malloc for hash buckets i failed\n");
    }
    hash->hmax = hmax;
    return hash;
}

int ht_has_key(hashtable_t *ht, void *key)
{
	int poz = hash_function_key(key);
	poz = poz%ht->hmax;
	if (ht->buckets[poz]->head != NULL) {
       return 1;
    }
    return 0;
}

int compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}
void *ht_get(hashtable_t *ht, void *key)
{
	unsigned int hash_value = hash_function_key(key)% ht->hmax;
	ll_node_t *node = ht->buckets[hash_value]->head;
	info *info_node;
	if (node == NULL) {
		return NULL;
    } else {
		for (int i = 0; i < ht->buckets[hash_value]->size; i++) {
			info_node = (info*) node->data;
            if (compare_function_strings(key, info_node->key) == 0) {
				return info_node->value;
            }
			node = node->next;
		}
	}
	return NULL;
}

void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
    unsigned int poz;
	poz = hash_function_key(key);
	poz = poz%ht->hmax;
    char *copy;
    copy = malloc(key_size);
    DIE(!copy, "copy failed how?\n");
    memcpy(copy, key, key_size);
    info data;
    data.key = malloc(key_size);
    DIE(!data.key, "data.key failed..somehow\n");
    data.value = malloc(value_size);
    DIE(!data.value, "data.value failed\n");
    memcpy(data.key, copy, key_size);
    memcpy(data.value, value, value_size);
    free(copy);
    ll_add_nth_node(ht->buckets[poz], 0, &data);
    ht->size++;
}

void ht_remove_entry(hashtable_t *ht, void *key)
{
    unsigned int hash_value = hash_function_key(key) % ht->hmax;
    ll_node_t *prev_node = NULL;
    ll_node_t *curr_node = ht->buckets[hash_value]->head;

    while (curr_node != NULL) {
        info *info_node = (info*) curr_node->data;
        if (compare_function_strings(key, info_node->key) == 0) {
            if (prev_node == NULL) {
                ht->buckets[hash_value]->head = curr_node->next;
            } else {
                prev_node->next = curr_node->next;
            }
            free(info_node->key);
            free(info_node->value);
            free(info_node);
            free(curr_node);
            ht->size--;
            ht->buckets[hash_value]->size--;
            return;
        }
        prev_node = curr_node;
        curr_node = curr_node->next;
    }
}

void ht_free(hashtable_t *ht)
{
    linked_list_t* bucket;
    ll_node_t* node;
    for (int i = 0; i < ht->hmax; i++) {
        bucket = ht->buckets[i];
        node = bucket->head;
        while (node) {
            info* info = node->data;
            free(info->value);
            free(info->key);
            node = node->next;
        }
        ll_free(&bucket);
    }
    free(ht->buckets);
    free(ht);
}


/*



  Aici incepe rezolvarea temei.




*/

struct server_memory {
    // variabila pentru hashtable
	hashtable_t *server_hash;
    // memoreaza id-ul primei replici
    int replica1;
    // memoreaza id-ul la a doua replica
    int replica2;
    // id-ul serverului initial
    int server_id;
    // variabila pentru a verifica daca este replica sau nu
    int is_replica;
};

typedef struct server_memory server_memory;

struct load_balancer {
    // retin key fiecarui server
    unsigned int server_key;
    // datele din fiecare server
	server_memory server;
};

struct load_balancer;

typedef struct load_balancer load_balancer;

unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}



server_memory *init_server_memory()
{
	return NULL;
}

void server_store(server_memory *server, char *key, char *value) {
}

char *server_retrieve(server_memory *server, char *key) {
	return NULL;
}

void server_remove(server_memory *server, char *key) {
}

void free_server_memory(server_memory *server) {
}

load_balancer *init_load_balancer() {
    // initializez vectorul de hashtable-uri cu 8 elemente initial 2^3
    // si il voi realloca cu puteri ale lui doi
    // si dealoca daca s-au eliminat suficient de multe servere
    load_balancer *load;
	load = malloc(8*sizeof(load_balancer));
    DIE(!load, "malloc for load failed\n");
    return load;
}

void loader_add_server(load_balancer *main, int server_id, int *nr_server)
{
    // Initializez hashtable-ul pentru server-ul original
    unsigned int server_key = hash_function_servers(&server_id);
    main[*nr_server].server_key = server_key;
    unsigned int key = hash_function_servers(&server_id);
    main[*nr_server].server.server_hash = ht_create(2000);

    // Calculez replicile servere-ului si creez hashtable-urile respective
    int replica1 = 100000 + server_id;
    int replica2 = 100000 * 2 + server_id;
    main[*nr_server].server.replica1 = replica1;
    main[*nr_server].server.replica2 = replica2;
    main[*nr_server].server.is_replica = 0;

    unsigned int key1 = hash_function_servers(&replica1);
    unsigned int key2 = hash_function_servers(&replica2);
    main[*nr_server].server.server_id = server_id;
    main[*nr_server+1].server_key = key1;
    main[*nr_server+1].server.server_hash = ht_create(2000);
    main[*nr_server+2].server_key = key2;
    main[*nr_server+2].server.server_hash = ht_create(2000);
    main[*nr_server+1].server.server_id = server_id;
    main[*nr_server+2].server.server_id = server_id;

    main[*nr_server+1].server.is_replica = 1;
    main[*nr_server+2].server.is_replica = 1;
    *nr_server = *nr_server+2;

    // Verific daca datele dintr-un hashtable existent
    // se pot muta pe server-ul nou adaugat
  for (int i=0; i <= *nr_server; i++)
    {
        for (int j =0; j < main[i].server.server_hash->hmax; j++)
        {
            if (main[i].server.server_hash->buckets[j] != NULL)
            {
                // Ma plimb prin fiecare element din bucket si
                // verific daca cheia acestuia
                // este mai mica decat key serverului adaugat
                // si daca ar fi in ordine dupa sortare
                if (main[i].server.server_hash->buckets[j]->head != NULL) {
                info *test = main[i].server.server_hash->buckets[j]->head->data;
                unsigned int real_key = hash_function_key(test->key);

                // In primele 3 if-uri verific daca un
                // datele de la un server pot fi mutate
                // pe server-ul anterior
                if (main[*nr_server-2].server_key > real_key &&
                    main[*nr_server-2].server_key < main[i].server_key &&
                    (i == 0 || real_key > main[i-1].server_key))
                {
                    ht_put(main[*nr_server-2].server.server_hash, test->key,
                        strlen((char*)test->key)+1, test->value,
                        strlen((char*)test->value)+1);
                    ht_remove_entry(main[i].server.server_hash, test->key);
                }
                else if (main[*nr_server].server_key > real_key &&
                    main[*nr_server].server_key < main[i].server_key &&
                    (i == 0 || real_key > main[i-1].server_key))
                {
                    ht_put(main[*nr_server].server.server_hash, test->key,
                    strlen((char*)test->key)+1, test->value,
                    strlen((char*)test->value)+1);
                    ht_remove_entry(main[i].server.server_hash, test->key);
                }
                else if (main[*nr_server-1].server_key > real_key &&
                    main[*nr_server-1].server_key < main[i].server_key &&
                    (i == 0 || real_key > main[i-1].server_key))
                {
                    ht_put(main[*nr_server-1].server.server_hash, test->key,
                    strlen((char*)test->key)+1, test->value,
                    strlen((char*)test->value)+1);
                    ht_remove_entry(main[i].server.server_hash, test->key);
                }
                // In urmatoarele 3 if-uri verific daca datele
                // de pe primul server
                // Pot fi mutate pe ultimul acesta fiind un vector circular
                if (i == 0 && main[*nr_server-2].server_key <
                    main[0].server_key &&
                    (real_key <= main[*nr_server-2].server_key ||
                    real_key > main[0].server_key))
                {
                    ht_put(main[*nr_server-2].server.server_hash, test->key,
                    strlen((char*)test->key)+1, test->value,
                    strlen((char*)test->value)+1);
                    ht_remove_entry(main[0].server.server_hash, test->key);
                }
                else if (i == 0 && main[*nr_server-1].server_key <
                    main[0].server_key &&
                    (real_key <= main[*nr_server-1].server_key ||
                    real_key > main[0].server_key))
                {
                    ht_put(main[*nr_server-1].server.server_hash, test->key,
                    strlen((char*)test->key)+1, test->value,
                    strlen((char*)test->value)+1);
                    ht_remove_entry(main[0].server.server_hash, test->key);
                }
                else if (i == 0 && main[*nr_server].server_key <
                    main[0].server_key &&
                    (real_key <= main[*nr_server].server_key ||
                    real_key > main[0].server_key) && server_id)
                {
                    ht_put(main[*nr_server].server.server_hash, test->key,
                    strlen((char*)test->key)+1, test->value,
                    strlen((char*)test->value)+1);
                    ht_remove_entry(main[0].server.server_hash, test->key);
                }
            }
            }
        }
    }
    // Sortez elementele in functie de key sau in functie de server_id
    // In cazul in care 2 servere au aceasi key
    for (int i=0; i <= *nr_server-1; i++)
        for (int j=i+1; j <= *nr_server; j++)
           if (main[i].server_key > main[j].server_key)
            {
               load_balancer tmp;
                memcpy(&tmp, &main[i], sizeof(load_balancer));
                memcpy(&main[i], &main[j], sizeof(load_balancer));
                memcpy(&main[j], &tmp, sizeof(load_balancer));
            }
            else if (main[i].server_key == main[j].server_key &&
                main[i].server.server_id > main[j].server.server_id)
            {
               load_balancer tmp;
                memcpy(&tmp, &main[i], sizeof(load_balancer));
                memcpy(&main[i], &main[j], sizeof(load_balancer));
                memcpy(&main[j], &tmp, sizeof(load_balancer));
            }
        // Dupa ce au fost sortate elementele mai verific o data daca
        // informatiile de pe primul server pot fi mutate pe ultimul
        for (int j=0; j < main[0].server.server_hash->hmax; j++)
            {
                if (main[0].server.server_hash->buckets[j]->head != NULL)
                {
                    info *data;
                    data = main[0].server.server_hash->buckets[j]->head->data;
                    unsigned int key = hash_function_key(data->key);
                    if ((key < main[*nr_server].server_key) &&
                        key > main[*nr_server-1].server_key)
                        {
                            ht_put(main[*nr_server].server.server_hash,
                                data->key,
                                strlen((char*)data->key)+1, data->value,
                                strlen((char*)data->value)+1);
                            ht_remove_entry(main[0].server.server_hash,
                                data->key);
                        }
                }
            }
}
void loader_remove_server(load_balancer *main, int server_id, int nr_server)
{
    // Calculez key serverului care trebuie sters
    unsigned int key = hash_function_servers(&server_id);
    // Caut server-ul cu key respectiva
    for (int i=0; i < nr_server; i++)
    {
        if (key == main[i].server_key)
        {
            int poz1 = 0, poz2 = 0, poz3 = 0;
            // Calculez poiztia pe care trebuie mutate elementele
            for (int j = i+1; j < nr_server; j++)
                if (main[i].server.server_id != main[j].server.server_id)
                    {
                        poz1 = j;
                        break;
                    }
            // Mut elementele din acesta pe pozitia gasita
            for (int j =0; j < main[i].server.server_hash->hmax; j++)
                {
                    if (main[i].server.server_hash->buckets[j]->head != NULL)
                    {
                        info *data =
                        main[i].server.server_hash->buckets[j]->head->data;
                        ht_put(main[poz1].server.server_hash, data->key,
                        strlen((char*)data->key)+1, data->value,
                        strlen((char*)data->value)+1);
                    }
                }
            // Calculez key-le celor 2 replici
            int sv_id1, sv_id2;
            sv_id1 = main[i].server.replica1;
            sv_id2 = main[i].server.replica2;
            unsigned int key1, key2;
            key1 = hash_function_servers(&sv_id1);
            key2 = hash_function_servers(&sv_id2);
            int ind1 = 0, ind2 = 0;
            // Caut cele replicile si salvez indicii acestora
            for (int j = 0; j < nr_server; j++)
            {
                    if (main[j].server_key == key1)
                        {
                            ind1 = j;
                            break;
                        }
            }
            for (int j =0; j < nr_server; j++)
            {
                if (main[j].server_key == key2)
                {
                    ind2 = j;
                    break;
                }
            }
            // Apoi caut pozitia pe care ar trebui sa fie mutate elementele
            for (int j = ind1+1; j < nr_server; j++)
                if (main[i].server.server_id != main[j].server.server_id)
                    {
                        poz2 = j;
                        break;
                    }
            for (int j = ind2+1; j < nr_server; j++)
                if (main[i].server.server_id != main[j].server.server_id)
                    {
                        poz3 = j;
                        break;
                    }
            // Urmeaza sa mut elementele din replici pe pozitiile gasite
            for (int j =0; j < main[ind1].server.server_hash->hmax; j++)
                {
                    if (main[ind1].server.server_hash->buckets[j]->head != NULL)
                    {
                        info *data =
                        main[ind1].server.server_hash->buckets[j]->head->data;
                        ht_put(main[poz2].server.server_hash, data->key,
                            strlen((char*)data->key)+1, data->value,
                            strlen((char*)data->value)+1);
                    }
                }
            for (int j =0; j < main[ind2].server.server_hash->hmax; j++)
                {
                    if (main[ind2].server.server_hash->buckets[j]->head != NULL)
                    {
                        info *data =
                        main[ind2].server.server_hash->buckets[j]->head->data;
                        ht_put(main[poz3].server.server_hash, data->key,
                            strlen((char*)data->key)+1, data->value,
                            strlen((char*)data->value)+1);
                    }
                }
            // Eliberez hashtable-ul server-ului si al replicilor
            // Apoi mut scot server-ul si replicile din vector
            ht_free(main[i].server.server_hash);
              for (int j = i; j < nr_server-1; j++)
                {
                    memcpy(&main[j], &main[j+1], sizeof(load_balancer));
                }
            if (ind1 >= i)
            ind1--;
            if (ind2 >= i)
                ind2--;
            nr_server--;
            ht_free(main[ind1].server.server_hash);
            for (int j = ind1; j < nr_server-1; j++)
                {
                    memcpy(&main[j], &main[j+1], sizeof(load_balancer));
                }
            nr_server--;
            if (ind2 > ind1)
                ind2--;
            ht_free(main[ind2].server.server_hash);
            for (int j = ind2; j < nr_server-1; j++)
                {
                    memcpy(&main[j], &main[j+1], sizeof(load_balancer));
                }
            nr_server--;
            break;
        }
    }
}

void loader_store(load_balancer *main, char *key,
    char *value, int *server_id, int nr_server)
{
    // Calculez key pentru sirul de caractere primit
    unsigned int  key2 = hash_function_key(key);
    int ok = 0;
    // La primul server gasit care este mai mare
    // decat key calculata pun sirul de caractere
    // pe el
    for (int i = 0; i < nr_server; i++)
    {
        if (main[i].server_key > key2)
            {
                ok = 1;
                *server_id = main[i].server.server_id;
                ht_put(main[i].server.server_hash, key,
                strlen(key)+1, value, strlen(value)+1);
                break;
            }
    }
    // Daca nu am gasit nici un element
    // care sa respecte proprietate o pun
    // pe primul server
    if (!ok)
    {
        *server_id = main[0].server.server_id;
        ht_put(main[0].server.server_hash, key,
            strlen(key)+1, value, strlen(value)+1);
    }
}

char *loader_retrieve(load_balancer *main, char *key,
    int *server_id, int nr_server)
{
    int ok = 0;
    for (int i = 0; i < nr_server; i++)
    {
        ok = 1;
        *server_id = main[i].server.server_id;
        // Ma folosesc de functia ht_get pentru a vedea
        // unde se afla key cautata
        void *value = ht_get(main[i].server.server_hash, key);
        // Cand o gasesc o returnez
        if (value != NULL) {
            return (char*)value;
        }
    }

    if (!ok)
    {
        *server_id = main[0].server.server_id;
        void *value = ht_get(main[0].server.server_hash, key);
        return (char*)value;
    }
    return NULL;
}

void free_load_balancer(load_balancer *main, int nr_server) {
    // Eliberez hashtable-urile
    for (int i = 0; i < nr_server; i++)
            ht_free(main[i].server.server_hash);
}

void get_key_value(char* key, char* value, char* request) {
	int key_start = 0, value_start = 0;
	int key_finish = 0, value_finish = 0;
	int key_index = 0, value_index = 0;

	for (unsigned int i = 0; i < strlen(request); ++i) {
		if (request[i] == '"' && value_start != 1) {
			if (key_start == 0) {
				key_start = 1;
			} else if (key_finish == 0) {
				key_finish = 1;
			} else if (value_start == 0) {
				value_start = 1;
			}
		} else {
			if (key_start == 1 && key_finish == 0) {
				key[key_index++] = request[i];
			} else if (value_start == 1 && value_finish == 0) {
				value[value_index++] = request[i];
			}
		}
	}

	value[value_index - 1] = 0;
}

void get_key(char* key, char* request) {
	int key_start = 0, key_index = 0;

	for (unsigned int i = 0; i < strlen(request); ++i) {
		if (request[i] == '"') {
			key_start = 1;
		} else if (key_start == 1) {
			key[key_index++] = request[i];
		}
	}
}

void apply_requests(FILE* input_file) {
	char request[REQUEST_LENGTH] = {0};
	char key[KEY_LENGTH] = {0};
	char value[VALUE_LENGTH] = {0};
	load_balancer* main_server = init_load_balancer();
    int nr_server = 0;
    int power_of_two = 8;
	while (fgets(request, REQUEST_LENGTH, input_file)) {
		request[strlen(request) - 1] = 0;
		if (!strncmp(request, "store", sizeof("store") - 1)) {
			get_key_value(key, value, request);

			int index_server = 0;
			loader_store(main_server, key, value, &index_server, nr_server);
			printf("Stored %s on server %d.\n", value, index_server);

			memset(key, 0, sizeof(key));
			memset(value, 0, sizeof(value));
		} else if (!strncmp(request, "retrieve", sizeof("retrieve") - 1)) {
			get_key(key, request);
			int index_server = 0;
			char *retrieved_value = loader_retrieve(main_server,
											key, &index_server, nr_server);
			if (retrieved_value) {
				printf("Retrieved %s from server %d.\n",
						retrieved_value, index_server);
			} else {
				printf("Key %s not present.\n", key);
			}

			memset(key, 0, sizeof(key));
		} else if (!strncmp(request, "add_server", sizeof("add_server") - 1)) {
			int server_id = atoi(request + sizeof("add_server"));
            // Cand am nevoie de mai multe pozitii
            // Realloc vectorul cu 2 la urmatoarea putere
            if (nr_server+3 >= power_of_two)
                {
                    power_of_two = power_of_two*2;
                    main_server = realloc(main_server,
                        power_of_two*sizeof(load_balancer));
                    DIE(!main_server, "REALLOC FAILED PANICK MODE ON\n");
                }
			loader_add_server(main_server, server_id, &nr_server);
            nr_server++;
		} else if (!strncmp(request, "remove_server",
					sizeof("remove_server") - 1)) {
			int server_id = atoi(request + sizeof("remove_server"));

			loader_remove_server(main_server, server_id, nr_server);
            nr_server = nr_server-3;
            // Daca am sters prea multe servere dealloc la o putere
            // mai mica a lui 2
            if (nr_server <= power_of_two/2)
                {
                    power_of_two = power_of_two/2;
                    main_server = realloc(main_server,
                        power_of_two*sizeof(load_balancer));
                    DIE(!main_server, "REALLOC FAILED PANICK PANICK\n");
                }
		} else {
			DIE(1, "unknown function call");
		}
	}
	free_load_balancer(main_server, nr_server);
    free(main_server);
}

int main(int argc, char* argv[]) {
	FILE *input;

	if (argc != 2) {
		printf("Usage:%s input_file \n", argv[0]);
		return -1;
	}

	input = fopen(argv[1], "rt");
	DIE(input == NULL, "missing input file");

	apply_requests(input);

	fclose(input);

	return 0;
}

