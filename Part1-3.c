#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>   

typedef struct record{
    long cumulative;
    long dateNum;
    char date[20];
}Record;

long convertDate(char date[]){
    int day, month, year;
    sscanf(date, "%d/%d/%d", &day, &month, &year);
    return day * 10000 + month * 100 + year;
}

void countingSort(Record array[],int size){     //counting sort για ταξινόμηση των records με βάση το dateNum
    //ευρεση του max και min dateNum
    long max = array[0].dateNum;
    long min = array[0].dateNum;
    for(int i=1;i<size;i++){
        if(array[i].dateNum > max)
            max = array[i].dateNum;
        if(array[i].dateNum < min)
            min = array[i].dateNum;
    }
    
    // δεσμευση στον heap για τον count και το output array(μεγαλυτερο απο stack, δεν κινδυνευει απο overflow)
    long *count = calloc(max - min + 1, sizeof(long));
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
    for(int i=0;i<=max-min;i++)
        count[i]=0;

    //καταμέτρηση εμφανίσεων του κάθε dateNum
    for(int i=0;i<size;i++)
        count[array[i].dateNum - min]++;

    //υπολογισμος prefix sum
    for(int i=1;i<=max-min;i++){
        count[i]+=count[i-1];
    }

    //ταξινόμηση των records στο output array με βάση το dateNum απο το τέλος για  σταθερότητα
    for(int i=size-1;i>=0;i--){
        output[count[array[i].dateNum - min]-1]=array[i];
        count[array[i].dateNum - min]--;
    }

    //αντιγραφή των ταξινομημένων records πίσω στο αρχικό array
    for(int i=0;i<size;i++)
        array[i]=output[i];

    //αποδεσμευση μνημης
    free(count);
    free(output);
}

long binarySearch(Record array[], int size, long target){   //δυαδική αναζήτηση για εύρεση του index του record με το συγκεκριμένο dateNum
    int left=0, right=size-1;
    while(left<=right){
        int mid=left+(right-left)/2;
        if(array[mid].dateNum==target)
            return mid;
        else if(array[mid].dateNum<target)
            left=mid+1;
        else
            right=mid-1;
    }
    return -1;  //επιστρέφει -1 αν δεν βρεθεί το target
}


long interpolationSearch(Record array[], int size, long target){
    int low = 0;
    int high = size - 1;

    while(low <= high &&
          target >= array[low].dateNum &&
          target <= array[high].dateNum){

        // αν low και high έχουν ίδια τιμή
        if(array[high].dateNum == array[low].dateNum){
            if(array[low].dateNum == target)
                return low;
            return -1;
        }

        // υπολογισμός θέσης παρεμβολής
        int pos = low + (
            (double)(high - low) /
            (array[high].dateNum - array[low].dateNum)
        ) * (target - array[low].dateNum);

        if(array[pos].dateNum == target)
            return pos;

        if(array[pos].dateNum < target)
            low = pos + 1;
        else
            high = pos - 1;
    }

    return -1;
}

void printAllCumulative(Record array[],int size, long target, long index){
    if(index==-1){
        printf("Record with dateNum %ld not found.\n", target);
        return;
    } 
    for(int i=0;i<size;i++){
        if(array[i].dateNum==target){
            printf("Cumulative for date %s: %ld\n", array[i].date, array[i].cumulative);
        }
    }
}

int main(){
    FILE *file=fopen("data.csv", "r");
    if(file == NULL){
        printf("Error opening file.\n");
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

        Record dateNumRecord={0};
        while(token!=NULL){
            if(column==2){    //ημερομηνια
                strcpy(dateNumRecord.date, token);
                dateNumRecord.dateNum = convertDate(dateNumRecord.date);

            }
            if(column==9){   //cumulative
                dateNumRecord.cumulative=atol(token);
            }
            token=strtok(NULL,",");
            column++;   //αν δεν ειναι 2 ή 9 αυξάνω με κάθε επανάληψη
        }
        //αποθήκευση του Record στο array των records
        records[count++]=dateNumRecord;
    }

    fclose(file);  //κλεισε αρχείο 

    countingSort(records, count);
    char inputDate[20];
    printf("Δώσε ημερομηνία (DD/MM/YYYY): ");
    scanf("%19s", inputDate);
    long target = convertDate(inputDate);
    
    clock_t start, end;
    double cpu_time;

    // Binary Search
    start = clock();
    long index1 = binarySearch(records, count, target);
    end = clock();

    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("=======BINARY SEARCH=======\n");
    printf("Binary Search index: %ld\n", index1);
    printf("Binary Search time: %f seconds\n", cpu_time);
    printAllCumulative(records, count, target, index1);

    // Interpolation Search
    start = clock();
    long index2 = interpolationSearch(records, count, target);
    end = clock();

    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("\n\n=======INTERPOLATION SEARCH=======\n");
    printf("Interpolation Search index: %ld\n", index2);
    printf("Interpolation Search time: %f seconds\n", cpu_time);
    printAllCumulative(records, count, target, index2);
    return 0;
}
