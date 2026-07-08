  #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char date[20];      //ημερομηνία
    long cumulative;    //τιμη cumulative για sorting 
} Record;

void countingSort(Record array[],int size){     //counting sort για ταξινόμηση των records με βάση το cumulative
    //ευρεση του max cumulative
    long max = array[0].cumulative;
    for(int i=1;i<size;i++){
        if(array[i].cumulative > max)
            max = array[i].cumulative;
    }
    
    printf("max cumulative = %ld\n", max);
    printf("max = %ld, range needed = %ld MB\n", max, (max * 8) / 1024 / 1024);
    // δεσμευση στον heap για τον count και το output array(μεγαλυτερο απο stack, δεν κινδυνευει απο overflow)
    long *count = calloc(max + 1, sizeof(long));
    Record *output = (Record *)malloc(size * sizeof(Record));
    
    //ελεγχος αποτυχίας δεσμευσης μνημης
    if(count == NULL){
        printf("Memory allocation failed\n");
        return;
    }
    if(output == NULL){
        printf("Memory allocation failed\n");
        free(count);
        return;
    }

    //αρχικοποίηση array count με 0
    for(int i=0;i<=max ;i++)
        count[i]=0;

    //καταμέτρηση εμφανίσεων του κάθε cumulative
    for(int i=0;i<size;i++)
        count[array[i].cumulative]++;

    //υπολογισμος prefix sum
    for(int i=1;i<=max;i++){
        count[i]+=count[i-1];
    }

    //ταξινόμηση των records στο output array με βάση το cumulative απο το τέλος για  σταθερότητα
    for(int i=size-1;i>=0;i--){
        output[count[array[i].cumulative]-1]=array[i];
        count[array[i].cumulative]--;
    }

    //αντιγραφή των ταξινομημένων records πίσω στο αρχικό array
    for(int i=0;i<size;i++)
        array[i]=output[i];

    //αποδεσμευση μνημης
    free(count);
    free(output);
}

void merge(Record array[], int left, int mid, int right){
    //για να δεσμευσω μνημη στον σωρό γα τον μισό πίνακα
    int n1=mid-left+1;      //πρώτο μισό
    int n2=right-mid;       //δεύτερο μισό

    if(mid < left || mid >= right){
        printf("Invalid mid index\n");
        return;
    }

    //δεσμευση στον heap για τα υποarrays L και R(μεγαλυτερο απο stack, δεν κινδυνευει απο overflow)
    Record *L = (Record *)malloc(n1 * sizeof(Record));
    Record *R = (Record *)malloc(n2 * sizeof(Record));
    //test για αποτυχία δεσμευσης μνημης
    if(L == NULL || R == NULL){
        printf("Memory allocation failed\n");
        free(L);
        free(R);
        return;
    }

    //αντιγραφή των δεδομένων στα υποarrays L και R
    for(int i=0;i<n1;i++)
        L[i]=array[left+i];
    for(int i=0;i<n2;i++)
        R[i]=array[mid+1+i];
    
    //αρχικοποίηση μεταβλητών για την συγχώνευση των υποarrays L και R πίσω στο αρχικό array
    // θα μπορουσα να βάλω 2 for loops για να αντιγράψω τα υποarrays L και R πίσω στο αρχικό array αλλά προτιμώ να χρησιμοποιήσω 3 while loops 
    //για να διατηρήσω την σταθερότητα του αλγορίθμου σε αυτο το κομμάτι σε Ο(n) χρόνο αντί για Ο(n1+n2) που θα ήταν με τα for loops
    int i = 0, j = 0, k = left;

    while(i < n1 && j < n2) {
        if(L[i].cumulative <= R[j].cumulative)
            array[k++] = L[i++];
        else
            array[k++] = R[j++];
    }

    //αν υπάρχουν υπολειπόμενα στοιχεία στο υποarray L ή R τα αντιγράφω πίσω στο αρχικό array
    while(i < n1)
        array[k++] = L[i++];

    while(j < n2)
        array[k++] = R[j++];

    //αποδεσμευση μνημης
    free(L);
    free(R);
}

void mergesort(Record array[], int left, int right){
    if(left < right){
        int mid = left + (right - left) / 2;        //χωρισμος του array σε δύο υποarrays σε κάθε κλήση
        mergesort(array, left, mid);                //αναδρομκη κλήση για το πρώτο υποarray
        mergesort(array, mid + 1, right);
        merge(array, left, mid, right);         //συνένωση των υποarrays με βάση το cumulative
    }
}

int main(){
    //ανοιγμα csv αρχείου για ανάγνωση("r"=read)
    FILE *file = fopen("data.csv","r");
    //ελεγχος αν το αρχείο άνοιξε επιτυχώς
    if(file==NULL){
        printf("Error opening file\n");
        return 1;
    }
    //buffer για καθε γραμμη του csv αρχειου
    char line[200];
    fgets(line,sizeof(line),file); //προσπερναω ονοματα στηλων
    //δεσμευση στον heap για τα records(μεγαλυτερο απο stack, δεν κινδυνευει απο overflow)
    Record *records = (Record *)calloc(200000, sizeof(Record));
    if (records == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    int count=0;

    while(fgets(line,sizeof(line),file)!=NULL && count<200000){  //λουπα χωρισμου καθε γραμμης σε tokens για πιο εύκολη επεξεργασία με την strtok
        line[strcspn(line, "\n")] = 0;  //αν υπάρχει newline το αφαιρω 
        char* token=strtok(line,",");
        int column=0;
        
        //προσωρινο Record για αποθήκευση της ημερομηνίας και του cumulative της τρέχουσας γραμμής
        Record cumulativeRecord={0};
        while(token!=NULL){
            if(column==2){    //ημερομηνια
                strcpy(cumulativeRecord.date, token);
            }
            if(column==9){   //cumulative
                cumulativeRecord.cumulative=atol(token);
            }
            token=strtok(NULL,",");
            column++;   //αν δεν ειναι 2 ή 9 αυξάνω με κάθε επανάληψη
        }
        //αποθήκευση του Record στο array των records
        records[count++]=cumulativeRecord;
    }
    fclose(file);  //κλεισε αρχείο 

    Record *records1 = malloc(count * sizeof(Record));  //δυο διαφορετικα copies των δεδομένων για να μετρήσω ταυτόχρονα τον χρόνο των δύο αλγορίθμων ξεχωριστά
    Record *records2 = malloc(count * sizeof(Record));

    memcpy(records1, records, count * sizeof(Record));
    memcpy(records2, records, count * sizeof(Record));

    clock_t start1 = clock();   //μέτρηση χρόνου για counting sort
    countingSort(records1, count);      //ταξινόμηση με counting sort με βάση το cumulative
    clock_t end1 = clock();

    clock_t start2 = clock();       //μέτρηση χρόνου για merge sort
    mergesort(records2, 0, count - 1);    //ταξινόμηση με merge sort με βάση το cumulative
    clock_t end2 = clock();

    double time1 = (double)(end1 - start1) / CLOCKS_PER_SEC;        //υπολογισμός χρόνου εκτέλεσης για counting sort
    double time2 = (double)(end2 - start2) / CLOCKS_PER_SEC;        //υπολογισμός χρόνου εκτέλεσης για merge sort

    printf("Counting Sort Time: %f sec\n", time1);        //εκτύπωση των χρόνων εκτέλεσης των δύο αλγορίθμων
    printf("Merge Sort Time: %f sec\n", time2);
    
    free(records);   //αποδεσμευση μνημης
    free(records1);
    free(records2);

    printf("Sorting completed successfully!\n");
    return 0;
}


