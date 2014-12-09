#ifndef _OVERLAPPRINTER_H_
#define _OVERLAPPRINTER_H_

#include <string>
#include <vector>

#include "Overlap.h"

void OverlapPrinter_format_overlap(const std::vector<Overlap_T>& ovl_list,
                                   const QnameToOvlsMap_T& qname_to_ovls,
                                   const std::string& query_name,
                                   int width,
                                   std::string* formatted_ovl);

#endif
