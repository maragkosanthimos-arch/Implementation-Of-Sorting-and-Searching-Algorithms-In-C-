#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//typedef struct για δημιουργία κόμβων με αυτα τα δεδομένα
typedef struct{
    long value;
    char date[20];
} Record;

//swap για αφαιρετικότητα
void swap(Record *a, Record *b) {
    Record temp = *a;
    *a = *b;
    *b = temp;
}

//συνάρτηση heapify. συντασσει τον εκαστοτε πίνακα σε max heap
void heapify(Record arr[], int n, int i) {
    int largest = i; //γονεας
    int left = 2*i + 1; //παιδιά
    int right = 2*i + 2;

    if (left < n && arr[left].value > arr[largest].value)
        largest = left;

    if (right < n && arr[right].value > arr[largest].value)
        largest = right;

    //αν ο γονιος ηταν μεγαλυτερος απο τα παιδιά και δεν άλλαξε τιμή. συνεχίζουμε,
    //αν του αναθέσαμε την θέση παιδιού κάνουμε swap και αναδρομη για τη θέση του παιδιού που αντικαταστήσαμ
    if (largest != i) {
        swap(&arr[i], &arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(Record arr[],int n){
    //max-heap
    for(int i=n/2-1;i>=0;i--){
        heapify(arr,n,i);
    }
    //το πργματικο heap-sort. μειώνουμε κατα μια θέση τον πίνακα για τα στοιχείανπου ταξινομήσαμε
    for(int i=n-1;i>=0;i--){
        swap(&arr[0], &arr[i]);
        heapify(arr,i,0);
    }
}

void quickSort(Record arr[], int left, int right) {
    int leftArrow = left; 
    int rightArrow = right; 
    
    // επιλογή της τιμής του μεσαίου στοιχείου ως pivot
    long pivot = arr[(left + right) / 2].value;
    
    do {
        // μετακινηση του δεξιού δείκτη αριστερά όσο βρίσκει τιμές μεγαλυτερες από το pivot
        while (arr[rightArrow].value > pivot)
            --rightArrow; 

        // μετακίνηση του αριστερού δείκτη δεξιά οσο βρίσκει τιμές μικροτερες από το pivot
        while (arr[leftArrow].value < pivot) 
            ++leftArrow;
            
        //αν οι δείκτες δεν εχουν προσπελάσει ο ένας τον άλλο, swap
        if (leftArrow <= rightArrow) { 
            swap(&arr[leftArrow], &arr[rightArrow]); 
            ++leftArrow; 
            --rightArrow; 
        }
    } while (rightArrow >= leftArrow); 
    
    // αναδρομη για αριστερο διάστημα
    if (left < rightArrow) 
        quickSort(arr, left, rightArrow); 
        
    // αναδρομη για δεξί διάστημα
    if (leftArrow < right) 
        quickSort(arr, leftArrow, right); 
}

int main(){
    FILE *file=fopen("data.csv","r");
    if(file==NULL){
        printf("Error opening file\n");
        return 1;
    }

    char line[200];
    fgets(line, sizeof(line), file); // Skip header line
    Record *records = (Record *)calloc(200000, sizeof(Record));

    if(records == NULL){
        printf("Memory allocation failed\n");
        return 1;
    }

    int count=0;
    while(fgets(line, sizeof(line), file) != NULL && count < 200000){
        line[strcspn(line, "\n")] = 0; // Remove newline character
        char *token = strtok(line, ",");
        int column = 0;
        Record valueRecord={0};
        
        while(token!=NULL){
            if(column==2){
                strcpy(valueRecord.date, token);
            }
            if(column==8){
                valueRecord.value = atol(token);
            }
            token = strtok(NULL, ",");
            column++;
        }
        records[count++] = valueRecord;
    }
    fclose(file);
    Record *heapRecords = malloc(count * sizeof(Record));
    Record *quickRecords = malloc(count * sizeof(Record));

    memcpy(heapRecords, records, count * sizeof(Record));
    memcpy(quickRecords, records, count * sizeof(Record));

    clock_t start1 = clock();
    heapSort(heapRecords, count);
    clock_t end1 = clock();

    clock_t start2 = clock();
    quickSort(quickRecords, 0, count - 1);
    clock_t end2 = clock();

    double heapTime = (double)(end1 - start1) / CLOCKS_PER_SEC;
    double quickTime = (double)(end2 - start2) / CLOCKS_PER_SEC;

    printf("Heap Sort Time: %f sec\n", heapTime);
    printf("Quick Sort Time: %f sec\n", quickTime);

    free(heapRecords);
    free(quickRecords);
    free(records);
    return 0;
}