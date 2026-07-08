#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>  
#include <math.h> 

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

long binaryInterpolationSearch(Record array[],int size,long target){
    int low=0, high=size-1;
    while(low<=high && target>=array[low].dateNum && target<=array[high].dateNum){
        if(low==high){
            if(array[low].dateNum==target)
                return low;
            return -1;
        }
        if(array[low].dateNum==array[high].dateNum){
            if(array[low].dateNum==target)
                return low;
            return -1;      //division by zero 
        }
        long pos=low + ((high-low)*(target-array[low].dateNum))/(array[high].dateNum-array[low].dateNum);
        if(pos<low || pos>high)
            return -1;
        if(array[pos].dateNum==target)
            return pos;
        
        double size2=high-low+1;
        int step=(int)sqrt(size2);
        if(step<1)
            step=1;
        int i=1;

        if(target>array[pos].dateNum){
            while(pos+i*step<=high && array[pos+i*step].dateNum<target)
                i++;
            low=pos+1+(i-1)*step;
            high=pos+i*step;

            if(high>size-1)
                high=size-1;
        }

        if(target<array[pos].dateNum){
            while(pos-i*step>=low && array[pos-i*step].dateNum>target)
                i++;
            high=pos-1-(i-1)*step;
            low=pos-i*step;

            if(low<0)
                low=0;
        }
        
    }
    return -1;
}

long ModifiedbinaryInterpolationSearch(Record array[],int size,long target){
    int low=0, high=size-1;
    while(low<=high && target>=array[low].dateNum && target<=array[high].dateNum){
        if(low==high){
            if(array[low].dateNum==target)
                return low;
            return -1;
        }
        if(array[low].dateNum==array[high].dateNum){
            if(array[low].dateNum==target)
                return low;
            return -1;      //division by zero 
        }
        long pos=low + ((high-low)*(target-array[low].dateNum))/(array[high].dateNum-array[low].dateNum);
        if(pos<low || pos>high)
            return -1;
        if(array[pos].dateNum==target)
            return pos;
        
        double size2=high-low+1;
        int step=(int)sqrt(size2);
        if(step<1)
            step=1;
        int i=0;

        int exponent=1;

        if(target>array[pos].dateNum){
            while(pos+exponent*step<=high && array[pos+exponent*step].dateNum<target)
                exponent*=2;
            
            int previousExponent=exponent/2;

            low=pos+previousExponent*step+1;
            high=pos+exponent*step;

            if(high>size-1)
                high=size-1;
        }
        

        if(target<array[pos].dateNum){
            while(pos-exponent*step>=low && array[pos-exponent*step].dateNum>target)
                exponent*=2;

            int previousExponent=exponent/2;

            high=pos-1-(previousExponent)*step;
            low=pos-exponent*step;

            if(low<0)
                low=0;
        }
        
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
    double cpu_time_used;
    
    start = clock();
    long index = binaryInterpolationSearch(records, count, target);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Binary Interpolation Search took %f seconds.\n", cpu_time_used);
    start = clock();
    long index2 = ModifiedbinaryInterpolationSearch(records, count, target);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Modified Binary Interpolation Search took %f seconds.\n", cpu_time_used);
    printAllCumulative(records, count, target, index);
    printf("------------------------------------------/------------------------------------------\n");
    printAllCumulative(records, count, target, index2);
    free(records);  //αποδέσμευση μνήμης
    return 0;
}

