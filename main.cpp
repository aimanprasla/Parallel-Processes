#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
using namespace std;


int main(int argc, char* argv[]) {

  //Open the files
  ifstream finInput(argv[1]);
  ifstream finOperation(argv[2]);


  //tools to read and store input
  string line;
  map <char, int> Map;


  //grabs variable line, 1st line, from the file and reads variables
  getline(finOperation, line);
  line.erase(remove(line.begin(), line.end(), ' '), line.end());
  line.erase(remove(line.begin(), line.end(), ','), line.end());
  line = line.substr(9, line.length());
  string tempVariableString;
  int index = 0;
  while(getline(finInput, tempVariableString, ','))
    {
      tempVariableString.erase(remove(tempVariableString.begin(), tempVariableString.end(), ' '), tempVariableString.end());
      int tempVariable = stoi(tempVariableString);
      char letter = line[index];
      Map[letter] = tempVariable;
      index++;
    }
  //for (auto i = Map.begin(); i != Map.end(); i++) 
    //cout << i->first << "\t" << i->second << endl; 

  //grabs processes line, 2nd line, from the file and reads processes
  getline(finOperation, line);
  line.erase(remove(line.begin(), line.end(), ' '), line.end());
  line.erase(remove(line.begin(), line.end(), ','), line.end());
  line.erase(remove(line.begin(), line.end(), ';'), line.end());
  line = line.substr(12, line.length());
  int numProcesses;
  if(line[line.length() - 2] == 'p')
    numProcesses = line[line.length() - 1] - 48 + 1;
  else
    numProcesses = line[line.length() - 2] - 48 + 1;
  //cout << line[line.length() - 2] << " " << numProcesses << endl;

  //Creates the pipes
  int pipes[numProcesses][2];
  for(int a = 0; a < numProcesses; a++)
    {
      if(pipe(pipes[a]) == -1)
        printf("Error creating pipe \n");
    }

  //Creates forks
  int pids [numProcesses];
  for(int a = 0; a < numProcesses; a++)
    {
      pids[a] = fork();
      if(pids[a] == -1)
      {
        printf("Error creating forks \n");
        return 1;
      }
      //Child process doing individual tasks
      if(pids[a] == 0)
      {
        int alreadyReadProcesses[numProcesses]; //If process values already read once, grabs value from here
        for(int a = 0; a < numProcesses; a++)
          {
            alreadyReadProcesses[a] = 0;
          }
        string line;
        int totalForProcess = 0;
        //Grabs the lines for the processes
        while(getline(finOperation, line))
          {
            //Writes final result to the pipe when broken
            if(finOperation.peek() == EOF)
            {
              cout << "Final total for process " << a << " is: " << totalForProcess << endl;
              if(write(pipes[a][1], &totalForProcess, sizeof(int)) == -1)
                printf("Error writing final total for process \n");
              break;
            }
            line.erase(remove(line.begin(), line.end(), ' '), line.end());
            line.erase(remove(line.begin(), line.end(), ';'), line.end());
            int toProcess = line[line.length()-2] - 48;
            if(line[line.length() - 2] == 'p')
              toProcess = line[line.length() - 1] - 48;
            else
              toProcess = line[line.length() - 2] - 48;
            
            //Does tasks if related to itself, else does nothing
            if(toProcess == a)
            {
              //cout << " I am process: " << a << " " << "and I caught " << line << " " << endl;
              if(Map.count(line[0]) == 1) //Check for variable insert into process
              {
                //cout << "in regular transfer" << endl;
                totalForProcess += Map.at(line[0]);
                //cout << "Total for process " << a << " is: " << totalForProcess << endl;
              }
              else if(line[0] == 'p') //Check for process insert into another process
              {
                //cout << "in process transfer" << endl;
                int temp;
                int fromProcess = line[1] - 48;
                if(alreadyReadProcesses[fromProcess] == 0)
                  {
                    if(read(pipes[fromProcess][0], &temp, sizeof(int)) == -1)
                      printf("Error reading during checking non arithmetic process to process transfer \n");
                    alreadyReadProcesses[fromProcess] = temp;
                  }
                  else
                    temp = alreadyReadProcesses[fromProcess];
                totalForProcess += temp;
                //cout << "Process: " << a << " " << "grabbed " << temp << " from " << fromProcess <<  endl;
                //cout << "Total for process " << a << " is: " << totalForProcess << endl;
              }
              else if(line[0] == '+' || line[0] == '-' || line[0] == '*' || line[0] == '/') //Check for arithmetic 
              {
                //cout << "in arithmetic" << endl;
                if(line[1] == 'p') //Check if its arithmetic process insert to another process
                {
                  //cout << "in process arithmetic" << endl;
                  int temp;
                  int fromProcess = line[2] - 48;
                  if(alreadyReadProcesses[fromProcess] == 0)
                    {
                      if(read(pipes[fromProcess][0], &temp, sizeof(int)) == -1)
                        printf("Error reading during checking non arithmetic process to process transfer \n");
                      alreadyReadProcesses[fromProcess] = temp;
                    }
                    else
                      temp = alreadyReadProcesses[fromProcess];
                  //cout << "Process: " << a << " " << "grabbed " << temp << " from " << fromProcess <<  endl;
                  if(line[0] == '+')
                    totalForProcess = totalForProcess + temp;
                  else if(line[0] == '-')
                    totalForProcess = totalForProcess - temp;
                  else if(line[0] == '*')
                    totalForProcess = totalForProcess * temp;
                  else if(line[0] == '/')
                    totalForProcess = totalForProcess / temp;
                  //cout << "Total for process " << a << " is: " << totalForProcess << endl;
                }
                else if(Map.count(line[1]) == 1) //Check if its variable arithmetic insert into process
                {
                  int value = Map.at(line[1]);
                  //cout << "in the regular arithmetic" << endl;
                  if(line[0] == '+')
                    totalForProcess = totalForProcess + value;
                  else if(line[0] == '-')
                    totalForProcess = totalForProcess - value;
                  else if(line[0] == '*')
                    totalForProcess = totalForProcess * value;
                  else if(line[0] == '/')
                    totalForProcess = totalForProcess / value;
                  //cout << "Total for process " << a << " is: " << totalForProcess << endl;
                }
              }
            }
          }
      }
      sleep(3); //Keeps already finished processes running so other processes can access values
      kill(pids[a], SIGTERM); //Kills the process, since in a loop every process ends itself
    }
  
  //Closes the files
  finInput.close();
  finOperation.close();
  
  return 0;
}