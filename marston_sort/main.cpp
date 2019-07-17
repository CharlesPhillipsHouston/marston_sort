/* standard stuff
by mike marston, jun 9, 2019
 as of 25 jun 2019
written to show use of libraries, sorting inside application
builds fine in xcode
changed so that code gets TLEs from charles's computer and puts output
 into right folder on charles' computer
 saved to github
 */

#include <stdio.h>
#include <ctype.h>  // page 1062 in book
#include <string.h> // strtok, strcpy stuff not even used yet
#include <stdlib.h> // atoi, atof not used yet
#include <math.h>  // math functions

#define MU  398600.4418  // gravitational parameter
#define MIN_PER_DAY  24 * 60  // minutes per day
#define SEC_PER_DAY  (MIN_PER_DAY * 60)   //seconds per day
#define TLE_LINE_LENGTH 80

// For Mike's computer
#define INPUTFILE "tle-cards.txt"  // ??
#define OUTPUTFILE "tle_output.txt"  // ??

using namespace std;

class Tle
{
public:
    char platform[25];  // satellite name
    char line1[TLE_LINE_LENGTH];
    char line2[TLE_LINE_LENGTH];
    
    int satnumber ;
    char classification[2];
    int id_launch_year ;
    int id_launch_number ;
    char id_launch_piece[4];
    int epoch_year ;
    float epoch_day ;
    char epoch[50] ;
    float mean_motion_derivative ;
    float mean_motion_sec_derivative ;
    float bstar ;
    int ephemeris_type ;
    int element_number ;
    float inclination ;
    float right_ascension ;
    float eccentricity ;
    float arg_perigee ;

    
    
    float mean_anomaly ;
    float mean_motion ;
    int orbit ;
    bool linesGood;
    //calculated parameters
    float semi_major;
    float apogee;
    float perigee;
    
    float tleStrToFloat(char * in)
    {
        char temp[10];
        if(in[0] == '-') temp[0] = '-';
        else temp[0] = ' ';
        int offset = 1;
        if(isdigit(in[0])) offset = 0;
        temp[1] = '.';
        for (int i = 0; i < 4; i++){
            temp[2+i] = in[offset+i];
        };
        temp[6] = 'e';
        temp[7] = in[offset+5];
        temp[8] = in[offset+6];
        temp[9] = '\0'; // null character
        float b = atof(temp);
        return(b);
    }
    Tle(){}; // empty constructor
    Tle(char *satName, char * l1, char* l2)  // function Tle ???
    {
        strncpy(platform, satName, sizeof(platform));
        // platform is name of satellite
        strncpy(line1, l1, sizeof(line1));
        strncpy(line2, l2, sizeof(line2));
        linesGood = checksum(line1) & checksum(line2);
        int linenum;
        char mm2d[10], bstr[10], temp[12];
       // scan card 2
        sscanf( line1, "%d %5d%1s %2d%3d%3s %2d%f %f %s %s %d %4d",
               &linenum,
               &satnumber, classification,
               &id_launch_year, &id_launch_number, id_launch_piece,
               &epoch_year, &epoch_day,
               &mean_motion_derivative,
               mm2d,
               bstr,  // not really used
               &ephemeris_type,  // not really used
               &element_number); // end scan card #1
        
        // scan card 3
            sscanf( line2, "%d %5d %f %f %f %f %f %11f%5d",
               &linenum,
                   
                   
               &satnumber,
               &inclination,
               &right_ascension,
               &eccentricity,
               &arg_perigee,
               &mean_anomaly,
               &mean_motion,
               &orbit); // scan card #1
        // now fix the assumed decimal points
        mean_motion_sec_derivative = tleStrToFloat(mm2d); // that function
        bstar = tleStrToFloat(bstr); // that function again
        /*//this block assumes values are in a std::string
         if (mm2d[0] == '-') mm2d.insert(mm2d.begin()+1, '.'); //leading - sign
         else(!isdigit(mm2d[0])) mm2d[0] = '.'; //leading blank or + sign
         //else mm2d.insert(mm2d.begin(), '.'); //digit in first character
         mm2d.insert(mm2d.end() -2, 'e');
         mean_motion_sec_derivative = atof(mm2d.c_str());
         
         if (bstr[0] == '-') bstr.insert(bstr.begin()+1, '.'); //leading - sign
         else if (!isdigit(bstr[0])) bstr[0] = '.'; //leading blank or + sign
         else bstr.insert(bstr.begin(), '.'); //digit in first character
         bstr.insert(bstr.end() -2, 'e');
         bstar = atof(bstr.c_str());
         */
        
        eccentricity = eccentricity / 1e7;
        
        float tmp = pow((SEC_PER_DAY / (mean_motion * 2 * 3.141592653589)), 2);
        semi_major = pow(MU, .33333333) * pow(tmp, .33333333);
        // good code here
        perigee = semi_major * (1 - eccentricity) - 6378.135;
        apogee = semi_major * (1 + eccentricity) - 6378.135;
    }  // end Tle definition
    
    bool checksum(char *line)  // function that must do something
    {
        int check = 0;
        int i = 0;
        char a;
        for( i = 0; i != strlen(line) -1; ++i)
        {
            a = line[i];
            if(isdigit(a))
                check += a - '0';
            if(line[i] == '-')
                check++;
        }  // end for loop
        
        
        
        
        
        
    
        a = line[strlen(line) - 1]; // -1 for 0 based another -1 to exclude the checksum
        if((check % 10) != a - '0')
            return(false);
        else
            return(true);
    }  // end checksum function
    
    void print(FILE* spOutput)
    { //if no file given prints to stdout (i.e. terminal)
        fprintf(spOutput, "Name: %s\n", platform);
        fprintf(spOutput, "satno: %d\n", satnumber );
        fprintf(spOutput, "inclination: %f\n", inclination);
        fprintf(spOutput, "right_ascension: %f\n", right_ascension );
        fprintf(spOutput, "eccentricity: %f\n", eccentricity );
        fprintf(spOutput, "arg_perigee: %f\n", arg_perigee );
        fprintf(spOutput, "mean_anomaly: %f\n", mean_anomaly );
        fprintf(spOutput, "mean_motion: %f\n", mean_motion );
        fprintf(spOutput, "semi_major axis: %f\n", semi_major );
        fprintf(spOutput, "apogee: %f\n", apogee );
        fprintf(spOutput, "perigee: %f\n\n", perigee );
        //        fprintf(stdout, "", );
    }  // end of print function
    
}; // odd to see } then ; end of Tle thing

int compareSatellitesPerigee(const void * a, const void * b) // sort perigee
{
    if (((Tle*)a)->perigee  < ((Tle*)b)->perigee) return -1;
    if (((Tle*)a)->perigee == ((Tle*)b)->perigee) return  0;
    if (((Tle*)a)->perigee  > ((Tle*)b)->perigee) return  1;
    return 0; //gets rid of compiler warning, should never get here
}

int compareSatellitesRAAN(const void * a, const void * b) // sort RAAN
{
    if (((Tle*)a)->right_ascension  < ((Tle*)b)->right_ascension) return -1;
    if (((Tle*)a)->right_ascension == ((Tle*)b)->right_ascension) return  0;
    if (((Tle*)a)->right_ascension  > ((Tle*)b)->right_ascension) return  1;
    return 0; //gets rid of compiler warning, should never get here
}



int main()
{

    FILE* spInput; // input points to file to read from
    FILE* spOutput; // output points to file to write to
    char name_card[TLE_LINE_LENGTH];
    char second_card[TLE_LINE_LENGTH];
    char third_card[TLE_LINE_LENGTH];
    
    Tle sattellites[400];  // structure of 400 lines?
    
  //  spInput = fopen("tle_cards.txt", "r");  // read data from marston
  //  spOutput = fopen("tle_output.txt", "w");  // put satellite in marston
    
    spInput = fopen("/Users/Admin/Documents/c++_marston/tle_cards.txt", "r");  // read data from
    // folder with code
    spOutput = fopen("/Users/Admin/Documents/c++_marston/tle_output.txt", "w");
    // put satellite in folder with code
    
    
    
    
    int i = 0;
    while (feof(spInput) == 0) // read in all three cards
    {
        
        fgets(name_card, sizeof(name_card), spInput);  // get first line of TLE
        fgets(second_card, sizeof(second_card), spInput);  // get first line of TLE, outside of the while
        fgets(third_card, sizeof(third_card), spInput);  // outside of the while loop
        sattellites[i] = Tle(name_card, second_card, third_card);
        i++;
    }  // end of while loop, reads TLEs
    
    int numSats = i;
    sattellites[0].print(spOutput);  // directed to spOutput
    sattellites[numSats-1].print(spOutput); // directed to spOutput
    // this only prints first four satellites
    
    // sort by perigee
    qsort(&sattellites[0], i, sizeof(Tle), compareSatellitesPerigee);
    
    sattellites[0].print(spOutput);  // spOutput
    sattellites[i-1].print(spInput); //??
    fprintf(spOutput, "sort by perigee\n\n");
    for(int j = 0; j < numSats; j++) fprintf(spOutput, "%d\t %d\t %f\n", j, sattellites[j].satnumber, sattellites[j].perigee);
    // prints record number (j), sat number, and perigee
  // need to unsort??
    
    // sort by RAAN
    qsort(&sattellites[0], i, sizeof(Tle), compareSatellitesRAAN);
    
    sattellites[0].print(spOutput);  // spOutput
    sattellites[i-1].print(spInput);
    fprintf(spOutput, "sort by RAAN\n\n");
    
    for(int j = 0; j < numSats; j++) fprintf(spOutput, "%d\t %d\t %f\n", j, sattellites[j].satnumber, sattellites[j].right_ascension);
    // prints record number (j), sat number, and perigee
    
}  // end of main
