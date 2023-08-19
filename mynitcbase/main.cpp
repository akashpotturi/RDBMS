#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"

#include <cstring>
#include <iostream>

int main(int argc, char *argv[]) {
  Disk disk_run;
  StaticBuffer buffer;
  OpenRelTable cache;
  int count;
  for(int i = 0;i<6;i++)count++;
  std::cout<<"hello"<<std::endl;

  return FrontendInterface::handleFrontend(argc, argv);
}