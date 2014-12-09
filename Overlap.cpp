#include "Overlap.h"

void Overlap_create_map(const std::vector<Overlap_T>& overlaps,
                        QnameToOvlsMap_T* qname_to_ovls)
{
  for(int i=0; i<overlaps.size(); i++) {
    Overlap_T ovl = overlaps[i];
    (*qname_to_ovls)[ovl.name_a].push_back(i); 
    (*qname_to_ovls)[ovl.name_b].push_back(i); 
  }
}

std::string Overlap_name_tail(const std::string& qname)
{
  size_t slash_pos = qname.find_first_of('/');
  std::string tail = qname.substr(slash_pos + 1, qname.size() - slash_pos - 1);
  return tail;
}
