//
//  main.cpp
//  EM
//
//  Created by Jinwei Lin on 10/18/15.
//  Copyright (c) 2015 JL. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include "Node.h"
#include "LinkedList.h"
using namespace std;

string dataset_10,dataset_30,dataset_50,dataset_70,dataset_100; //different input file names
int numOfData = 200;
int* genderArray = new int[numOfData];
int* weightArray = new int[numOfData];
int* heightArray = new int[numOfData];
LinkedList* data; // use to update expected counts of data points
double maleProb = 0.7;
double fmaleProb = 1 - maleProb;
double g0w0=0.8,g0w1=0.2,g1w0=0.4,g1w1=0.6; //probs of weight given gender
double g0h0=0.7,g0h1=0.3,g1h0=0.3,g1h1=0.7; //probs of height given gender
double likelihood;

double g0w0h0 = 0.0,g1w0h0 = 0.0,g0w0h1=0.0,g1w0h1=0.0,g0w1h0=0.0,g1w1h0=0.0,g0w1h1=0.0,g1w1h1=0.0; // variables stand for given weight is 0 and height is 0, the prob of gender is 0; given weight is 0 and height is 0, the prob of gender is 1 etc to represent the expected counts
double threshold = 0.001;
double likelihoodDiff = 0.0; // difference of data likelihood for two iterations
double mProbDiff=0.0,fProbDiff=0.0,g0w0Diff=0.0,g0w1Diff=0.0,g1w0Diff=0.0,g1w1Diff=0.0,g0h0Diff=0.0,g0h1Diff=0.0,g1h0Diff=0.0,g1h1Diff=0.0; // variables for for parameters changes

// detect the convergence of the EM Algorithm until small change in data likelihood and parameters
bool isConverge(){
    if (likelihoodDiff<=threshold && mProbDiff<=threshold && fProbDiff<=threshold && g0w0Diff<=threshold && g0w1Diff<=threshold && g1w0Diff<=threshold && g1w1Diff<=threshold && g0h0Diff<=threshold && g0h1Diff<=threshold && g1h0Diff<=threshold && g1h1Diff<=threshold) {
        return true;
    }
    else
        return false;
}

// populate data sets with expected counts
void insert(int gender,int weight,int height,double exCount){
    Node* newNode = new Node(gender,weight,height,exCount);
    Node* walker = data->get_listHead();
    while (walker->getNext() != NULL) {
        walker = walker->getNext();
    }
    newNode->setNext(walker->getNext());
    walker->setNext(newNode);
}

//for debugging purpose
void printData(){
    Node* walker = data->get_listHead()->getNext();
    while (walker != NULL) {
        cout<< walker->getGender()<<" "<<walker->getWeight()<<" "<<walker->getHeight()<<" "<<walker->getExpectCount()<<endl;
        walker = walker->getNext();
    }
}

// add initial counts to the data table
void addExpectCount(){
    for (int i=0; i<numOfData; i++) {
        if (genderArray[i] != -1) {
            insert(genderArray[i], weightArray[i], heightArray[i],1);
        } else{
            if (weightArray[i]==0 && heightArray[i]==0) {
                insert(0, 0, 0,g0w0h0);
                insert(1, 0, 0,g1w0h0);
            }
            if (weightArray[i]==0 && heightArray[i]==1) {
                insert(0, 0, 1,g0w0h1);
                insert(1, 0, 1,g1w0h1);
            }
            if (weightArray[i]==1 && heightArray[i]==0) {
                insert(0, 1, 0,g0w1h0);
                insert(1, 1, 0,g1w1h0);
            }
            if (weightArray[i]==1 && heightArray[i]==1) {
                insert(0, 1, 1,g0w1h1);
                insert(1, 1, 1,g1w1h1);
            }
        } // add expected counts to different values of weight and height
    }
    //printData();
}

// M-step, compute and update new parameters after each iteration
void computeNewParam(){
    Node* walker = data->get_listHead()->getNext();
    double sumMale=0.0,sumFmale=0.0,sumG0w0=0.0,sumG1w0=0.0,sumG0h0=0.0,sumG1h0=0.0,newMProb=0.0,newFProb=0.0,newg0w0=0.0,newg0w1=0.0,newg1w0=0.0,newg1w1=0.0,newg0h0=0.0,newg0h1=0.0,newg1h0=0.0,newg1h1=0.0;
    while (walker != NULL) {
        if(walker->getGender()==0){
            sumMale = sumMale+walker->getExpectCount();
        }
        if (walker->getGender()==0 && walker->getWeight()==0) {
            sumG0w0 = sumG0w0+walker->getExpectCount();
        }
        if (walker->getGender()==1 && walker->getWeight()==0) {
            sumG1w0 = sumG1w0+walker->getExpectCount();
        }
        if (walker->getGender()==0 && walker->getHeight()==0) {
            sumG0h0 = sumG0h0+walker->getExpectCount();
        }
        if (walker->getGender()==1 && walker->getHeight()==0) {
            sumG1h0 = sumG1h0+walker->getExpectCount();
        }
        walker = walker->getNext();
    }
    sumFmale = numOfData-sumMale;
    newMProb = sumMale/numOfData;
    newFProb = 1 - newMProb;
    mProbDiff = abs(newMProb - maleProb);
    fProbDiff = abs(newFProb - fmaleProb);
    
    maleProb = newMProb;
    fmaleProb = newFProb;
    
    newg0w0 = sumG0w0/sumMale;
    newg0w1 = 1-newg0w0;
    newg1w0 = sumG1w0/sumFmale;
    newg1w1 = 1-newg1w0;
    newg0h0 = sumG0h0/sumMale;
    newg0h1 = 1-newg0h0;
    newg1h0 = sumG1h0/sumFmale;
    newg1h1 = 1-newg1h0;
    
    g0w0Diff = abs(newg0w0 - g0w0);
    g0w1Diff = abs(newg0w1 - g0w1);
    g1w0Diff = abs(newg1w0 - g1w0);
    g1w1Diff = abs(newg1w1 - g1w1);
    g0h0Diff = abs(newg0h0 - g0h0);
    g0h1Diff = abs(newg0h1 - g0h1);
    g1h0Diff = abs(newg1h0 - g1h0);
    g1h1Diff = abs(newg1h1 - g1h1);
    
    g0w0 = newg0w0;
    g0w1 = newg0w1;
    g1w0 = newg1w0;
    g1w1 = newg1w1;
    g0h0 = newg0h0;
    g0h1 = newg0h1;
    g1h0 = newg1h0;
    g1h1 = newg1h1;
 
}

// E-step, estimating the probabilities of different values of Gender given that we know a person's weight and height
void estimateGender(){
    
    double newg0w0h0=0.0,newg1w0h0=0.0,newg0w0h1=0.0,newg1w0h1=0.0,newg0w1h0=0.0,newg1w1h0=0.0,newg0w1h1=0.0,newg1w1h1=0.0;
    
    newg0w0h0 = (g0w0*g0h0*maleProb)/(g0w0*g0h0*maleProb+g1w0*g1h0*fmaleProb);
    newg1w0h0 = 1-newg0w0h0;
    
    newg0w0h1 = (g0w0*g0h1*maleProb)/(g0w0*g0h1*maleProb+g1w0*g1h1*fmaleProb);
    newg1w0h1 = 1-newg0w0h1;
    
    newg0w1h0 = (g0w1*g0h0*maleProb)/(g0w1*g0h0*maleProb+g1w1*g1h0*fmaleProb);
    newg1w1h0 = 1-newg0w1h0;
    
    newg0w1h1 = (g0w1*g0h1*maleProb)/(g0w1*g0h1*maleProb+g1w1*g1h1*fmaleProb);
    newg1w1h1 = 1-newg0w1h1;
    
    g0w0h0 = newg0w0h0;
    g1w0h0 = newg1w0h0;
    g0w0h1 = newg0w0h1;
    g1w0h1 = newg1w0h1;
    g0w1h0 = newg0w1h0;
    g1w1h0 = newg1w1h0;
    g0w1h1 = newg0w1h1;
    g1w1h1 = newg1w1h1;
}

// update the expected counts for each data point
void updateExpectedCount(){
    Node* walker = data->get_listHead()->getNext();
    while (walker != NULL) {
        if ( walker->getExpectCount() != 1) {
            
            if (walker->getGender()==0 && walker->getWeight()==0 && walker->getHeight()==0) {
                walker->setExpectCount(g0w0h0);
            }
            if (walker->getGender()==1 && walker->getWeight()==0 && walker->getHeight()==0) {
                walker->setExpectCount(g1w0h0);
            }
            if (walker->getGender()==0 && walker->getWeight()==0 && walker->getHeight()==1) {
                walker->setExpectCount(g0w0h1);
            }
            if (walker->getGender()==1 && walker->getWeight()==0 && walker->getHeight()==1) {
                walker->setExpectCount(g1w0h1);
            }
            if (walker->getGender()==0 && walker->getWeight()==1 && walker->getHeight()==0) {
                walker->setExpectCount(g0w1h0);
            }
            if (walker->getGender()==1 && walker->getWeight()==1 && walker->getHeight()==0) {
                walker->setExpectCount(g1w1h0);
            }
            if (walker->getGender()==0 && walker->getWeight()==1 && walker->getHeight()==1) {
                walker->setExpectCount(g0w1h1);
            }
            if (walker->getGender()==1 && walker->getWeight()==1 && walker->getHeight()==1) {
                walker->setExpectCount(g1w1h1);
            }
        }
        walker = walker->getNext();
    }

}

// compute the log likelihood of data sets
void computeLikelihood(){
    Node* walker = data->get_listHead()->getNext();
    double likelihoodProduct=1.0,newLikelihood=0.0;
    while (walker != NULL) {
        if (walker->getExpectCount()==1) {
            if (walker->getGender()==0 && walker->getWeight()==0 && walker->getHeight()==0) {
                likelihoodProduct = likelihoodProduct*g0w0*g0h0*maleProb;
            }
            if (walker->getGender()==1 && walker->getWeight()==0 && walker->getHeight()==0) {
                likelihoodProduct = likelihoodProduct*g1w0*g1h0*fmaleProb;
            }
            if (walker->getGender()==0 && walker->getWeight()==0 && walker->getHeight()==1) {
                likelihoodProduct = likelihoodProduct*g0w0*g0h1*maleProb;
            }
            if (walker->getGender()==1 && walker->getWeight()==0 && walker->getHeight()==1) {
                likelihoodProduct = likelihoodProduct*g1w0*g1h1*fmaleProb;
            }
            if (walker->getGender()==0 && walker->getWeight()==1 && walker->getHeight()==0) {
                likelihoodProduct = likelihoodProduct*g0w1*g0h0*maleProb;
            }
            if (walker->getGender()==1 && walker->getWeight()==1 && walker->getHeight()==0) {
                likelihoodProduct = likelihoodProduct*g1w1*g1h0*fmaleProb;
            }
            if (walker->getGender()==0 && walker->getWeight()==1 && walker->getHeight()==1) {
                likelihoodProduct = likelihoodProduct*g0w1*g0h1*maleProb;
            }
            if (walker->getGender()==1 && walker->getWeight()==1 && walker->getHeight()==1) {
                 likelihoodProduct = likelihoodProduct*g1w1*g1h1*fmaleProb;
            }

        } else{
            if (walker->getWeight()==0 && walker->getHeight()==0) {
                likelihoodProduct = likelihoodProduct*(g0w0*g0h0*maleProb+g1w0*g1h0*fmaleProb);
            }
            if (walker->getWeight()==0 && walker->getHeight()==1) {
                likelihoodProduct = likelihoodProduct*(g0w0*g0h1*maleProb+g1w0*g1h1*fmaleProb);
            }
            if (walker->getWeight()==1 && walker->getHeight()==0) {
                likelihoodProduct = likelihoodProduct*(g0w1*g0h0*maleProb+g1w1*g1h0*fmaleProb);
            }
            if (walker->getWeight()==1 && walker->getHeight()==1) {
                likelihoodProduct = likelihoodProduct*(g0w1*g0h1*maleProb+g1w1*g1h1*fmaleProb);
            }
        }
        walker = walker->getNext();
    }
    newLikelihood = log(likelihoodProduct);
    likelihoodDiff = abs(newLikelihood-likelihood);
    likelihood = newLikelihood;
    cout<<likelihood<<endl;
}

void emAlgorithm(string inputFile){
    data = new LinkedList();
    likelihood = 0.0;
    int numOfIteration=0;
    for (int i=0; i<numOfData; i++) {
        genderArray[i]=0;
        weightArray[i]=0;
        heightArray[i]=0;
    } //initilize arrays
    ifstream infile;
    infile.open(inputFile);
    char gender;
    int intGender=0,weight=0,height=0,i=0;
    while(!infile.eof()){
        infile >> gender;
        if(gender == '-'){
            intGender = -1; // use -1 to represent missing data
        } else{
            intGender = gender - '0'; // convert char to int
        }
        infile >> weight;
        infile >> height;
        if (!infile) {
            break;
        }
        genderArray[i] = intGender;
        weightArray[i] = weight;
        heightArray[i] = height;
        i++;
    } // store data into 3 1-d arrays
    infile.close();
    estimateGender();
    addExpectCount();
    computeLikelihood();
    computeNewParam();
    numOfIteration++;
    while (!isConverge()) {
        numOfIteration++;
        estimateGender();
        updateExpectedCount();
        computeLikelihood();
        computeNewParam();
    }
    cout<<"The final conditional probability tables for "<<inputFile<<" is: "<<endl;
    cout<<"Gender table"<<endl;
    cout<<"P(gender=M) = "<<maleProb<<" P(gender=F) = "<<fmaleProb<<endl<<endl;
    cout<<"Weight(w) given gender(g) table"<<endl;
    cout<<"P(w=0|g=0) = "<<g0w0<<" P(w=1|g=0) = "<<g0w1<<endl;
    cout<<"P(w=0|g=1) = "<<g1w0<<" P(w=1|g=1) = "<<g1w1<<endl<<endl;
    cout<<"Height(h) given gender(g) table"<<endl;
    cout<<"P(h=0|g=0) = "<<g0h0<<" P(h=1|g=0) = "<<g0h1<<endl;
    cout<<"P(h=0|g=1) = "<<g1h0<<" P(h=1|g=0) = "<<g1h1<<endl<<endl;
}

//use to generate random starting points
void randomStartingPoint(){
    double ranMProb=0.0,ranG0w0=0.0,ranG1w0=0.0,ranG0h0=0.0,ranG1h0=0.0;
    ranMProb = ((rand()%100)*0.01);
    maleProb = ranMProb;
    fmaleProb = 1-maleProb;
    ranG0w0 = ((rand()%100)*0.01);
    ranG1w0 = ((rand()%100)*0.01);
    ranG0h0 = ((rand()%100)*0.01);
    ranG1h0 = ((rand()%100)*0.01);
    g0w0 = ranG0w0;
    g0w1 = 1-g0w0;
    g1w0 = ranG1w0;
    g1w1 = 1-g1w0;
    g0h0 = ranG0h0;
    g0h1 = 1-g0h0;
    g1h0 = ranG1h0;
    g1h1 = 1-g1h0;
    cout<<"The starting points of the learning: "<<endl;
    cout<<"P(gender=M) ="<<maleProb<<endl;
    cout<<"P(w=0|g=0) = "<<g0w0<<endl;
    cout<<"P(w=0|g=1) = "<<g1w0<<endl;
    cout<<"P(h=0|g=0) = "<<g0h0<<endl;
    cout<<"P(h=0|g=1) = "<<g1h0<<endl<<endl;
}

int main(int argc, const char * argv[]) {
    dataset_10 = argv[1];
    dataset_30 = argv[2];
    dataset_50 = argv[3];
    dataset_70 = argv[4];
    dataset_100 = argv[5];
    srand(time(0));
    randomStartingPoint();
    emAlgorithm(dataset_10);
    emAlgorithm(dataset_30);
    emAlgorithm(dataset_50);
    emAlgorithm(dataset_70);
    emAlgorithm(dataset_100);
    delete data;
    return 0;
}
