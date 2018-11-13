#include <iostream>
#include <math.h>
#include <time.h>
using namespace std;


// Random number generator. If selector equals 1, it's exponential distribution.
// If selector equals 2, it's uniform distribution. Else it return constant valuse.
// A is the range of the result. B works with A in uniform distribution.
double rng(int selector, double A, double B);

// GI/G/1 formula. S is mean service time; U is server utilization;
// Vs is sigma_s/S; Va is sigma_a/a(coefficient of variation of interarrival time).
double responseTimeFormula(double S, double U, double Vs, double Va);

//// Formula for Vs. If selector equals 1, it's exponential distribution. Else it's
//// uniform distribution. A and B is the boundary of reslut.
//double vs(int selector, double A, double B) {
//    if (selector == 1) {
//        return 1;
//    }
//    return (B-A)/(sqrt(3.0)*(A+B));
//}

// Formula for standard deviation. The parameters are an array and the length of the array.
double standardDeviation(double x[], int length);

// Formula for mean value.
double meanValue(double x[], int length);

// Sum of an array of first n elements
double sumOfArray(double x[], int n);

int main() {
    srand(time(NULL));
    
    // Variables declaration.
    int totalEvents = 40000;
    int queueMax = 50;
    int totalNumberOfArrivalEvents = 0;
    int numberOfjobsInQueue = 0;
    int counter_a = 0, counter_s = 0, counter_r = 0, counter_time = 0;
    int numOfEvent = 0;
    
    double A1 = 2., A2 = 1., B1 = 3., B2 = 2., C1 = 2., C2 = 1.;
    double serverUtilization = 0., responseTime = 0., queueLength = 0., waitingTime = 0.;
    double* arrivalEvents = new double[totalEvents];
    double* servedEvents = new double[totalEvents];
    double* responseTimeArray = new double[totalEvents];
    double timeTracker[2];
    double periodOfAnEvent = 0., totalBusyTime = 0., currentTime = 0., tNextArrival = 0.,
            tNextDeparture = 0., a = 0., s = 0., startOfServes = 0., startTime = 0.;
    bool nextEventIsArrival = false;
  
    
    // Start of simulation loop.
    while (numOfEvent < totalEvents || numberOfjobsInQueue != 0) {
        if (counter_a == 0 && counter_s == 0) { //first initialization.
            a = 2.0;
//            a = rng(1, A1, B1);
//            a = rng(2, A1, B1);
//            s = 1.0;
//            s = rng(1, A2, B2);
            s = rng(2, A2, B2);

            tNextArrival = a;
            tNextDeparture = a+s;
            nextEventIsArrival = true;
            startTime = a;
        }
        if (nextEventIsArrival == true){
            currentTime = tNextArrival;
            timeTracker[counter_time++%2] = currentTime;
            if (counter_time > 1) {
                periodOfAnEvent = timeTracker[(counter_time-1)%2] - timeTracker[(counter_time-2)%2];
                queueLength += periodOfAnEvent*numberOfjobsInQueue;
            }
            if (numberOfjobsInQueue == 0) {
                startOfServes = currentTime;
            }
            totalNumberOfArrivalEvents++;
            numberOfjobsInQueue++;
            arrivalEvents[counter_a++] = a;
            a = 2.0;
//            a = rng(1, A1, B1);
//            a = rng(2, A1, B1);
            tNextArrival += a;
            numOfEvent++;
        }
        if (nextEventIsArrival == false) {
            currentTime = tNextDeparture;
            timeTracker[counter_time++%2] = currentTime;
            if (counter_time > 1) {
                periodOfAnEvent = timeTracker[(counter_time-1)%2] - timeTracker[(counter_time-2)%2];
                queueLength += periodOfAnEvent*numberOfjobsInQueue;
            }
            servedEvents[counter_s++] = s;
//            s = 1.0;
//            s = rng(1, A2, B2);
            s = rng(2, A2, B2);
            numberOfjobsInQueue--;
            if (numberOfjobsInQueue != 0) {
                tNextDeparture = currentTime + s;
            }
            if (numberOfjobsInQueue == 0) {
                totalBusyTime += (currentTime - startOfServes);
            }
            responseTimeArray[counter_r++] = (currentTime - sumOfArray(arrivalEvents, counter_s-1));
            responseTime += currentTime - sumOfArray(arrivalEvents, counter_s-1);
            numOfEvent++;
        }
        // decide next event
        if (numberOfjobsInQueue == 0) {
            nextEventIsArrival = true;
            tNextDeparture = sumOfArray(arrivalEvents, counter_s) + a + s;
        } else {
            if (tNextArrival < tNextDeparture) {
                nextEventIsArrival = true;
            } else {
                nextEventIsArrival = false;
            }
        }
    }
//    if (numberOfjobsInQueue > 0) {
//        totalBusyTime += (currentTime - startOfServes);
//    }
    
    // Compute all statistics.
    serverUtilization = totalBusyTime/(currentTime-startTime);
    queueLength /= (currentTime - startTime);
    responseTime /= (counter_r);
    for (int i = 0; i <= counter_r-1; i++) {
        waitingTime += (responseTimeArray[i] - servedEvents[i]);
    }
    waitingTime /= (counter_r);
    
    
    double X = totalNumberOfArrivalEvents/(currentTime-startTime);
    double S = meanValue(servedEvents, counter_s-1);
    double A = meanValue(arrivalEvents, counter_a-1);
    double U = S/A;
//    double U = serverUtilization;
    double Vs = standardDeviation(servedEvents, counter_s)/S;
    double Va = standardDeviation(arrivalEvents, counter_a)/A;
    double R = responseTimeFormula(S, U, Vs, Va);
//    double R = responseTimeFormula(S, U, 0, 1);
//    double Q = U/(1-U);
    double Q = R * X;
    double W = R-S;
    
    // Showing the result.
    cout << "Total number of events is: " << numOfEvent << endl;
    cout << "Server Utilization U is: " << serverUtilization << endl;
    cout << "In analytical U is: " << U << endl << endl;
    
    cout << "Server mean queue length Q is: " << queueLength << endl;
    cout << "In analytical Q is: " << Q << endl << endl;
    
    cout << "Server mean response time R is: " << responseTime << endl;
    cout << "In analytical R is: " << R << endl << endl;
    
    cout << "Server average waiting time W is: " << waitingTime << endl;
    cout << "In analytical W is: " << W << endl << endl;
    
    delete[] arrivalEvents;
    delete[] servedEvents;
    return 0;
}

double rng(int selector, double A, double B) {
    if (selector == 1) {
        return -A*log(double(rand())/RAND_MAX);
    }
    if (selector == 2) {
        return A+(B-A)*(double(rand())/RAND_MAX);
    }
    if (selector == 3) {
        return 2.;
    }
    return 1.;
}

double responseTimeFormula(double S, double U, double Vs, double Va) {
    return S/(1-U)*(1-U/2*(1-Vs*Vs-((Vs*Vs+1)*(Va*Va-1)/(U*U*Vs*Vs+1))));
}

double standardDeviation(double x[], int length) {
    double mean = 0., sigma = 0.;
    for (int i = 0; i < length; i++) {
        mean += x[i];
        sigma += x[i]*x[i];
    }
    mean /= length;
    return sigma = sqrt(sigma/length - mean*mean);
}

double meanValue(double x[], int length) {
    double mean = 0.;
    for (int i = 0; i <= length; i++) {
        mean += x[i];
    }
    return mean /= length;
}

double sumOfArray(double x[], int n) {
    double sum = 0.;
    for (int i = 0; i <= n; i++) {
        sum += x[i];
    }
    return sum;
}
