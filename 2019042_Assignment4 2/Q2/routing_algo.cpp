#include "node.h"
#include <iostream>

using namespace std;

void printRT(vector<RoutingNode*> nd){
/*Print routing table entries*/
  for (int i = 0; i < nd.size(); i++) {
    nd[i]->printTable();
  }
}

void routingAlgo(vector<RoutingNode*> nd){
  //Your code here


  //The loop runs infinitely. Breaks when the tables get converged
  while(1){


    //Flag to check convergence
    int flag = 0;

    //Vector to store tables before sending
    vector<struct routingtbl> prevTables;

    //Storing previous tables
    for(int i=0;i<nd.size();i++){

      struct routingtbl table = nd[i]->getTable();
      prevTables.push_back(table);

    }

    //Sendimg messages from all the nodes
    for(int i=0;i<nd.size();i++){
      nd[i]->sendMsg();
      
    }

     //Flag to check if prev and updated tables of all nodes are same
    int flag2_c = 0;
    //Checking convergence
    for(int i=0;i<nd.size();i++){

      

      //Getting previous and updated tables
      struct routingtbl Prevtable = prevTables[i];
      struct routingtbl Updatedtable = nd[i]->getTable();


      //Getting table sizes
      int PrevtableSize = Prevtable.tbl.size();
      int UpdatedtableSize = Updatedtable.tbl.size();



      //If the table sizes are same
      if(PrevtableSize==UpdatedtableSize){

        //Flag to check if prev and updated tables of one node are same
        int flag3_c = 0;
        
        //Iterating through table entries
        for(int j=0; j<UpdatedtableSize; j++){
          

          //Getting previous and updated routing entries
          struct RoutingEntry prevEntry = Prevtable.tbl[j];
          struct RoutingEntry UpdEntry = Updatedtable.tbl[j];

          //If the routing entry parameters are same
          if(prevEntry.dstip == UpdEntry.dstip && prevEntry.cost == UpdEntry.cost && prevEntry.nexthop == UpdEntry.nexthop && prevEntry.ip_interface == UpdEntry.ip_interface){

            flag3_c++;
            
          }

          
        }

        //If the updated routing table for this node is same as previous
        if(flag3_c==UpdatedtableSize){
          flag2_c++;
        }

      }
      else{
        break;
      }




    }

    //If for all the nodes, previous and updated routing tables are same-

    if(flag2_c == nd.size()){
      flag = 1;
    }

    //Convergence has taken place. Break the loop
    if(flag==1){
      break;
    }


  }


 

  cout<<"Converged - Q1"<<"\n";

  //Changing the entries of B and C

  for(int i=0;i<nd.size();i++){

    if(nd[i]->getName() == "B"){

      nd[i]->updateTblEntry("10.0.1.3");

    }

    else if(nd[i]->getName() == "C"){

      nd[i]->updateTblEntry("10.0.1.23");

    }
  }


  //Setting iteration count
  int itr = 1;
  while(true){

    //Flag to check convergence
    int flag = 0;

    //Vector to store tables before sending
    vector<struct routingtbl> prevTables;

    //Storing previous tables
    for(int i=0;i<nd.size();i++){

      struct routingtbl table = nd[i]->getTable();
      prevTables.push_back(table);

    }

    //Sendimg messages from all the nodes
    for(int i=0;i<nd.size();i++){
      nd[i]->sendMsg();
      
    }

    cout<<"Iteration: "<<itr<<"\n";

    //Printing routing table entries at the and of itperation itr

    printRT(nd);


     //Flag to check if prev and updated tables of all nodes are same
    int flag2_c = 0;
    //Checking convergence
    for(int i=0;i<nd.size();i++){

      

      //Getting previous and updated tables
      struct routingtbl Prevtable = prevTables[i];
      struct routingtbl Updatedtable = nd[i]->getTable();


      //Getting table sizes
      int PrevtableSize = Prevtable.tbl.size();
      int UpdatedtableSize = Updatedtable.tbl.size();



      //If the table sizes are same
      if(PrevtableSize==UpdatedtableSize){

        //Flag to check if prev and updated tables of one node are same
        int flag3_c = 0;
        
        //Iterating through table entries
        for(int j=0; j<UpdatedtableSize; j++){
          

          //Getting previous and updated routing entries
          struct RoutingEntry prevEntry = Prevtable.tbl[j];
          struct RoutingEntry UpdEntry = Updatedtable.tbl[j];

          //If the routing entry parameters are same
          if(prevEntry.dstip == UpdEntry.dstip && prevEntry.cost == UpdEntry.cost && prevEntry.nexthop == UpdEntry.nexthop && prevEntry.ip_interface == UpdEntry.ip_interface){

            flag3_c++;
            
          }

          
        }

        //If the updated routing table for this node is same as previous
        if(flag3_c==UpdatedtableSize){
          flag2_c++;
        }

      }
      else{
        break;
      }




    }

    //If for all the nodes, previous and updated routing tables are same-

    if(flag2_c == nd.size()){
      flag = 1;
    }

    //Convergence has taken place. Break the loop
    if(flag==1){
      break;
    }

 


    int flagU_c = 0;

    //Finding unreachable nodes, if any:
    for(int i=0; i<nd.size();i++){

      int flag4_c = 0;
      struct routingtbl table = nd[i]->getTable();
      for(int j = 0;j<table.tbl.size();j++){
        if(table.tbl[j].cost>=16){
          flag4_c = 1;
          break;
        }
      }

      if(flag4_c ==1){
        flagU_c++;
      }
    }

    if(flagU_c >=3){
      cout<<"Breaking the loop. Unreachable nodes found"<<"\n";
      break;
    }



    itr+=1;



  }

}


void RoutingNode::recvMsg(RouteMsg *msg) {


  //Iterating through the routing table of the sender
  for(int i=0; i<msg->mytbl->tbl.size(); i++){

    struct RoutingEntry *routingEntrySender = &msg->mytbl->tbl[i];

    //Flag to check whether the destination IP exists
    int flag = 0;

    //Iterating through receiver's entries
    for(int j=0; j<mytbl.tbl.size(); j++){

      struct RoutingEntry *MyroutingEntry = &mytbl.tbl[j];

      //Checking if the entry exists
      if(routingEntrySender->dstip == MyroutingEntry->dstip  ){


          //Since it exists, setting the flag to 1
          flag = 1;

          //If the next hop of the entry is mInterface, we'll ignore
          // if(isMyInterface(routingEntrySender.nexthop)){
          //   break;
          // }

          //To check if cost to reach through the sender is lower
          if(MyroutingEntry->cost > routingEntrySender->cost+1 ||  MyroutingEntry->nexthop==msg->from){

            //If Yes, update the parameters of the entry
            MyroutingEntry->cost = min(16,routingEntrySender->cost+1);
            MyroutingEntry->nexthop = msg->from;
            // MyroutingEntry.ip_interface = msg->recvip;
            break;

          }

      }
    }

    if(flag!=1){

      //Creating a new entry in mytbl
      struct RoutingEntry newEntry;
       

      newEntry.dstip = routingEntrySender->dstip;
      newEntry.ip_interface = msg->recvip;
      newEntry.nexthop = msg->from;
       if(msg->from==newEntry.dstip)
      {
        newEntry.cost=1;
      }
      else
      newEntry.cost = routingEntrySender->cost+1;
      mytbl.tbl.push_back(newEntry);


    }


  }



}


