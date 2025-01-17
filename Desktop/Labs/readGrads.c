#include "worksheet2.h"

int readGrads (FILE *file, gradStudent g [MAX_GRAD_STUDENTS]){

    int count = 0;
    char gradInfo[101]; 
    char fnG [MAX_LEN_NAME], lnG [MAX_LEN_NAME], fnS [MAX_LEN_NAME], lnS [MAX_LEN_NAME];

    while(!feof (file)){

        fgets (gradInfo, 100, file);

        sscanf (gradInfo, "%s %s %s %s %d %f", fnG, lnG, fnS, lnS, &g[count].numCoursesDone, &g[count].currentPercentage);

        strcpy (g[count].name, fnG);
        strcat (g[count].name, " ");
        strcat (g[count].name, lnG);
        
        strcpy (g[count].supervisor, fnS);
        strcat (g[count].supervisor, " ");
        strcat (g[count].supervisor, lnS);

        count++;
    }
    return count;
}