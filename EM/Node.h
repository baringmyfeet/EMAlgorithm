//
//  Node.h
//  EM
//
//  Created by Jinwei Lin on 10/18/15.
//  Copyright (c) 2015 JL. All rights reserved.
//

#include <iostream>
using namespace std;

class Node{
    
private:
    int gender;
    int weight;
    int height;
    double expectCount;
    Node* next;
    
public:
    Node(){
        gender=0;
        weight=0;
        height=0;
        expectCount=0.0;
        next = NULL;
    }
    
    Node(int g, int w, int h, double ex){
        gender = g;
        weight = w;
        height = h;
        expectCount = ex;
        next = NULL;
    }
    
    void setGender(int g){
        gender = g;
    }
    
    int getGender(){
        return gender;
    }
    
    void setWeight(int w){
        weight = w;
    }
    
    int getWeight(){
        return weight;
    }

    void setHeight(int h){
        height = h;
    }
    
    int getHeight(){
        return height;
    }

    void setExpectCount(double c){
        expectCount = c;
    }
    
    double getExpectCount(){
        return expectCount;
    }
    
    void setNext(Node* n){
        next = n;
    }
    
    Node* getNext(){
        return next;
    }
};