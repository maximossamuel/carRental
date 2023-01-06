/*
 * Maximos Samuel
 * 1184139
 * October 20, 2022
 * CIS*2520 A2 Q1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct car_struct{
    int mileage;
    int expectedReturnDate;
    char plateNumber[500];

    struct car_struct* next;
} car;

void loadCarsFromFile (char filename[100], car** carList);
void carFromFile (FILE * file, car ** carList);
void addCarToList (car ** carList, car * newCar);
void sortMileage (car ** carListHead);
void sortReturnDate (car ** carListHead);
void createCar (car** carList, char newPlateNumber[500]);
bool doesPlateNumberExist (char plateNumberCheckedFor[500], car* carList);
int moveCar (car** oldCarList, car** newCarList, char oldListString[20], char newListString[20], bool mustEnterMileage, bool rentalTransaction);
float calculateFee (int mileageIncrease);
void printList (car * carList);
void saveToFile (char filename[100], car * carList);
void freeList (car * carList);

int main(){
    int playerChoice;
    float totalProfits;
    char tempPlateNumber[100];

    car* availableForRent;
    car* rented;
    car* inRepair;

    playerChoice = -1;
    totalProfits = 0;

    availableForRent = NULL;
    rented = NULL;
    inRepair = NULL;

    //Loading files and storing them into the appropriate lists
    loadCarsFromFile ("availableForRent.txt", &availableForRent);
    loadCarsFromFile ("inRepair.txt", &inRepair);
    loadCarsFromFile ("rented.txt", &rented);

    printf ("Welcome to the Car Rental Portal!\n");

    while (playerChoice != 7){
        
        //Sort occurs after each transaction.
        sortMileage (&availableForRent);
        sortMileage (&inRepair);
        sortReturnDate (&rented);

        printf ("==========================================================\n");
        printf ("Select an option from the menu below:\n");
        printf ("1. Add new car to the available-for-rent list\n");
        printf ("2. Add a returned car to the available-for-rent list\n");
        printf ("3. Add a returned car to the repair list\n");
        printf ("4. Transfer a car from the repair list to the available-for-rent list\n");
        printf ("5. Rent the first available car\n");
        printf ("6. Print all the lists\n");
        printf ("7. Quit\n\n");
        printf ("Enter your choice: ");
        scanf ("%d", &playerChoice);
        getchar();

        switch (playerChoice){

            case 1:
                //Check to see if the plate number of the new car already exists.
                //Creation of new car occurs if not
                {
                    printf ("Please enter the new car's plate number: ");
                    fgets (tempPlateNumber, 500, stdin);
                    tempPlateNumber [strlen (tempPlateNumber) - 1] = '\0';

                    while (doesPlateNumberExist (tempPlateNumber, availableForRent) || doesPlateNumberExist (tempPlateNumber, rented) || doesPlateNumberExist (tempPlateNumber, inRepair)){
                        tempPlateNumber[0] = '\0';
                        printf ("Sorry, but this plate is already being used. Please enter the new car's plate number: ");
                        fgets (tempPlateNumber, 500, stdin);
                        tempPlateNumber [strlen (tempPlateNumber) - 1] = '\0';
                    }

                    createCar (&availableForRent, tempPlateNumber);
                }
                break;
            
            case 2:

                //Moving a car from rented to available for rent
                totalProfits = totalProfits + calculateFee (moveCar (&rented, &availableForRent, "RENTED", "AVAILABLE FOR RENT", true, false));
                break;
            
            case 3:

                //Moving a car from rented to repairs
                totalProfits = totalProfits + calculateFee (moveCar (&rented, &inRepair, "RENTED", "REPAIRS", true, false));
                break;

            case 4:

                //Moving a car from repairs to available for rent
                moveCar (&inRepair, &availableForRent, "REPAIRS", "AVAILABLE FOR RENT", false, false);
                break;

            case 5:

                //Renting out a car (move from available to rented)
                moveCar (&availableForRent, &rented, "AVAILABLE FOR RENT", "RENTED", false, true);
                break;

            case 6:
                {
                    printf ("AVAILABLE FOR RENT\n=================\n");
                    printList (availableForRent);

                    printf ("IN REPAIR\n================\n");
                    printList (inRepair);

                    printf ("RENTED\n================\n");
                    printList (rented);
                }
                break;

            case 7:
                {
                    printf ("Total profits made: $%.2f\n", totalProfits);

                    saveToFile ("rented.txt", rented);
                    saveToFile ("availableForRent.txt", availableForRent);
                    saveToFile ("inRepair.txt", inRepair);

                    freeList (rented);
                    freeList (availableForRent);
                    freeList (inRepair);
                    return 0;
                }

            default:

                printf ("Invalid input.\n");
                break;
        }      
    }
}

/* 
 Opens a file named the same the filename parameter
 and keeps adding cars from the file to the appropriate
 list until end of the file is reached.
 */
void loadCarsFromFile (char filename[100], car** carList){
    FILE * fptr;

    fptr = NULL;

    fptr = fopen (filename, "r");

    while (!feof (fptr)){
        carFromFile (fptr, &(*carList));
    }

    fclose (fptr);
}

/*
 Creates and stores info from files in a new car node and then
 adds it to the car list.
 */
void carFromFile (FILE * file, car ** carList){
    car * newCar;

    newCar = malloc (sizeof (car));

    fgets (newCar->plateNumber, 500, file);

    //removal of newline at the end
    newCar->plateNumber[strlen (newCar->plateNumber) - 2] = '\0';

    fscanf (file, "%d\n", &newCar->mileage);
    fscanf (file, "%d\n\n", &newCar->expectedReturnDate);

    //Setting next to null since this element will be placed at the end of the list
    newCar->next = NULL;

    addCarToList (&(*carList), newCar);
}

/*
 Adds the the node passed into the function to the end of the list
 also passed into the function
*/
void addCarToList (car ** carList, car * newCar){
    car * temp;
    
    temp = *carList;

    //Function is closes if found that the list is empty. Prevents seg fault
    if (*carList == NULL){
        *carList = newCar;
        return;
    }
    
    //traversal of list
    while (temp->next != NULL){
        temp = temp->next;
    }

    //insertion of new car into list
    temp->next = newCar;
}

/*
 Mainly used for the available and repair lists. Takes the list
 passed into the function and sorts it by mileage
*/
void sortMileage (car ** carListHead){
    car * ptr;
    car * ptr2;
    char tempPlateNumber[500];
    int tempMileage;
    int tempExpectedReturnDate;

    //check to see if list is empty. If so, function exits
    if (*carListHead == NULL){
        return;
    }

    ptr = *carListHead;

     //For loop goes through each item in the list until there is nothing after ptr. Where ptr2 is...
     //then set as the next item in the linked list after ptr.
    while (ptr->next != NULL){
        ptr2 = ptr->next;

        //Branch checks if the mileage for the current car in the list is greater than that of the...
        //next element in the list. If so, a switch between the two nodes occurs.
        if (ptr->mileage > ptr2->mileage){
            tempPlateNumber[0] = '\0';

            //switches of all 3 pieces of data in list done using temp variables
            tempMileage = ptr->mileage;
            ptr->mileage = ptr2->mileage;
            ptr2->mileage = tempMileage;

            tempExpectedReturnDate = ptr->expectedReturnDate;
            ptr->expectedReturnDate = ptr2->expectedReturnDate;
            ptr2->expectedReturnDate = tempExpectedReturnDate;

            strcpy (tempPlateNumber, ptr->plateNumber);
            ptr->plateNumber[0] = '\0';
            strcpy (ptr->plateNumber, ptr2->plateNumber);
            ptr2->plateNumber[0] = '\0';
            strcpy (ptr2->plateNumber, tempPlateNumber);

            //ptr->next is then set to ptr2. Meaning the lower mileage car now points to the higher mileage car.
            ptr->next = ptr2;

            //Everytime a switch occurs, ptr is set to the head node to check the list again. This will keep
            //occuring until ptr reaches the end of the list without running into any nodes that need to be sorted.
            ptr = *carListHead;
            continue;
        }

        ptr = ptr->next;
    }
}

/*
 Mainly used for the rented list. Takes the list
 passed into the function and sorts it by return date.
 Works the same as previous function only sorts by different
 data
*/
void sortReturnDate (car ** carListHead){
    car * ptr;
    car * ptr2;
    char tempPlateNumber[500];
    int tempMileage;
    int tempExpectedReturnDate;

    //check to see if list is empty. If so, function exits
    if (*carListHead == NULL){
        return;
    }


    ptr = *carListHead;

    while (ptr->next != NULL){
        ptr2 = ptr->next;

        //Branch checks if the date for the current car in the list is greater than that of the...
        //next element in the list. If so, a switch between the two nodes occurs.
        if (ptr->expectedReturnDate > ptr2->expectedReturnDate){
            tempPlateNumber[0] = '\0';

            //switches of all 3 pieces of data in list done using temp variable
            tempMileage = ptr->mileage;
            ptr->mileage = ptr2->mileage;
            ptr2->mileage = tempMileage;

            tempExpectedReturnDate = ptr->expectedReturnDate;
            ptr->expectedReturnDate = ptr2->expectedReturnDate;
            ptr2->expectedReturnDate = tempExpectedReturnDate;

            strcpy (tempPlateNumber, ptr->plateNumber);
            ptr->plateNumber[0] = '\0';
            strcpy (ptr->plateNumber, ptr2->plateNumber);
            ptr2->plateNumber[0] = '\0';
            strcpy (ptr2->plateNumber, tempPlateNumber);

            //ptr->next is then set to ptr2. Meaning the lower date car now points to the higher date car.
            ptr->next = ptr2;

            ptr = *carListHead;
            continue;
        }

        ptr = ptr->next;
    }
}

/*
 * User creates a new car and then puts it through addCarToList
 */
void createCar(car** carList, char newPlateNumber[500]){
    car * newCarNode;

    newCarNode = malloc (sizeof (car));

    //Since this function is only used for the available list, there is no need
    //for the return date
    newCarNode->expectedReturnDate = 0;

    //Check to ensure user inputs a positive mileage
    printf ("Please enter the car's mileage: ");
    scanf ("%d", &newCarNode->mileage);

    while (newCarNode->mileage < 0){
        printf ("Invalid input. The car's mileage should be at least 0. Please enter the new car's milage: ");
        scanf ("%d", &newCarNode->mileage);
    }

    strcpy (newCarNode->plateNumber, newPlateNumber);

    addCarToList (carList, newCarNode);

    printf ("Successfully created car %s and added it to AVAILABLE FOR RENT.\n", newPlateNumber);
}

/*
 * Takes in a string and checks the passed list to see if a plate number of
 * that string exists within the list. 
 */
bool doesPlateNumberExist (char plateNumberCheckedFor[500], car* carList){
    car * ptr;

    //Automatically returns false if list is empty
    if (carList == NULL){
        return false;
    }
    else{
        ptr = carList;

        //Returns true once the same string is found. Traverses the list if not. 
        //Returns false if end of list is reached
        while (ptr != NULL){
            if (strcmp (plateNumberCheckedFor, ptr->plateNumber) == 0){
                return true;
            }
            else{
                ptr = ptr->next;
            }
        }
    }

    return false;
}

/*
 * moves car from oldCarList to newCarList. Performs certain operations depending on
 * boolean values passed into the function. Returns the amount of miles the car has been driven
 * during a rental to calculate fees.
 */
int moveCar (car** oldCarList, car** newCarList, char oldListString[20], char newListString[20], bool mustEnterMilage, bool isRental){
    car * ptr;
    car * prevPtr;
    int mileageInput;
    int mileageIncrease;
    char searchedPlateNumber[500];

    ptr = *oldCarList;
    prevPtr = NULL;

    //If the transaction is not a rental then  this is a return or transfer transaction. 
    //This means a plate number needs to be searched for
    if (!isRental){
        printf ("Please enter the plate number of the car you wish to transfer: ");
        fgets (searchedPlateNumber, 500, stdin);
        searchedPlateNumber [strlen (searchedPlateNumber) - 1] = '\0';

        //Check to see if inputted plate number exists. Loop continues iterate if not
        while (!doesPlateNumberExist (searchedPlateNumber, *oldCarList)){
            searchedPlateNumber[0] = '\0';
            printf ("Sorry, but this plate number either does not exist or is not in the %s list. Please enter the plate number of the car you wish to transfer: ", oldListString);
            fgets (searchedPlateNumber, 500, stdin);
            searchedPlateNumber [strlen (searchedPlateNumber) - 1] = '\0';
        }
    }
    //If the transaction is a rental, then a return date needs to be inputted.
    else{
        printf ("Please enter the expected return date for this car: ");
        scanf ("%d", &ptr->expectedReturnDate);

        //Check to see if the return date is valid. Loop iterates if not.
        while (ptr->expectedReturnDate < 22000 || ptr->expectedReturnDate > 999999 || 
        (ptr->expectedReturnDate % 100) < 1 || (ptr->expectedReturnDate % 100) > 31 || 
        (ptr->expectedReturnDate % 10000) < 100 || (ptr->expectedReturnDate % 10000) > 1299) {
            printf ("Invalid input. Please enter the expected return date for this car: ");
            scanf ("%d", &ptr->expectedReturnDate);
        }

        //searchedPlateNumber is automatically set as the plate number of the first element
        //in the available list. This makes it so that it is what is rented out
        strcpy (searchedPlateNumber, ptr->plateNumber);
    }

    //traversal of list if desired string is not found
    while (strcmp (searchedPlateNumber, ptr->plateNumber) != 0){
        prevPtr = ptr;
        ptr = ptr->next;
    }

    //If head is what is being moved, head is set equal to the next node in
    //the list
    if (prevPtr == NULL){
        *oldCarList = ptr->next;
    }
    //next of the previous node of list is set to the next node of current element...
    //removing the element from the old list
    else{
        prevPtr->next = ptr->next;
    }

    ptr->next = NULL;

    //If this is a return transaction, a mileage needs to be entered
    if (mustEnterMilage){
        printf ("Enter the car's new milage: ");
        scanf ("%d", &mileageInput);

        //check to ensure that the new mileage is not lower than the car's previous mileage 
        while (mileageInput < ptr->mileage){
            printf ("Invalid input. The mileage cannot be lower than %d. Enter the car's new mileage: ", ptr->mileage);
            scanf ("%d", &mileageInput);
        }

        //return date is set to 0 since the car is no longer being rented
        ptr->expectedReturnDate = 0;

        //difference is calculated (and eventually returned for the calculation of fees)
        mileageIncrease = mileageInput - ptr->mileage;

        ptr->mileage = mileageInput;
    }

    addCarToList (&(*newCarList), ptr);

    printf ("\nSuccessfully moved %s from %s to %s.\n", searchedPlateNumber, oldListString, newListString);

    if (!mustEnterMilage){
        return 0;
    }
    else{
        return mileageIncrease;
    }
}

/*
 * Calculates and returns the fees for a rental
 */
float calculateFee (int mileageIncrease){
    int kmOver200;
    float chargeOver200;

    printf ("Total km driven: %d\n", mileageIncrease);

    if (mileageIncrease <= 200){
        printf ("$80.00 flat rate for up to 200km was made.\nTotal made: $80.00\n");
        return 80.00;
    }
    else{
        kmOver200 = mileageIncrease - 200;
        chargeOver200 = kmOver200 * 0.15;

        printf ("$80.00 flat rate for up to 200km + $%.2f for 15 cents per km on %dkm was made.\nTotal made: $%.2f\n", chargeOver200, kmOver200, chargeOver200 + 80.00);
        return 80.00 + chargeOver200;
    }
}

/*
 * Traverses through the passed list and prints each element within it
 */
void printList (car * carList){
    car * ptr;
    int i;

    ptr = carList;
    i = 1;

    while (ptr != NULL){
        printf ("Car %d:\n", i);
        printf ("Plate Number: %s\n", ptr->plateNumber);
        printf ("Mileage: %d\n", ptr->mileage);
        printf ("Expected Return Date: ");

        if (ptr->expectedReturnDate == 0){
            printf ("N/A\n");
        }
        else {
            printf ("%d\n", ptr->expectedReturnDate);
        }

        ptr = ptr->next;

        i++;
        printf("\n");
    }
}

/*
 * Creates a file named after the string passed into the function
 * and saves all the elements in the passed list into that file
 */
void saveToFile (char filename[100], car * carList){
    FILE * fptr;
    car * temp;

    fptr = NULL;

    fptr = fopen (filename, "w");

    temp = carList;

    while (temp != NULL){
        fprintf (fptr, "%s\n%d\n%d\n\n", temp->plateNumber, temp->mileage, temp->expectedReturnDate);
        temp = temp->next;
    }

    fclose (fptr);
}

/*
 * Traverses through the list and frees all the elements within it
 */
void freeList (car * carList){
    car * temp;

    while (carList != NULL){
        temp = carList;
        carList = carList->next;
        free (temp);
    }    
}