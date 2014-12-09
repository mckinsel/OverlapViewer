#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include "Overlap.h"
#include "CAReader.h"
#include "LASReader.h"
#include "OverlapPrinter.h"

int main(int argc, char* argv[])
{
  
  std::string gkp_store_name, fastq_filename, ovl_store_name;
  std::string las_name, db_name;
  int width = 80;

  int arg = 1;
  while(arg < argc) {
    if(strcmp(argv[arg], "-G") == 0) {
      gkp_store_name = std::string(argv[++arg]);
    }
    if(strcmp(argv[arg], "-F") == 0) {
      fastq_filename = std::string(argv[++arg]);
    }
    if(strcmp(argv[arg], "-O") == 0) {
      ovl_store_name = std::string(argv[++arg]);
    }
    if(strcmp(argv[arg], "-L") == 0) {
      las_name = std::string(argv[++arg]);
    }
    if(strcmp(argv[arg], "-D") == 0) {
      db_name = std::string(argv[++arg]);
    }
    if(strcmp(argv[arg], "-W") == 0) {
      width = atoi(argv[++arg]);
    }
    arg++;
  }
  
  bool celera_options = gkp_store_name.size() > 0 && fastq_filename.size() > 0 && ovl_store_name.size() > 0;
  bool dalign_options = las_name.size() > 0 && db_name.size() > 0;
  
  if(celera_options == dalign_options) {
    std::cout << "Use either -G/-F/-O for Celera or -L/-D for Dalign." << std::endl;
    exit(0);
  }

  std::vector<Overlap_T> ovl_list;
  if(celera_options) {
    CAReader_create_ovl_list(fastq_filename, gkp_store_name, ovl_store_name, &ovl_list);
  } else {
    LASReader_create_ovl_list(las_name, db_name, &ovl_list);
  }

  QnameToOvlsMap_T qname_to_ovls;
  Overlap_create_map(ovl_list, &qname_to_ovls);
  
  QnameToOvlsMap_T::iterator qname_it;
  for(qname_it = qname_to_ovls.begin(); qname_it != qname_to_ovls.end(); qname_it++) {
    std::string formatted_ovl;
    OverlapPrinter_format_overlap(ovl_list, qname_to_ovls, qname_it->first,
                                  width, &formatted_ovl);

    std::cout << formatted_ovl << std::endl;
  }

  return 0;
}
