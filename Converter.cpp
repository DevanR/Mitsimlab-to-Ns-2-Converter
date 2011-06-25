//-*-c++-*------------------------------------------------------------
// Converter.cpp
//
// Created By : Devan Bing
// Copyright (C) 2007
// Nanyang Technological University
// All Rights Reserved
//
//This program reads the trajectory.out file and the network database file and 
//uses the data to create the corresponding scene file, that can be run on NS2.
//--------------------------------------------------------------------
 
 #include <iostream>
  using std::cerr;
  using std::cout;
  using std::endl;
  using std::fixed;
  using std::ios;
  using std::left;
  using std::right;
  using std::showpoint;

  #include <fstream> // file stream        
  using std::ifstream; // input file stream

  #include <iomanip>
  using std::setw;
  using std::setprecision;

  #include <string>
  using std::string;

  #include <cstdlib>
  using std::exit; // exit function prototype

  #include "WcsPoint.h"
  #include "RN_Arc.h"

  using namespace std;

  struct Trajectory {
  int segment;
  int lane;
  float position;
  float speed; 
  float acceleration;
  WcsPoint A;
  };

 // A structure used to store vehicle trajectories
 struct Vehicle {
 Trajectory traj[800]; // set value to equal number of samples/vehicle in simulation
 int vehicle_type;
 int start_time;
 int end_time;
 bool status; //false = ended simulation run
 } car[3000]; // set value to equal numbers of vehicles +1

 // A structure used to store segment characteristics
 struct Segment {
 WcsPoint startpnt;
 WcsPoint endpnt;
 double bulge; 
 
 };

 void outputLine( int,int,int,int,double,double,long double,int ); // prototype

int main()
{
    const int max_num_of_char_in_a_line = 512;
    
    double map_x_shift = 0;
    double map_y_shift = 0;
    
    // attributes for tractory.out
    int MAX_CARS = 3000; // set to equal number of nodes in simulation +1...exclude ID = 0
    int counter = 0; // for time
    int simulation_start = 0;
    int simulation_end = 0;
    RN_Arc Arc; // to calculate x/y positions
    
    // attributes for network.dat
    string link("[Links]");
    string test;
    char read_char;
    int num_segments = 0;
    int num_links = 0;
    int read_segment_id = 0;
    double start_x_coord = 0;
    double start_y_coord = 0;
    double end_x_coord = 0;
    double end_y_coord = 0;
    double segment_bulge = 0;
    int segment_id = 0;
    
    // for display purposes
    int time;
    int ID;
    int segment;
    int lane;
    double position;
    double speed; 
    long double acceleration;
    int vehicle_type;
    
    //testing purposes
    WcsPoint P;
    
    
    ///////////////////////////////////////////////////////////////////////////
    // code for reading network file
    //////////////////////////////////////////////////////////////////////////
    
     // error checking
     ifstream network_file("test.dat", ios::in );

     // exit program if ifstream could not open file
     if ( !network_file )
     {
       cerr << "Network File could not be opened" << endl;
       exit( 1 );
     } // end if

     //scans to [Links]
     while ( network_file >> test && test != link ) { // keep reading until end-of-file
     }
      
      // skip number of link to number of segments
      network_file >> read_char;
      network_file >> num_links;
      network_file >> read_char;
      network_file >> num_segments;
      
      Segment Segment_array[num_segments];
      
      // Skip explaination for Link : Segments : Lanes
      int netw_header_lines = 12; 
      for(int j = 0; j < netw_header_lines; ++j)
      network_file.ignore(max_num_of_char_in_a_line, '\n');
      
      int brackets = 0;
      int prev_brackets = 0;
      bool multiple_segments = false;
      
      // Begin reading segment data, loop style based on no.of segments
      for ( int y = 0; y < num_segments; y++)
      {
          
      if(multiple_segments == false)
      {         
             
      while( brackets < 3)
      { 
          network_file >> read_char;
          if ( read_char == '{' )
          {
           prev_brackets = brackets;
           brackets = brackets + 1;
          }
          
          if( brackets == 2 && prev_brackets == 1 )
           {
            network_file >> segment_id;
            network_file.ignore(max_num_of_char_in_a_line, '\n');
           }

      }
      
      }//multiple_segment == false
      
      if(multiple_segments == true)
      {
           
       network_file >> segment_id;
       network_file.ignore(max_num_of_char_in_a_line, '\n');
       
       network_file >> read_char;
       if ( read_char == '{' )
          {
           prev_brackets = brackets;
           brackets = brackets + 1;
          }
       multiple_segments = false;
      }                   
      
      network_file >> start_x_coord;
      network_file >> start_y_coord;
      network_file >> segment_bulge;
      network_file >> end_x_coord;
      network_file >> end_y_coord;

      Segment_array[segment_id].startpnt.set(start_x_coord*0.3048,start_y_coord*0.3048);
      Segment_array[segment_id].endpnt.set(end_x_coord*0.3048,end_y_coord*0.3048);
      Segment_array[segment_id].bulge = segment_bulge;
      
      // code for getting values to correct network map for NS2 usage
      // searches for the smallest x and y coordinates used
      if ( start_x_coord < map_x_shift )
        map_x_shift = start_x_coord;
        
      if ( end_x_coord < map_x_shift )
        map_x_shift = end_x_coord;  
        
      if ( start_y_coord < map_y_shift )
        map_y_shift = start_y_coord;
        
      if ( end_y_coord < map_y_shift )
        map_y_shift = end_y_coord;  
      
      while( brackets != 0 )
      {
        network_file >> read_char;
             
        if ( read_char == '}' )
        {
         prev_brackets = brackets;    
         brackets = brackets - 1;
        }
        if ( read_char == '{' )
        {
         prev_brackets = brackets;    
         brackets = brackets + 1;
        }
        
        if(brackets == 2 && prev_brackets == 1)
          multiple_segments = true;
          
        if(multiple_segments == true)
         break;
         
        if ( y == num_segments - 1 )
         break;

      }
      
      }
      
      /////////////////////////////////////////////////////////////////////////
      // code for reading trajectory file
      ////////////////////////////////////////////////////////////////////////
      
      //initialize Vehicle Array
    for( int x = 0;x < 800; x++) // set end value of x = to number of seconds in simlation
    {
         for (int y = 0; y< MAX_CARS;y++)
         {
             car[y].vehicle_type = 0;
             car[y].start_time = -1;
             car[y].end_time = -1;
             car[y].traj[x].segment = 0;
             car[y].traj[x].lane = 0;
             car[y].traj[x].position = 0;
             car[y].traj[x].speed = 0;
             car[y].traj[x].acceleration = 0;
             car[y].traj[x].A;
             car[y].status = true; //still in simulation
             }
    }
      
      // error checking
     ifstream traj_file("trajectory.out", ios::in );

     // exit program if ifstream could not open file
     if ( !traj_file )
     {
       cerr << "Trajectory File could not be opened" << endl;
       exit( 1 );
     } // end if
      
      // Skip header files in trajectory.out
      int traj_header_lines = 2; 
      for(int j = 0; j < traj_header_lines; ++j)
      traj_file.ignore(max_num_of_char_in_a_line, '\n');

     /*
     // Print out Headings
     cout << left << setw( 5 )<< "Time " << setw( 3 )<< "ID " << setw( 8 )<<"Segment " 
     << setw( 5 )<< "Lane " << setw( 14 ) << "Position " << setw( 13 ) << "Speed "  << setw( 13 )
     << "Acceleration " << setw( 2 )<< "Vehicle_Type " << endl << fixed << showpoint;
     */
     
     while ( traj_file >> time >> ID >> segment >> lane >> position >> speed >> acceleration >> vehicle_type  )
     {     
           //outputLine( time,ID,segment,lane,position,speed,acceleration,vehicle_type );
           
           if(counter == 0)
            simulation_start = time;
           
           if(ID>0)// All Bus IDs are negative, cars are positive
           {
           // what happens when node IDs are not in seq order in traj.out!!!!!
           car[ID].traj[time - simulation_start].segment = segment;
           car[ID].traj[time - simulation_start].lane = lane;
           car[ID].traj[time - simulation_start].position = position;
           car[ID].traj[time - simulation_start].speed = speed;
           car[ID].traj[time - simulation_start].acceleration = acceleration;
           car[ID].vehicle_type = vehicle_type;
           
           // Initialse A to Specific segment attributes
           Arc.initArc( Segment_array[segment].startpnt.x(),Segment_array[segment].startpnt.y(),
                      Segment_array[segment].bulge,
                      Segment_array[segment].endpnt.x(),Segment_array[segment].endpnt.y() );         
                       
           Arc.computeArc();
   
           position = Arc.getPositionRatio(position);
           
           car[ID].traj[time - simulation_start].A = Arc.intermediatePoint(position);
           }

           counter++;
           
           simulation_end = time - simulation_start;
     }
     
     //////////////////////////////////////////////////////////////////////////
     // code for creating NS2 scene file
     //////////////////////////////////////////////////////////////////////////
     
     float shift_map = 0;
     float start_x = 0;
     float start_y = 0;
     int start_time = 0;
     int end_time = 0;
     
     // map correction for MITSIMLab maps to NS2
     // shifts entire map by the smallest values choosing between x and y
     if( map_x_shift < map_y_shift)
      shift_map = - map_x_shift;
     else
      shift_map = - map_y_shift;
     
     ofstream scene_write("scene.txt");
     
     scene_write << "# Scene File for NS2 " << endl;
     
     scene_write << "set god_ [God instance] " << endl;
     
     //scene_write <<setw( 5 ) << "Time" << setw( 5 ) << "Node" 
     //<< setw( 10 ) << "X" << setw( 10 ) << "Y" << endl;
     
     for ( int i = 1; i< MAX_CARS; i++)
     {   
           // searching for the first occurrence of node
           for ( int h = 0; h< simulation_end+1; h++)
           {
               // reset start_time value
               start_time = -1;
               
               if ( car[i].traj[h].A.x() != 0 )
                {
                 start_time = h;
                 h = simulation_end+1;
                 //cout<<"node = "<<i<<"start = " <<start_time<<endl;
                }
           }
           
           // first appearence of node
           car[i].start_time = start_time;
           
           
           // searching for the last occurrence of node
           for ( int h = car[i].start_time+1; h <= simulation_end+1; h++)
           {
                // reset end_time value
               end_time = -1;
               
               if ( car[i].traj[h].A.x() == 0 && car[i].start_time != -1)
                {
                 end_time = h;
                 h = simulation_end+1;
                //cout<<"node = "<<i<<"end = " <<end_time<<endl;
                }
           }
           
            // last appearence of node
           car[i].end_time = end_time;
          
            
       if ( car[i].traj[0].A.x() == 0 )
        { 
           start_x = 5000;
           start_y = 5000;       
        }
        
       else
        {
         start_x = car[i].traj[0].A.x()+ shift_map;
         start_y = car[i].traj[0].A.y()+ shift_map;
        }
           
        // uses start_time to eliminate unused nodes
        if( car[i].start_time != -1)
        {
        //we use i-1 to because ns2 assumes all node IDs begin with node0 not node1
        scene_write << "$node_(" << i-1 << ") set X_ " << start_x << endl;
        scene_write << "$node_(" << i-1 << ") set Y_ " << start_y << endl;
        scene_write << "$node_(" << i-1 << ") set Z_ " << 0 << endl;
        }
     }
      
      
      
      for ( int h = 1; h< simulation_end+2; h++)
     //for ( int h = simulation_end+2; h> 1; h--)
     {
         for ( int i = 1; i< MAX_CARS; i++)
         {   
             // add node to scene with original speed
             if( h == car[i].start_time)
              {
               scene_write << "$ns_ at " << h-1 << " \"$node_(" << i-1 << ") setdest " 
               << car[i].traj[h].A.x()+ shift_map << " " << car[i].traj[h].A.y()+ shift_map << " "
               << 100000 <<"\""<< endl;
              }
             
             
             //discrete movement
             if( h > car[i].start_time && h < car[i].end_time)
             { 
              scene_write << "$ns_ at " << h-1 << " \"$node_(" << i-1 << ") setdest " 
              << car[i].traj[h].A.x()+ shift_map << " " << car[i].traj[h].A.y()+ shift_map << " "
              << 100000 <<"\""<< endl;
             }
             
             
             /*
             //continuos movement, does not work for all situations!
             if( h > car[i].start_time && h < car[i].end_time)
             { 
              scene_write << "$ns_ at " << h-1 << " \"$node_(" << i-1 << ") setdest " 
              << car[i].traj[h].A.x()+ shift_map << " " << car[i].traj[h].A.y()+ shift_map << " "
              << car[i].traj[h].speed <<"\""<< endl;
             }
             */
             
             // remove node from scene
             if( h == car[i].end_time )
              {
               scene_write << "$ns_ at " << h-1 << " \"$node_(" << i-1 << ") setdest " 
               << 5000 << " " << 5000 << " "
               << 100000 <<"\""<< endl;
               
               car[i].status = false;
              }
         }
     }
     
     //remove for optimization with calcdest
     // allow for smoother Nam
         for ( int k = simulation_end+2; k< simulation_end+4; k++)
         {
            for ( int j = 1; j< MAX_CARS; j++)
            {     
                  if( car[j].status == false)
                  {
                    scene_write << "$ns_ at " << k-1 << " \"$node_(" << j-1 << ") setdest " 
                    << 5000 << " " << 5000 << " "
                    << 100000 <<"\""<< endl;
                  }
            }   
        }   
     
     scene_write << "#  End of NS2 Scene File " << endl;
 
     //////////////////////////////////////////////////////////////////////////
     // code for creating X/Y Coordinate file
     //////////////////////////////////////////////////////////////////////////
     
     ofstream coord_write("coord.txt");
     
     for ( int h = 1; h< simulation_end+2; h++)
     {
         for ( int i = 1; i< MAX_CARS; i++)
         {   
             if( h > car[i].start_time && h < car[i].end_time)
             { 
              coord_write << car[i].traj[h].A.x()+ shift_map << "," 
              << car[i].traj[h].A.y()+ shift_map << endl;
             }
         }
     }
     
     /*
     ///////////////////////////////////////////////////////////////////////////
     // code for testing reading of trajectory.out
     //////////////////////////////////////////////////////////////////////////
     
     cout << left << setw( 5 )<< "Time " << setw( 3 )<< "ID " << setw( 8 )<<"Segment " 
     << setw( 5 )<< "Lane " << setw( 14 ) << "Position "<< endl << fixed << showpoint;

     // display array
     for ( int i = 1; i< MAX_CARS; i++)
     {
         for ( int h = 0; h< simulation_end +1; h++)
         {
             
               cout << setw( 5 ) << h << setw( 3 )<< i << setw( 8 )<< car[i].traj[h].segment
               << setw( 5 )<< car[i].traj[h].lane << setw( 14 ) << car[i].traj[h].position
               << endl << fixed << showpoint;
             
         }
     }
     
     ///////////////////////////////////////////////////////////////////////////
     */
     
     
     ///////////////////////////////////////////////////////////////////////////
     // code for testing reading of network file
     ///////////////////////////////////////////////////////////////////////////
     
     ofstream map_write("map.txt");
     
     // display array
     for ( int i = 0; i< num_segments; i++)
     {
         //cout<< "Segment ID = "<< i<< endl;
         //cout<< "Start x = "<< Segment_array[i].startpnt.x()<<endl;
         //cout<< "Start y = "<< Segment_array[i].startpnt.y()<<endl;
         //cout<< "End x = "<< Segment_array[i].endpnt.x()<<endl;
         //cout<< "End y = "<< Segment_array[i].endpnt.y()<<endl;
         //cout<< "Bulge = "<< Segment_array[i].bulge<<endl;
         //cout<< endl;
         
         map_write << Segment_array[i].startpnt.x()+ shift_map << "," <<
         Segment_array[i].startpnt.y()+ shift_map << endl;
         
         map_write << Segment_array[i].endpnt.x()+ shift_map << "," <<
         Segment_array[i].endpnt.y()+ shift_map << endl;
         
         //map_write<< "Segment ID = "<< i<< endl;
         //map_write<< "Start x = "<< Segment_array[i].startpnt.x()<<endl;
         //map_write<< "Start y = "<< Segment_array[i].startpnt.y()<<endl;
         //map_write<< "End x = "<< Segment_array[i].endpnt.x()<<endl;
         //map_write<< "End y = "<< Segment_array[i].endpnt.y()<<endl;
         //map_write<< "Bulge = "<< Segment_array[i].bulge<<endl;
         //map_write<< endl;
         
     }
     
     //////////////////////////////////////////////////////////////////////////
     
     
    cout << "Press ENTER to continue..." << endl; 
    cin.get();
    return 0;

}

/*
// display single record from file
void outputLine( int time,int ID,int segment,int lane,double position,double speed,long double acceleration,int vehicle_type )
{    
     cout << setw( 5 )<< time<< setw( 3 ) << ID << setw( 8 )<< segment  << setw( 5 )
     << lane << setw( 14 ) << position << setw( 13 ) << speed << setw( 13 )<< acceleration << setw( 2 )
     << vehicle_type << endl;
     
} // end function outputLine
*/
