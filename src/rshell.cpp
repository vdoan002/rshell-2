// ZIYA EMANET
// rshell: a basic shell

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <string>
#include <deque>

using namespace std;

string strtokPrep(string);

int main(){

    char hostName[65];
    char userName[33];
    int eUser = getlogin_r(userName,sizeof(userName));
    int eHost = gethostname(hostName,sizeof(hostName));
    
    if(eHost == -1){
        perror("gethostname");
    }
    if(eUser != 0){
        perror("getlogin_r");
    }

    bool runMain = true;
    
    while(runMain){
        string usrInput = "";

        cout << userName  << "@" << hostName << "$ ";
        getline(cin,usrInput);
  
        if(usrInput == "exit"){
           runMain = false;
           break;
        }

        usrInput = strtokPrep(usrInput);
    
        char* cInput = new char[usrInput.length()+1];
        strcpy(cInput,usrInput.c_str());
        char* grab = strtok(cInput," ");
    
        deque<string> inputQ;
    
        while(grab != NULL){
            inputQ.push_back(string(grab));
            grab = strtok(NULL," ");
        }
 
        int numTerms = 0;
        string chkInp;
        int delEnd = 0;
        deque<string>::iterator itr = inputQ.begin();
        bool cont = true;

        while(cont){
            bool ampCon = false;    
            bool pipeCon = false;
            numTerms++;
            chkInp = *itr;
            bool conChk;
            bool run = false;

            if(chkInp == "exit"){
                runMain = false;
                break;
            }

            if(chkInp == ";" || chkInp == "||" || chkInp == "&&"){
                conChk = true;
                if(chkInp == "&&"){
                    ampCon = true;
                
                }
                else if(chkInp == "||"){
                    pipeCon = true;
                }
            }
            else{
                conChk = false;
            }
            
            int inputQsize = inputQ.size();
            if(numTerms == inputQsize || conChk){
                char* argv[999999];
            
                if(numTerms == inputQsize){
               
                    if(conChk && chkInp != ";"){
                        cout << "*ERROR last command not run, check connector useage*" << endl;
                        break;
                    }
                    cont =  false;
                }

                if(conChk){
                    numTerms--;
                    delEnd = 1;
                }
            
                for(int i = 0;i < numTerms; i++){
                    argv[i] = new char[inputQ[i].size()+1];
                    strcpy(argv[i],inputQ[i].c_str());
                }
            
                argv[numTerms] = NULL;
          
                for(int j = 0; j < numTerms + delEnd; j++){
                    inputQ.pop_front();
                }

                int pid = fork();
	
	            if(pid == -1){
		            perror("fork");
	            }
	            else if(pid == 0){
                    execvp(argv[0],argv);
		            perror("execvp");
	            }
	            else{
                    int status;
	    	        int eWait = wait(&status);
		            
                    if(eWait == -1){
			            perror("wait");
		            }   
	
                    if(WIFEXITED(status)){
                        if(WEXITSTATUS(status) == 0){
                            if(pipeCon){
                                cont =  false;
                                break;
                            }
                        }
                        else{
                            if(ampCon){
                                cont = false;
                                break;
                            }
                        }
                    }
	            }
            
                itr = inputQ.begin();
                run = true;
                delEnd = 0;
                numTerms = 0;

            }

            if(!run){
                itr++;
            }
        }
    }

    return 0;
}

string strtokPrep(string inString){
    string currChar;
    inString += " ";

    int strLen = inString.length();
    for(int i = 0; i < strLen;i++){
        currChar = inString[i];
    
        if(currChar.compare(";") == 0){
            inString.insert(i," ");
            inString.insert(i+2," ");
            i += 2;
        }

        if(currChar.compare("|") == 0){
           int chkLen = inString.length();
           if((i+2) <= chkLen){
                string afterChar; 
                afterChar = inString[i+1];
                if(afterChar.compare("|") == 0){
                    inString.insert(i," ");
                    inString.insert(i+3," ");
                    i += 2;
                }
                else{
                    cout << "*ERROR using connector ||*" << endl;
                }
           }
           else{
               cout << "*ERROR using connector ||*" << endl;
           }
        }
        
        if(currChar.compare("&") == 0){
           int chkLen = inString.length();
           if((i+2) <= chkLen){
                string afterChar; 
                afterChar = inString[i+1];
                if(afterChar.compare("&") == 0){
                    inString.insert(i," ");
                    inString.insert(i+3," ");
                    i += 2;
                }
                else{
                    cout << "*ERROR using connector &&*" << endl;
                }
           }
           else{
               cout << "*ERROR using connector &&*" << endl;
           }
        
        }

        if(currChar.compare("#") == 0){
            inString.erase(i,-1);
            break;
        }
        strLen = inString.length();
    }
    return inString;
}

