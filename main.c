#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#define MAX_CITY_LENGTH 20
#define MAX_TIME 6

struct passenger { // passenger data
    int id;
    int travel_date;
    char travel_time[MAX_TIME];
    char from[MAX_CITY_LENGTH];
    char dest[MAX_CITY_LENGTH];
    int busNumber;
};
struct bus { // bus data
    int number;
    int date;
    char dep_time[MAX_TIME];
    char takeOffPoint[MAX_CITY_LENGTH]; // from
    char dest[MAX_CITY_LENGTH]; // destination
    int ticket_price;
    int capacity;
    int noOfOccupiedSeats;
};
struct node_passenger { // passenger node to make a linked list of passengers
    struct passenger data;
    struct node_passenger *next;
    struct node_passenger* next_2;
    /** note: 'in this project, I used two next pointers for the passenger node, I did this to be able to put one node
                in two different lists'.
    **/
};
struct node_bus { // bus node to make a linked list of busses
    struct bus data;
    struct node_bus *next;
    struct node_passenger *passengers; // pointer to the list of passengers for the  bus
};

typedef struct node_passenger *PassengerList;
typedef PassengerList passengerPtr;
typedef struct node_bus *BusList;
typedef BusList busPtr;
BusList makeEmptyBusList (busPtr busPtr1);
void readBussesFile(struct node_bus *busList);
void readPassengersFile(PassengerList L);
void printPassengerInfo(struct passenger p);
PassengerList makeEmptyPassengerList(PassengerList L);
void printPassengerList(PassengerList L, int mode);
void printBusList(BusList list);
void printBusInfo(struct bus bus);
void assignPassenger(BusList busses, passengerPtr p, PassengerList unmatchedList);
float timeValue(char time[]);
void traverseAndCallAssign(BusList busses, PassengerList passengers, struct node_passenger *unmatched);
void insertFirstBusNode(busPtr busPtr1, BusList L);
void insertFirstPassengerNode(passengerPtr passengerPtr1, struct node_passenger *L, int mode);
void printPassengerSpecifiedInfo(PassengerList list);
void printBusInfoWithAssignedPassengersInfo(BusList busList);
void addNewPassenger(BusList busList, PassengerList unmatched, PassengerList passengerList);
void deletePassenger(BusList busList, PassengerList unmatched, PassengerList passengerList);
void deleteBus(BusList busList, PassengerList unmatched);
busPtr findPreviousBus(int number, BusList busList);
passengerPtr findPreviousPassenger(passengerPtr X, PassengerList L, int mode);
void deletePassengerList(struct node_passenger *L, int mode);
void deleteBusList(busPtr busPtr1);

int main() {
    // declare and initialize the list of busses
    BusList bussesList = (struct node_bus *) malloc(sizeof(struct node_bus));
    bussesList->next = NULL;
    // declare and initialize the list of passengers
    PassengerList passengersList = (PassengerList)malloc(sizeof (struct node_passenger));
    passengersList->next= NULL;
    passengersList->next_2= NULL;

    // declare and initialize the list of unmatched passengers
    PassengerList unmatched = NULL;
    unmatched = makeEmptyPassengerList(unmatched);

    int choice; // user choice
    do {
        // menu
        printf("\n1. Load the bus information file");
        printf("\n2. Load the passenger information file");
        printf("\n3. print assignment information of all busses");
        printf("\n4. Print a specific bus information along with its passengers information (names and IDs)");
        printf("\n5. Print unmatched passengersList");
        printf("\n6. Add new passenger");
        printf("\n7. delete passenger");
        printf("\n8. delete bus number");
        printf("\n0. Exit the program\n\n");

        printf("Enter your choice: ");
        scanf("%d", &choice); // take choice from user
        printf("\n");

        // switch statement to display output depending on the user choice
        switch (choice) {

            case 0: // exit the program (free the lists in your way)

                deletePassengerList(passengersList, 1);
                deletePassengerList(passengersList, 2);
                deleteBusList(bussesList);

                printf("\nHave a good day\n");
                break;

            case 1: // load the busses file
                makeEmptyBusList(bussesList);
                // read the busses file and store the busses in bussesList linked-list.
                readBussesFile(bussesList);

                printf("\n%-15s%-15s%-10s%-20s%-17s%-10s%-15s\n", "BUS NO.", "DATE", "TIME", "TAKE OF POINT",
                       "DESTINATION","PRICE", "CAPACITY");
                // print the busses info
                printBusList(bussesList);
                break;
            case 2: // load the passengers file
                passengersList = makeEmptyPassengerList(passengersList);
                readPassengersFile(passengersList); // read the passengers and store them in a linked-list
                printf("\n%-15s%-15s%-10s%-20s%-15s\n", "STUDENT ID", "TRAVEL DATE", "TIME", "FROM", "DESTINATION");
                printPassengerList(passengersList, 1); // print the passengers info
                break;
            case 3: // assign the passengers to the busses
                deletePassengerList(unmatched, 2); /* empty the unmatched list before assignment (to use 1-2-3 options more than once)*
                 but we have to press 1 and 2 before we press 3,*/
                // loop through the list of passengers and call the assignPassenger method on them.
                traverseAndCallAssign(bussesList, passengersList, unmatched);
                break;
            case 4:
                // print the bus info after entering the bus number, also print id and time for each passenger assigned to it.
                printBusInfoWithAssignedPassengersInfo(bussesList);
                break;
            case 5:
                // print the unmatched passengers list.
                printf("\n%-15s%-15s%-10s%-20s%-15s\n", "STUDENT ID", "TRAVEL DATE", "TIME", "FROM", "DESTINATION");
                printPassengerList(unmatched, 2);
                break;
            case 6:
                // add new passenger to the list and assign it.
                addNewPassenger(bussesList, unmatched, passengersList);
                break;
            case 7:
                // delete a passenger by entering the id.
                deletePassenger(bussesList, unmatched, passengersList);
                break;
            case 8:
                // delete a bus by entering the bus number, and reassign the passengers assigned to it
                deleteBus(bussesList, unmatched);
                break;

            default:
                printf("Invalid choice");
                break;

        }

    } while (choice != 0);

    return 0;
}
// read the busses file and store the data in the linked list of busses.
void readBussesFile(BusList busList) {

    FILE *fptr2 = fopen("busses.txt", "r");
    char *arr2 = (char *) malloc(sizeof(char) * 100);

    if (!fptr2) // file not found.
        printf("Couldn't open busses file");


    while (fgets(arr2, 100, fptr2) != NULL) {

        struct bus bus1;

        // scan the line we brought with fgets, store each data in the struct of bus data.
        sscanf(arr2, "%d#%d#%[^#]#%[^#]#%[^#'\n]#%d#%d", &(bus1.number), &(bus1.date), bus1.dep_time, bus1.takeOffPoint,
               bus1.dest, &(bus1.ticket_price), &(bus1.capacity));

        // initialize the number of occupied seats of the bus
        bus1.noOfOccupiedSeats = 0;
        // declare and initialize the node_bus to insert it in the list.
        busPtr busPtr1 = (busPtr) malloc(sizeof(struct node_bus));
        busPtr1->data = bus1;
        busPtr1->next = NULL;
        // declare and initialize the list of passengers of the bus.
        busPtr1->passengers = (PassengerList) malloc(sizeof(struct node_passenger));
        busPtr1->passengers->next = NULL;
        busPtr1->passengers->next_2 = NULL;
        // insert the bus node in the beginning of the list of busses.
        insertFirstBusNode(busPtr1, busList);
    }
}
// read the passengers file and store the data in the linked list of passengers.
void readPassengersFile(PassengerList L) {

    FILE *fptr = fopen("passengers.txt", "r");
    char *arr;
    arr = (char *) malloc(sizeof(char) * 100);

    if (!fptr)
        printf("Couldn't open passengers file");

    while (fgets(arr, 100, fptr) != NULL) {

        struct passenger passenger;

        /* scan the line read by fgets and store each data in the variables of the struct of passenger data */
        sscanf(arr, "%d#%d#%[^#]#%[^#]#%[^#''\n']#", &(passenger.id), &(passenger.travel_date), passenger.travel_time,
               passenger.from, passenger.dest);

        // declare and initialize the struct node passenger.
        passengerPtr passengerPtr1 = (passengerPtr) malloc(sizeof(struct node_passenger));
        passengerPtr1->data = passenger;
        passengerPtr1->next = NULL;
        passengerPtr1->next_2 = NULL;

        passengerPtr1->data.busNumber = -1;
        insertFirstPassengerNode(passengerPtr1, L, 1);
    }
}
// is empty, returns true if the list is empty or the header points to null.
int isEmptyPassengerList(PassengerList L, int mode) {
    if (mode == 1)
        return L == NULL || L->next == NULL;

    return L==NULL || L->next_2 == NULL;
}

int isEmptyBusList(BusList L) {
    return L == NULL || L->next == NULL;
}

// returns true if the passengers' node next is null.
int isLastPassenger(passengerPtr P, PassengerList L, int mode) {

    if (mode == 1)
        return !isEmptyPassengerList(L, 1) && P->next == NULL;

    else
        return !isEmptyPassengerList(L, 2) && P->next_2 == NULL;

}

int isLastBus(busPtr bus, BusList L) {

    return !isEmptyBusList(L) && bus->next == NULL;
}
// method to delete the list of passengers and free the node data one by one. mode 1 to delete the list of all passengers,
// mode 2 to delete the list of passengers in unmatched list of a list of passengers for a bus.
void deletePassengerList(struct node_passenger *L, int mode) {

    struct node_passenger *p, *temp;

    if (mode == 1) {
        if (!isEmptyPassengerList(L, 1)) {
            p = L->next;
            L->next = NULL;

            while (p != NULL) {
                temp = p->next;
                free(p);
                p = temp;
            }
        }
    }

    else {
        if (!isEmptyPassengerList(L, 2)) {
            p = L->next_2;
            L->next_2 = NULL;
            while (p != NULL) {
                temp = p->next_2;
                free(p);
                p = temp;
            }
        }
    }
}
// makes a new passenger node header and returns it.
PassengerList makeEmptyPassengerList(struct node_passenger *L) {

    if (L != NULL) {
        deletePassengerList(L, 2);
        deletePassengerList(L, 1);
    }

    L = (PassengerList) malloc(sizeof(struct node_passenger));

    if (L == NULL) {
        printf("Error! out of memory!");
        return NULL;
    }

    L->next = NULL;
    L->next_2 = NULL;

    return L;
}
// find the node before the node we passed.
passengerPtr findPreviousPassenger(passengerPtr X, PassengerList L, int mode) {

    passengerPtr P = L;

    if (mode == 1) {
        if (isEmptyPassengerList(L, 1))
            return NULL;

        while (P->next != NULL && P->next->data.id != X->data.id)
            P = P->next;
        return P;
    }

    else {
        if (isEmptyPassengerList(L, 2))
            return NULL;

        while (P->next_2 != NULL && P->next_2->data.id != X->data.id)
            P = P->next_2;
        return P;
    }
}
// delete a passenger node X from the list L, with mode (1 to delete from list of all passengers, 2 to delete from a bus or unmatched).
void deletePassengerNode(passengerPtr X, PassengerList L, int mode) {


    passengerPtr P, temp;
    if (mode == 1) {

        if (isEmptyPassengerList(L, 1)) {
            printf("\nEmpty passenger list!");
            return;
        }

        P = findPreviousPassenger(X, L, 1);
        if (!isLastPassenger(P, L, 1)) {
            temp = P->next;
            P->next = temp->next; //bypass delete cell
            free(temp);
        }
    }

    else if (mode == 2) {

        if (isEmptyPassengerList(L, 2)) {
            printf("\nEmpty passenger list!");
            return;
        }


        P = findPreviousPassenger(X, L, 2);
        if (!isLastPassenger(P, L, 2)) {
            temp = P->next_2;
            P->next_2 = temp->next_2; //bypass delete cell
            free(temp);
        }
    }
}
// print passenger info in an escaped sequence
void printPassengerInfo(struct passenger p) {

    printf("%-15d%-15d%-10s%-20s%-15s\n", p.id, p.travel_date, p.travel_time, p.from, p.dest);
}
// print list of passengers, mode 1 to print all passengers, 2 to print passengers in a specific bus list, or unmatched list.
void printPassengerList(PassengerList L, int mode) {


    if (mode == 1) {
        if (isEmptyPassengerList(L, 1))
            return;

        passengerPtr P = L;
        do {
            P = P->next;
            printPassengerInfo(P->data);
        } while (!isLastPassenger(P, L, 1));
    }
    else if (mode == 2) {
        if (isEmptyPassengerList(L, 2))
            return;

        passengerPtr P = L;
        do {
            P = P->next_2;
            printPassengerInfo(P->data);
        } while (!isLastPassenger(P,L,2));
    }

    printf("\n");
}

// insert a new node to the list, mode 1 to insert to the list of all passengers, mode 2 to insert to unmatched or a specific bus list of passengers.
void insertFirstPassengerNode(passengerPtr passengerPtr1, PassengerList L, int mode) {

    if (L == NULL) {
        printf("The header in this list points to NULL!, cannot insert\n");
        return;
    }
    if (mode == 1) {
        passengerPtr1->next = L->next;
        L->next = passengerPtr1;
    }
    else if (mode == 2) {
        passengerPtr1->next_2 = L->next_2;
        L->next_2 = passengerPtr1;
    }

}
// print the bus info in an escaped sequence.
void printBusInfo(struct bus bus) {

    printf("%-15d%-15d%-10s%-20s%-17s%-10d%-15d\n", bus.number, bus.date, bus.dep_time,
           bus.takeOffPoint, bus.dest, bus.ticket_price, bus.capacity);
}
// traverse on the list of busses and call the print method for each nod in it.
void printBusList(BusList list) {

    if (isEmptyBusList(list))
        return;

    busPtr P = list;

    do {
        P = P->next;
        printBusInfo(P->data);
    } while (!isLastBus(P, list));

    printf("\n");

}
// assigned a passenger to some bus, or to the unmatched list if no match is found.
void assignPassenger(BusList busses, passengerPtr p, PassengerList unmatchedList) {

    busPtr temp = busses->next;

    busPtr bestBus = NULL;

    // traversing on the list of all busses to find the best bus that matches the passenger
    while (temp != NULL) {

        if (        // if we found a bus that matches the passenger

                p->data.travel_date == temp->data.date &&
                !strcasecmp(temp->data.dest, p->data.dest) &&
                !strcasecmp(temp->data.takeOffPoint, p->data.from) &&
                timeValue(p->data.travel_time) <= timeValue(temp->data.dep_time) &&
                temp->data.noOfOccupiedSeats <
                temp->data.capacity
                ) {

            // first time we find a bus(when best bus is null), then we compare other compatible busses to get the bus with the least time
            bestBus = (bestBus == NULL) ? temp : (timeValue(temp->data.dep_time) < timeValue(bestBus->data.dep_time)) ? temp : bestBus;
        }

        temp = temp->next;
    }
    // we didn't find any bus that matches the passenger; thus we send it to unmatched list
    if (bestBus == NULL) { //if we didn't find any bus that is compatible with the passenger
        printf("This passenger (%d) is unmatched and will be sent to unmatched list\n", p->data.id);
        insertFirstPassengerNode(p, unmatchedList, 2);
    }
        // we found the best bus that matches the passenger; we send that passenger to the list for that bus.
    else {
        printf("This passenger (%d) is sent to bus number (%d)\n", p->data.id, bestBus->data.number);
        bestBus->data.noOfOccupiedSeats++; // increment number of occupied seats in this bus.
        // insert the passenger in the list of passengers inside the bus.
        p->data.busNumber = bestBus->data.number;
        insertFirstPassengerNode(p, bestBus->passengers, 2);
    }
}

// this functions calculates the weight of the time for a string of format hh:mm
// used in time compare between buses and passengers in assign method.
float timeValue(char time[]) {

    float val;
    int hour;
    int minutes;
    sscanf(time, "%d:%d", &hour, &minutes);
    val = (float) (hour + (minutes / 60.0));

    return val;
}
// insert a bus node to the beginning of the list.
void insertFirstBusNode(busPtr busPtr1, BusList L) {

    if (L == NULL) {
        printf("the list is null\n");
        return;
    }

    busPtr1->next = L->next;
    L->next = busPtr1;
}

// in this function, we traverse on all passengers and call the function assign for each passenger.
void traverseAndCallAssign(BusList busses, PassengerList passengers, PassengerList unmatched) {

    passengerPtr temp = passengers->next;

    while (temp != NULL) {
        assignPassenger(busses, temp, unmatched);
        temp = temp->next;
    }
}

// this function finds and returns the bus which has the bus number ((number))
busPtr findBusWithNum(int number, BusList ourList) {

    if (ourList == NULL) {
        printf("This list points to NULL, nothing to find!");
        return NULL;
    }

    busPtr temp = ourList->next;

    while (temp != NULL && temp->data.number != number) {
        temp = temp->next;
    }

    return temp;
}

// this function prints the information of a bus and some basic information if the passengers assigned to it.
void printBusInfoWithAssignedPassengersInfo(BusList busList) {

    int busNumber;
    printf("Enter the bus number: ");
    scanf("%d", &busNumber);

    // find the bus that has the entered number
    busPtr bus = findBusWithNum(busNumber, busList);

    if (bus != NULL) {

        printf("\n%-15s%-15s%-10s%-20s%-17s%-10s%-15s\n", "BUS NO.", "DATE", "TIME", "TAKEOFF POINT", "DESTINATION",
               "PRICE", "CAPACITY");
        // print all the information of the bus
        printBusInfo(bus->data);
        printf("\n");
        // print id's and travel time of each passenger assigned to the bus
        printPassengerSpecifiedInfo(bus->passengers);
        printf("\nNumber of occupied seats = %d\n", bus->data.noOfOccupiedSeats);
    }
        //
    else
        printf("No such bus number");

}

// print id and time of the passenger.
void printPassengerSpecifiedInfo(PassengerList list) {

    passengerPtr temp = list->next_2;
    printf("\n%-20s%-20s\n", "PASSENGER ID", "TRIP TIME");

    while (temp != NULL) {
        printf("%-20d%-20s\n", temp->data.id, temp->data.travel_time);
        temp = temp->next_2;
    }

}

// add a new passenger and immediately assign it to the right bus
void addNewPassenger(BusList busList, PassengerList unmatched, PassengerList passengerList) {

    printf("\nEnter passenger's information in the following format:\n");

    printf("ID, TRIP_DATE, TRIP_TIME, TAKEOFF_POINT, DESTINATION\n");

    int id, date;
    char time[6];
    char takeoff[20];
    char dest[20];
    scanf("%d, %d, %[^,], %[^,], %[^\n]", &id, &date, time, takeoff, dest);

    struct passenger passenger;
    passenger.id = id;
    passenger.travel_date = date;
    strcpy(passenger.travel_time, time);
    strcpy(passenger.from, takeoff);
    strcpy(passenger.dest, dest);

    passengerPtr p = (passengerPtr) malloc(sizeof(struct node_passenger));
    p->data = passenger;
    p->next = NULL;
    p->data.busNumber = -1;

    // call assign for that passenger.
    assignPassenger(busList, p, unmatched);

    insertFirstPassengerNode(p, passengerList, 1);

}

// delete the passenger totally from the bus after finding it by entering the id.
void deletePassenger(BusList busList, PassengerList unmatched, PassengerList passengerList) {
    printf("\nEnter the id of the passenger: ");
    int pID;
    scanf("%d", &pID);

    passengerPtr p = passengerList->next;
    // find the node that as the entered id.
    while (p != NULL && p->data.id != pID)
        p = p->next;

    // we didn't find the passenger in the list.
    if (p == NULL) {
        printf("\nPassenger not found!\n");
        return;
    }
    // the passenger is found in the unmatched list
    if (p->data.busNumber == -1) {

        printf("\nPassenger found in unmatched list and got deleted!\n");

       deletePassengerNode(p, unmatched, 2);
    }

    else { // passenger is found in a bus
        printf("\nPassenger found in bus number %d and got deleted\n", p->data.busNumber);
        // get the bus with number from the passenger we found.
        busPtr bus = findBusWithNum(p->data.busNumber, busList);
        // delete the passenger from the list of the bus it is assigned to.
        deletePassengerNode(p, bus->passengers, 2);
        --bus->data.noOfOccupiedSeats; // decrement the number of occupied seats in the bus.
    }

}
// delete a bus and assign its passengers again.
void deleteBus(BusList busList, PassengerList unmatched) {

    int busNumber;
    printf("Enter bus number to delete it: ");
    scanf("%d", &busNumber);

    busPtr busPtr1 = findBusWithNum(busNumber, busList);
    // bus is not found in the list.
    if (busPtr1 == NULL) {
        printf("\nBus not found\n");
        return;
    }
    // get the list of passengers of the buss before deleting it.
    passengerPtr temp = busPtr1->passengers->next_2;
    passengerPtr temp2;
    printf("\nBus number %d has been deleted, and passengers were reassigned\n", busPtr1->data.number);
    // find the previous node of the bus to delete the bus.
    busPtr prev = findPreviousBus(busNumber, busList);
    prev->next = prev->next->next;
    free(busPtr1);
    // assign each passenger in the
    while (temp != NULL) {
        temp2 = temp->next_2;
        assignPassenger(busList, temp, unmatched);
        temp = temp2;
    }

}

busPtr findPreviousBus(int busNumber, BusList busList) {

    if (busList == NULL) {
        printf("Empty bus list\n");
        return NULL;
    }

    busPtr temp = busList;

    while (temp->next != NULL && temp->next->data.number != busNumber) {
        temp = temp->next;
    }

    return temp;
}
// make a new header of bus node and empty the list if it has previous data.
busPtr makeEmptyBusList (busPtr busPtr1) {

    if (busPtr1 != NULL)
        deleteBusList(busPtr1);

    busPtr1 = (BusList) malloc(sizeof(struct node_bus));
    busPtr1->next = NULL;

    return busPtr1;
}
void deleteBusList(busPtr busPtr1) {
    struct node_bus *p, *temp;

    if (!isEmptyBusList(busPtr1)) {
        p = busPtr1->next;
        busPtr1->next = NULL;

        while (p != NULL) {
            temp = p->next;
            free(p);
            p = temp;
        }
    }
}

