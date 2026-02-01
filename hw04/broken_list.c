/* Linked List implementation
 * version 1.0
 * "Guaranteed" to work (because LCDR Norine wrote it)
 */

#include <stdlib.h> //for malloc/calloc/free
#include <stdio.h> //for printf
#include <string.h> //for strlen/strcpy
#include <stdbool.h> //for bool type

typedef struct Node Node; //forward declared here

//definition of the Node within the list
typedef struct Node {
	int id; //user id. should be unique.
	char* name; //pointer to dynamic memory for user's name.
	Node* next; //pointer to the next node. NULL indicates that we are at the end of the list.
} Node;

//definition of the Linked List
typedef struct {
	Node* head; //effectively acts as a pointer to the list
} LinkedList;

//create a new node, returns a pointer to it.
Node* create_node(int id, char* data) {
    Node* new_node = malloc(sizeof(Node));
    new_node->id = id;
    new_node->name = malloc(strlen(data)+1);
    strcpy(new_node->name, data);
    new_node->name[strlen(data)] = '\0';
    
    new_node->next = NULL;

    return new_node;
}


//add node to front of list
void add_node(LinkedList* list, Node* node) {
	// Don't add a node that is already on the list!
	if (node->next != NULL) {
		return;
	}
	node->next = list->head; //make the current head the node after our new addition.
	list->head = node; //our new node is the new head.
}

//traverse and remove node from list, if node is not in the list, silently fails.
void remove_node_by_id(LinkedList* list, int id) {
    Node* curr_node = list->head;
    Node* prev_node = NULL;

    if (curr_node == NULL) {
        return;
    }

    if (curr_node->id == id) {
        list->head = curr_node->next;
        free(curr_node->name);
        free(curr_node);
        return;
    }

    while (curr_node != NULL && curr_node->id != id) {
        prev_node = curr_node;
        curr_node = curr_node->next;
    }

    if (curr_node == NULL) {
        return;
    }

    prev_node->next = curr_node->next;
    free(curr_node->name);
    free(curr_node);
}


//returns true if the id is in the list.
bool contains_node_by_id(LinkedList* list, int id) {
	Node* curr_node = list->head;
	while (curr_node != NULL) {
		if (curr_node->id == id) {
			return true;
		}
		curr_node = curr_node->next;
	}
	return false;
}

//returns a pointer to the node if it is in the list.
Node* get_node_by_id(LinkedList* list, int id) {
	Node* curr_node = list->head;
	while (curr_node != NULL) {
		if (curr_node->id == id) {
			return curr_node;
		}
		curr_node = curr_node->next;
	}
	return NULL;
}

int main() {
    //Setup
    LinkedList* list = calloc(1, sizeof(LinkedList));

    //Add Nodes
    printf("Step 1: Adding nodes to the list...\n");
    add_node(list, create_node(1, "Alice"));
    add_node(list, create_node(2, "Bob"));
    add_node(list, create_node(3, "Charlie"));

    //List is now: Charlie (Head) -> Bob -> Alice

	//Search for a member of the list.
    printf("\nStep 2: Searching for ID 1 (Alice)...\n");
    if (contains_node_by_id(list, 1)) {
        printf("  [SUCCESS] Found Alice!\n");
    } else {
        printf("  [FAIL] Alice was not found!\n");
    }

	//Remove a member from the list.
    printf("\nStep 3: Removing ID 3...\n");
    remove_node_by_id(list, 3);
    printf("  [SUCCESS] Charlie removed successfully.\n");

    //Cleanup
	// Free all nodes in the list
	Node* curr = list->head;
	while (curr != NULL) {
		Node* next = curr->next;  // save next node
		free(curr->name);         // free the name string
		free(curr);               // free the node itself
		curr = next;              // move to next node
	}
	
    free(list);
    return 0;
}
