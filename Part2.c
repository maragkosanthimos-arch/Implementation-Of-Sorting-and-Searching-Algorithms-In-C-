#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define M 11

typedef struct Record {
    int Date;
    long Cumulative;
    char date[15];
} Record;

typedef struct Node {
    Record data;
    struct Node *right_child;
    struct Node *left_child;
    int balance_factor;
} Node;

// παράμετρος next για chain hashing
typedef struct HashNode {
    Record data;
    struct HashNode *next;
} HashNode;

// Καθολικός Πίνακας Κατακερματισμού
HashNode *buckets[M];

//========= ΣΥΝΑΡΤΉΣΕΙΣ AVL ============

//LL
Node *right_rotation(Node *root) {
    Node *new_root1 = root->left_child;
    Node *new_root2 = new_root1->right_child;
    new_root1->right_child = root;
    root->left_child = new_root2;
    return new_root1;
}

//RR
Node *left_rotation(Node *root) {
    Node *new_root1 = root->right_child;
    Node *new_root2 = new_root1->left_child;
    new_root1->left_child = root;
    root->right_child = new_root2;
    return new_root1;
}

//συνάρτηση balancing avl tree
//αν bf > 1 και left_child bf >=0 -> LL

Node* balance_avl_tree(Node* node) {
    if (node == NULL) return NULL;
    // 1. ΠΕΡΙΠΤΩΣΗ LL (Left-Left)
    // Ο κόμβος είναι αριστερόβαρος (BF > 1) ΚΑΙ το αριστερό παιδί είναι επίσης αριστερόβαρο (BF >= 0).
    if (node->balance_factor > 1 && node->left_child && node->left_child->balance_factor >= 0) {
        return right_rotation(node); // Λύση: Μία απλή Δεξιά Περιστροφή στον κόμβο
    }
    // 2. ΠΕΡΙΠΤΩΣΗ LR (Left-Right)
    // Ο κόμβος είναι αριστερόβαρος (BF > 1) ΑΛΛΑ το αριστερό παιδί είναι δεξιόβαρο (BF < 0).
    else if (node->balance_factor > 1 && node->left_child && node->left_child->balance_factor < 0) {
        node->left_child = left_rotation(node->left_child); // Βήμα 1: Αριστερή περιστροφή στο παιδί για να "ισιώσει" το ζικ-ζακ σε LL
        return right_rotation(node);                        // Βήμα 2: Δεξιά περιστροφή στον αρχικό κόμβο για τελική ισορροπία
    } 
    // 3. ΠΕΡΙΠΤΩΣΗ RR (Right-Right)
    // Ο κόμβος είναι δεξιόβαρος (BF < -1) ΚΑΙ το δεξί παιδί είναι επίσης δεξιόβαρο (BF <= 0).
    else if (node->balance_factor < -1 && node->right_child && node->right_child->balance_factor <= 0) {
        return left_rotation(node); // Λύση: Μία απλή Αριστερή Περιστροφή στον κόμβο
    }
    // 4. ΠΕΡΙΠΤΩΣΗ RL (Right-Left)
    // Ο κόμβος είναι δεξιόβαρος (BF < -1) ΑΛΛΑ το δεξί παιδί είναι αριστερόβαρο (BF > 0).
    else if (node->balance_factor <-1 && node->right_child && node->right_child->balance_factor > 0) {
        node->right_child = right_rotation(node->right_child);
        return left_rotation(node);
    }
    return node;
}

// αναδρομική συνάρτηση υπολογισμού του ύψους ενός κόμβου
int height(Node *root) {
    if (root == NULL)
        return 0;
    int left_height = height(root->left_child);
    int right_height = height(root->right_child);
    return 1 + (left_height > right_height ? left_height : right_height);
}

// υπολογισμός και αποθήκευση του balance factor (ύψος αριστερού υποδεντρου - δεξια υποδέντρου)
Node *calculate_balance_factor(Node *node) {
    if (node == NULL)
        return NULL;
    int left = height(node->left_child);
    int right = height(node->right_child);
    node->balance_factor = left - right;
    return node;
}

// δημιουργία και αρχικοποίηση ενός νέου κόμβου AVL
Node *newNode(Record data) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->data = data;
    new_node->right_child = NULL;
    new_node->left_child = NULL;
    new_node->balance_factor = 0; //νεοι κόμβοι πάντα bf = 0
    return new_node;
}

//εισαγωγή όποως σε κανονικό  bst για (Α)
// ενημέρωση των bf των κόμβων
//εξισορρόπηση δεντρου
Node *insert_by_date(Node *root, Record data) {
    if (root == NULL) {
        return newNode(data);
    }
    if (data.Date < root->data.Date)
        root->left_child = insert_by_date(root->left_child, data);
    else if (data.Date > root->data.Date)
        root->right_child = insert_by_date(root->right_child, data);
    
    calculate_balance_factor(root->left_child);
    calculate_balance_factor(root->right_child);
    calculate_balance_factor(root);
    root = balance_avl_tree(root);
    return root;
}

//εισαγωγή νέας εγγραφης βάση cumulative για το (Β)
Node *insert_by_cumulative(Node *root, Record data) {
    if (root == NULL)
        return newNode(data);
    if (data.Cumulative <= root->data.Cumulative)
        root->left_child = insert_by_cumulative(root->left_child, data);
    else if (data.Cumulative > root->data.Cumulative)
        root->right_child = insert_by_cumulative(root->right_child, data);
    
    calculate_balance_factor(root->left_child);
    calculate_balance_factor(root->right_child);
    calculate_balance_factor(root);
    root = balance_avl_tree(root);
    return root;
}

Node* findMin(Node* root) {
    if (root == NULL) return NULL;
    while (root->left_child != NULL) {
        root = root->left_child;
    }
    return root;
}

Node* findMax(Node* root) {
    if (root == NULL) return NULL;
    while (root->right_child != NULL) {
        root = root->right_child;
    }
    return root;
}

//μετατροπή ημερομηνίας σε ακέραιο για όρισμα
long convertDate(char date[]) {
    int day, month, year;
    if (sscanf(date, "%d/%d/%d", &day, &month, &year) == 3) {
        return year * 10000 + month * 100 + day;
    }
    return 0;
}

//ενδοδιατεταγμένη διάσχιση 
//εκτύπωση σε αύξουσα σειρά
void inorder_traversal(Node *root) {
    if (root != NULL) {
        inorder_traversal(root->left_child);
        printf("Date: %s, Cumulative: %ld\n", root->data.date, root->data.Cumulative);
        inorder_traversal(root->right_child);
    }
}

//αναδρομικό δυαδικό ψάξιμο
Node* search(Node* root, int date) {
    if (root == NULL || root->data.Date == date)
        return root;
    if (date < root->data.Date)
        return search(root->left_child, date);
    return search(root->right_child, date);
}

//εντοπισμός κόμβου και αλλαγή τιμής
Node *edit(Node *root, int date, long new_Cumulative) {
    Node* new_node_edit = search(root, date);
    if (new_node_edit != NULL) {
        new_node_edit->data.Cumulative = new_Cumulative;
    }
    return root;
}

//4 περιπτώσεις για διαγραφή
Node *delete_node(Node *root, int date) {
    if (root == NULL) return NULL;
    if (date < root->data.Date) {
        root->left_child = delete_node(root->left_child, date);
    } else if (date > root->data.Date) {
        root->right_child = delete_node(root->right_child, date);
    } else {
        //βρήκαμε κόμβο
        //περίπτωση 1: ο κόμβος είναι φύλλο
        if (root->left_child == NULL && root->right_child == NULL) {
            free(root);
            return NULL;
        }
        //περίπτωση 2: μόνο δεξί παιδί
        //δεξόι παιδί παίρνει τη θέση του
        if (root->right_child != NULL && root->left_child == NULL) {
            Node *temp = root->right_child;
            free(root);
            return temp;
        }
        //περίπτωση 3: αντίστοιχα με περίπτωση 2 για αριστερό παιδί μόνο
        if (root->right_child == NULL && root->left_child != NULL) {
            Node *temp = root->left_child;
            free(root);
            return temp;
        }
        //περίπτωση 4:κόμβος με 2 παιδιά
        //βρίσκουμε in-order successor, αντιγράφουμε δεδομένα στον τρέχοντα κόμβο, διαγράφουμε in-order successor
        if (root->right_child != NULL && root->left_child != NULL) {
            Node *temp = root->right_child;
            while (temp->left_child != NULL)
                temp = temp->left_child;
            root->data = temp->data;
            root->right_child = delete_node(root->right_child, temp->data.Date);
        }
    }
    //επαναυπολογισμός bf και εξισορρόπησης
    calculate_balance_factor(root->left_child);
    calculate_balance_factor(root->right_child);
    calculate_balance_factor(root);
    root = balance_avl_tree(root);
    return root;
}

//Συναρτήησεις εκτυπώσεων για (Β)
//εκτυπώνουμε όλες τις εγγραφές για τυχόν διπλότυπα
void print_min_cumulative(Node *root, long minimum_cumulative) {
    if (root == NULL) return;
    print_min_cumulative(root->left_child, minimum_cumulative);
    if (root->data.Cumulative == minimum_cumulative)
        printf("Date: %s, Cumulative: %ld\n", root->data.date, root->data.Cumulative);
    print_min_cumulative(root->right_child, minimum_cumulative);
}

void print_max_cumulative(Node *root, long maximum_cumulative) {
    if (root == NULL) return;
    print_max_cumulative(root->left_child, maximum_cumulative);
    if (root->data.Cumulative == maximum_cumulative)
        printf("Date: %s, Cumulative: %ld\n", root->data.date, root->data.Cumulative);
    print_max_cumulative(root->right_child, maximum_cumulative);
}

//========= HASH TABLE ==========

//hash function εκφώνησης
int hash_function(char *date, int m) {
    int sum = 0;
    for (int i = 0; date[i] != '\0'; i++) {
        sum += date[i];
    }
    return sum % m;
}

//δημιουργία κόμβου με αρχικοποίηση δεδομένων
HashNode *createHashNode(Record data) {
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

//εισαγωγη.
//παίρνουμε index για τα buckets από hash function. αν κενη θέση βαζουμε κόμβο.
//αλλιώς το next του προυπάρχοντος δείχνει τον καινουριο για δομή linked list
HashNode *insertHash(Record data) {
    int index = hash_function(data.date, M);
    HashNode *new_node = createHashNode(data);
    if (buckets[index] == NULL) {
        buckets[index] = new_node;
    } else {
        HashNode *current = buckets[index];
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
    return buckets[index];
}


//βρίσκουμε την θέση του στοιχειου στον πίνακα buckets και  ψάχνουμε όλο το linked list του bucket 
HashNode *searchHash(char *date) {
    int index = hash_function(date, M);
    HashNode *current = buckets[index];
    while (current != NULL) {
        if (strcmp(current->data.date, date) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}


//διαγραφή του κόμβου και διπλοτύπων
HashNode* deleteHash(HashNode* current, char* date, int* success) {
    if (current == NULL) {
        return NULL; 
    }
    
    //διατήρηση ιδιότητας linked list
    //prev->node->next -----> prev->next και free(node)
    if (strcmp(current->data.date, date) == 0) {
        HashNode* temp = current->next;
        free(current);                  
        *success = 1;                   
        return deleteHash(temp, date, success);
    }

    //αναδρομική κλήση μεχρι να βρουμε σωστή ημερομηνία και να πιάσει η συνθήκη if
    current->next = deleteHash(current->next, date, success);
    return current;
}

void editHash(char *date, long new_Cumulative) {
    HashNode *node = searchHash(date);
    if (node != NULL)
        node->data.Cumulative = new_Cumulative;
    else
        printf("Date not found.\n");
}

/*
==================================================
        ΕΝΟΠΟΙΗΜΈΝΑ MENU (Α),(Β),(Γ)
==================================================
*/


void menu_bst_by_date(Node *root) {
    int option = 0;
    do {
        printf("===============BST(BY DATE) MENU===============\n");
        printf("1. In-order Traversal of BST\n");
        printf("2. Search Cumulative Value for a Given Date\n");
        printf("3. Edit the Cumulative Value for a Given Date\n");
        printf("4. Delete a Record for a Given Date\n");
        printf("5. Exit\n");

        printf("Enter the number of your desired choice:");
        scanf("%d", &option);
        switch (option) {
            case 1: {
                inorder_traversal(root);
                break;
            }
            case 2: {
                printf("Enter a date (dd/mm/yyyy) to search for its cumulative value:");
                char search_date[15];
                scanf("%s", search_date);
                int search_date_num = convertDate(search_date);
                Node* found_node = search(root, search_date_num);
                if (found_node != NULL) {
                    printf("Cumulative value for %s: %ld\n", search_date, found_node->data.Cumulative);
                } else {
                    printf("Date not found in the records.\n");
                }
                break;
            }
            case 3: {
                printf("Enter a date (dd/mm/yyyy) to edit its cumulative value:");
                char edit_date[15];
                scanf("%s", edit_date);
                int edit_date_num = convertDate(edit_date);
                printf("Enter the new cumulative value:");
                long new_cumulative;
                scanf("%ld", &new_cumulative);
                root = edit(root, edit_date_num, new_cumulative);
                break;
            }
            case 4: {
                printf("Enter a date (dd/mm/yyyy) to delete its record:");
                char delete_date[15];
                scanf("%s", delete_date);
                int delete_date_num = convertDate(delete_date);
                root = delete_node(root, delete_date_num);
                break;
            }
            case 5: {
                printf("Exiting the program. Goodbye!\n");
                break;
            }
            default:
                printf("Invalid option. Please try again.\n");
                break;
        }
    } while (option != 5);
}

void menu_bst_by_cumulative(Node *root) {
    int option = 0;
    do {
        printf("===============BST(BY CUMULATIVE) MENU===============\n");
        printf("\n===== BST by Cumulative Menu =====\n");
        printf("1. Find Day(s) with MINIMUM Cumulative\n");
        printf("2. Find Day(s) with MAXIMUM Cumulative\n");
        printf("3. Exit\n");
        printf("Enter the number of your desired choice:");
        scanf("%d", &option);
        switch (option) {
            case 1: {
                Node* min_node = findMin(root);
                if (min_node != NULL) {
                    printf("Minimum Cumulative Value: %ld\n", min_node->data.Cumulative);
                    printf("Records with Minimum Cumulative Value:\n");
                    print_min_cumulative(root, min_node->data.Cumulative);
                } else {
                    printf("The tree is empty.\n");
                }
                break;
            }
            case 2: {
                Node* max_node = findMax(root);
                if (max_node != NULL) {
                    printf("Maximum Cumulative Value: %ld\n", max_node->data.Cumulative);
                    printf("Records with Maximum Cumulative Value:\n");
                    print_max_cumulative(root, max_node->data.Cumulative);
                } else {
                    printf("The tree is empty.\n");
                }
                break;
            }
            case 3: {
                printf("Exiting the program. Goodbye!\n");
                break;
            }
            default:
                printf("Invalid option. Please try again.\n");
                break;
        }
    } while (option != 3);
}

void menu_hash_table() {
    int option = 0;
    do {
        printf("===============HASH TABLE MENU===============\n");
        printf("1. Search Cumulative by Date\n");
        printf("2. Edit Cumulative by Date\n");
        printf("3. Delete Record by Date\n");
        printf("4. Exit\n");
        printf("Enter the number of your desired choice:");
        scanf("%d", &option);
        switch (option) {
            case 1: {
                char date[20];
                printf("Enter a date (dd/mm/yyyy) to search for its cumulative value:");
                scanf("%s", date);
                HashNode *found = searchHash(date);
                if (found == NULL) {
                    printf("Date not found in the records.\n");
                } else {
                    printf("Cumulative value for %s: %ld\n", date, found->data.Cumulative);
                }
                break;
            }
            case 2: {
                char date[15];
                long new_cum;
                printf("Enter date (dd/mm/yyyy): ");
                scanf("%s", date);
                printf("Enter new cumulative value: ");
                scanf("%ld", &new_cum);
                editHash(date, new_cum);
                break;
            }
            case 3: {
                char date[15];
                printf("Enter date (dd/mm/yyyy): ");
                scanf("%s", date);
                
                // Υπολογισμός του index απευθείας εδώ για την αναδρομή
                int index = hash_function(date, M);
                int success = 0;
                
                buckets[index] = deleteHash(buckets[index], date, &success);
                
                if (success) {
                    printf("All records for date %s deleted successfully.\n", date);
                } else {
                    printf("Date not found.\n");
                }
                break;
            }
            case 4:
                printf("Exiting Hash Table menu.\n");
                break;
            default:
                printf("Invalid option.\n");
                break;
        }
    } while (option != 4);
}

//main που καλεί τα menu με σειρά και φορτώνει ανάλογα με επιλογή χρήστη

int main() {
    Node *root = NULL;
    for (int i = 0; i < M; i++) buckets[i] = NULL;

    int main_choice = 0;
    int bst_choice = 0;

    // ΒΗΜΑ 1: Ρωτάμε πρώτα την επιλογή δομής
    printf("\n===== Main Menu =====\n");
    printf("1. Load into BST\n");
    printf("2. Load into Hash Table\n");
    printf("Choice: ");
    scanf("%d", &main_choice);

    if (main_choice == 1) {
        printf("\n===== BST Menu =====\n");
        printf("1. BST ordered by Date\n");
        printf("2. BST ordered by Cumulative\n");
        printf("Choice: ");
        scanf("%d", &bst_choice);
        if (bst_choice != 1 && bst_choice != 2) {
            printf("Invalid option. Exiting.\n");
            return 1;
        }
    } else if (main_choice != 2) {
        printf("Invalid option. Exiting.\n");
        return 1;
    }

    // ΒΗΜΑ 2: Ανοίγουμε το αρχείο και γεμίζουμε την επιλεγμένη δομή απευθείας!
    FILE *file = fopen("data.csv", "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    char line[200];
    fgets(line, sizeof(line), file); // Προσπέραση επικεφαλίδων
    
    int count = 0;
    while (fgets(line, sizeof(line), file) != NULL && count < 200000) {
        line[strcspn(line, "\n")] = 0; 
        char *token = strtok(line, ",");
        int column = 0;

        Record dateNumRecord = {0};
        while (token != NULL) {
            if (column == 2) { 
                strcpy(dateNumRecord.date, token);
                dateNumRecord.Date = convertDate(dateNumRecord.date);
            }
            if (column == 9) { 
                dateNumRecord.Cumulative = atol(token);
            }
            token = strtok(NULL, ",");
            column++;
        }

        // Εισαγωγή "on-the-fly" χωρίς ενδιάμεσο πίνακα
        if (main_choice == 1) {
            if (bst_choice == 1) {
                root = insert_by_date(root, dateNumRecord);
            } else {
                root = insert_by_cumulative(root, dateNumRecord);
            }
        } else {
            insertHash(dateNumRecord);
        }
        count++;
    }
    fclose(file);
    printf("Successfully loaded %d records.\n", count);

    // ΒΗΜΑ 3: Άνοιγμα του αντίστοιχου μενού λειτουργιών
    if (main_choice == 1) {
        if (bst_choice == 1) menu_bst_by_date(root);
        else menu_bst_by_cumulative(root);
    } else {
        menu_hash_table();
    }
    return 0;
}