#include <iostream>

int main(){
  std::string inputString;
  int op;

  std::cout << "Operations:\n";
  std::cout << "1. Add point\n";
  std::cout << "2. Remove point\n";
  std::cout << "3. Display points\n\n";

  std::cout << "Enter operation number: ";  
  std::getline(std::cin, inputString);
  op = std::stoi(inputString);


  while(op < 1 || op > 3){
    std::cout << "Invalid operation number.\n";
    std::cout << "Please enter a valid operation number (1, 2, or 3): ";
    std::getline(std::cin, inputString);
    op = std::stoi(inputString);
  }
  switch(op){
    case 1:
      std::cout << "Add point operation selected.\n\n";

      break;
    case 2:
      std::cout << "Remove point operation selected.\n\n";
      // Remove point logic here
      break;
    case 3:
      std::cout << "Display points operation selected.\n\n";
      // Display points logic here
      break;
    default: 
      break;
  }


  return 0;
}