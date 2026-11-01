#include "TMS.h"
#include <iostream>
int main(int argc,char**argv){
 try {
  std::string configPath=argc>1?argv[1]:"simulation.conf";
  tms::Config config=tms::Config::load(configPath);
  tms::SimulationEngine engine(std::move(config));
  std::cout<<"Traffic Management System | C++17\nPress Ctrl+C to request OS termination; normal runs stop at configured ticks.\n";
  engine.run(); return 0;
 } catch(const std::exception&e){std::cerr<<"Configuration/startup error: "<<e.what()<<"\n";return 1;}
}
