/* standard stuff
original by mike marston, jun 9, 2019
 as of 9 dec 2021
written to grab three line TLEs and sort for parameters
this also reads TLEs and output w/o sort
 now runs in Terminal and asks which parameter to sort by
 greatly improved output, columns have labels
 currently revised to print out RAAN for Kara TLEs
 */

#include <stdio.h>
#include <ctype.h>  // page 1062 in book
#include <string.h> // strtok, strcpy stuff not even used yet
#include <stdlib.h> // atoi, atof not used yet
#include <math.h>  // math functions

#include <fstream>  // moving towards better C++ file handling
// does NOT like fstream.h!!

#define MU  398600.4418  // gravitational parameter
#define MIN_PER_DAY  24 * 60  // minutes per day
#define SEC_PER_DAY  (MIN_PER_DAY * 60)   //seconds per day
#define TLE_LINE_LENGTH 80

// For Mike's computer
#define INPUTFILE "tle-cards.txt"  //
#define OUTPUTFILE "tle_output.txt"  //

using namespace std;

class Tle  // this is C++
{
public:
    char platform[25];  // satellite name, first card
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
    Tle(char *satName, char * l1, char* l2)  // satName??? fill in TLE structure
    {
        strncpy(platform, satName, sizeof(platform));
        // platform is name of satellite
        strncpy(line1, l1, sizeof(line1));
        strncpy(line2, l2, sizeof(line2));
        linesGood = checksum(line1) & checksum(line2);
        int linenum;
        char mm2d[10], bstr[10], temp[12];
       // no need to scan card 1 which is the name, so scan card 2
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
        
        eccentricity = eccentricity / 1e7; // add decimal place
        
        float tmp = pow((SEC_PER_DAY / (mean_motion * 2 * 3.141592653589)), 2);
        semi_major = pow(MU, .33333333) * pow(tmp, .33333333);
        // good code here
        perigee = semi_major * (1 - eccentricity) - 6378.135;
        apogee = semi_major * (1 + eccentricity) - 6378.135;
    
    }  // end Tle definition
    
    bool checksum(char *line)
    // this fixes some characters in the BSTAR and other not needed fields
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
    
    void print(FILE* spOutput)  // print to output file for parameters
    { //if no file given prints to stdout (i.e. terminal)
        fprintf(spOutput, "Name: %s\n", platform);
        fprintf(spOutput, "satno: %d\n", satnumber );
       // fprintf(spOutput, "inclination: %f\n", inclination);
       // fprintf(spOutput, "right_ascension: %f\n", right_ascension );
       // fprintf(spOutput, "eccentricity: %f\n", eccentricity );
       // fprintf(spOutput, "arg_perigee: %f\n", arg_perigee );
       // fprintf(spOutput, "mean_anomaly: %f\n", mean_anomaly );
       // fprintf(spOutput, "mean_motion: %f\n", mean_motion );
       // fprintf(spOutput, "semi_major axis: %f\n", semi_major );
        fprintf(spOutput, "apogee: %f\n", apogee );
        fprintf(spOutput, "perigee: %f\n\n", perigee );
        //        fprintf(stdout, "", );
    }  // end of print function
    
}; // odd to see } then ; end of Tle thing

int compareSatellitesInclination(const void * a, const void * b) // sort inclination
{
    if (((Tle*)a)->inclination  < ((Tle*)b)->inclination) return -1;
    if (((Tle*)a)->inclination == ((Tle*)b)->inclination) return  0;
    if (((Tle*)a)->inclination  > ((Tle*)b)->inclination) return  1;
    return 0; //gets rid of compiler warning, should never get here
}

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
    
    FILE* spInputTLE;  // a file of all the TLEs
    
    FILE* spOutput = nullptr; // output points to file to write calculate results to
    
   // spInputTLE = fopen(“~/Desktop/analyses/input_tle.txt", "r");
  //  spOutput = fopen(“~/Desktop/analyses/selected_satellite.txt", "w”);

    spInputTLE = fopen("/Users/Charles/Documents/satellites_to_analyze/ukraine_kara/all_tles.txt", "r");
   // each case has a unique spOutput
   
    char name_card[TLE_LINE_LENGTH];
    char second_card[TLE_LINE_LENGTH];
    char third_card[TLE_LINE_LENGTH];
    
    Tle sattellites[500];  // structure named sattellites of 500 lines?
   // printf("sat number\n", &sattellites[0].satnumber); // can access fields?
    
  //  spInputTLE = fopen("tle_cards.txt", "r");  // read data from marston
  //  spOutput = fopen("tle_output.txt", "w");  // put satellite in marston
    
    int i = 0;
    while (feof(spInputTLE) == 0) // read in all three cards
    {
        fgets(name_card, sizeof(name_card), spInputTLE);  // get first line of TLE
        fgets(second_card, sizeof(second_card), spInputTLE);  // get second line of TLE
        fgets(third_card, sizeof(third_card), spInputTLE);  // third line of TLE
        sattellites[i] = Tle(name_card, second_card, third_card); //
        // printf("test", sattellites[0].satnumber);  // how to point at satnumber?
        // need to look for satnumber here and print to files
        i++;   // increment i
        
    }  // end of while loop, reads TLEs
         
    int numSats = i;  // count up the number of satellites handled
    //  sattellites[0].print(spOutput);  // not needed
    //  sattellites[numSats-1].print(spOutput); // not needed
    // this only prints first four satellites, leave it here for now
    
  printf("\nSatellite Program\n");
     printf("Three Line TLEs\n");
    
    char answer2 = 'f';  // define answer2 and give it a default value
    // sort by what?

    while (answer2 != 'q')
    {
        printf("\nDo What?\n\n");
        printf(" a) Sort By Perigee\n");
        printf(" b) Sort By RAAN\n");
        printf(" c) Sort By Inclination\n");
        printf(" d) Print Apogee And Perigee, Leave Sorted By Satellite Number\n");
        printf(" e) Print RAAN, Leave Sorted By Satellite Number\n\n");
        printf(" q) Quit\n\n");

    scanf(" %c", &answer2);
    
    switch (answer2)
    {
        case 'a':

    spOutput = fopen("/Users/Charles/Desktop/analyses/perigee_output.txt", "w");
            
    // sort by perigee
      qsort(&sattellites[0], i, sizeof(Tle), compareSatellitesPerigee);  // sends program to compare by perigee
      // this is the C sort routine
      //  sattellites[0].print(spOutput);  // print TLE, with sorted output
      // sattellites[i-1].print(spOutput); // also print TLE, with sorted output
      
      fprintf(spOutput, "Sort by Perigee\n\n");  // inclination or perigee or RAAN
       fprintf(spOutput, "Satno: \t Epoch \t \t Perigee \t Apogee \t Inclination \t\n");
      for(int j = 0; j < numSats; j++) fprintf(spOutput, "%d\t %d %f\t %5.2f\t %5.2f\t %5.2f \n", sattellites[j].satnumber, sattellites[j].epoch_year, sattellites[j].epoch_day, sattellites[j].perigee, sattellites[j].apogee, sattellites[j].inclination);
         // prints record number (j), sat number, and inclination
    
            break;
        case 'b':

   spOutput = fopen("/Users/Charles/Desktop/analyses/RAAN_output.txt", "w");
            
       qsort(&sattellites[0], i, sizeof(Tle), compareSatellitesRAAN);
       
     //  sattellites[0].print(spOutput);  // spOutput
     //  sattellites[i-1].print(spOutput); // spInput to spOutput
       fprintf(spOutput, "Sort by RAAN\n\n");
      fprintf(spOutput, "Sequential: \t Satno: \t RAAN \n");
       for(int j = 0; j < numSats; j++) fprintf(spOutput, "%d\t %d\t %5.4f\n", j, sattellites[j].satnumber, sattellites[j].right_ascension);
       // prints record number (j), sat number, and RAAN
       
            break;
            
            case 'c':
                    
       spOutput = fopen("/Users/Charles/Desktop/analyses/inclination_output.txt", "w");
            
        qsort(&sattellites[0], i, sizeof(Tle), compareSatellitesInclination);
             //  sattellites[0].print(spOutput);  // print TLE, with sorted output
             // sattellites[i-1].print(spOutput); // also print TLE, with sorted output

    fprintf(spOutput, "Sort by Inclination\n\n");
            
       fprintf(spOutput, "Satno: \t Epoch \t \t Perigee \t Apogee \t Inclination \t\n");
             for(int j = 0; j < numSats; j++) fprintf(spOutput, "%d\t %d %f\t %5.2f\t %5.2f\t %5.4f \n", sattellites[j].satnumber, sattellites[j].epoch_year, sattellites[j].epoch_day, sattellites[j].perigee, sattellites[j].apogee, sattellites[j].inclination);
                // prints record number (j), sat number, and inclination
            
            break;
  
            case 'd':
            spOutput = fopen("/Users/Charles/Desktop/analyses/apogee_perigee_output.txt", "w");
   fprintf(spOutput, "Output Perigee and Apogee \n");
            
   fprintf(spOutput, "Satno: \t Epoch \t\t Perigee \t Apogee \t Inclination \t\n");
            for(int j = 0; j < numSats; j++) fprintf(spOutput, "%d\t %d %f\t %5.2f\t %5.2f %5.2f\n", sattellites[j].satnumber, sattellites[j].epoch_year, sattellites[j].epoch_day, sattellites[j].perigee, sattellites[j].apogee, sattellites[j].inclination);
                  // prints record number (j), sat number, apogee and perigee
            
            break;
            
        case 'e':
          spOutput = fopen("/Users/Charles/Desktop/analyses/Kara_RAAN_print.txt", "w");
   fprintf(spOutput, "Output RAAN \n");
            
   fprintf(spOutput, "Sequential: \t Satno: \t RAAN: \n");
            for(int j = 0; j < numSats; j++) fprintf(spOutput, "%d %d %f\t %5.2f\n", sattellites[j].satnumber, sattellites[j].epoch_year, sattellites[j].epoch_day, sattellites[j].right_ascension);
                // prints sequential number, sat number, RAAN
          
          break;
            
        case 'q':
            printf("You said to quit so I will quit\n");
            break;
        default:
            printf("Invalid selection. Try again.\n");
            break;
   // end of switch

    /*  original code
    for(int j = 0; j < numSats; j++) fprintf(spOutput, "record %d\t satno %d\t epochyr %d\t epochday %f\t inclin %f\t perigee: %f\t apogee: %f\n", j, sattellites[j].satnumber, sattellites[j].epoch_year, sattellites[j].epoch_day, sattellites[j].inclination, sattellites[j].perigee, sattellites[j].apogee);
    // prints record number (j), sat number, and inclination
  
     */

     }  // end of switch statement
        
    fclose(spInputTLE);
    
    fclose(spOutput);
    // close all inputs and outputs, did not have that before

    }  // end of while

}// end of main
