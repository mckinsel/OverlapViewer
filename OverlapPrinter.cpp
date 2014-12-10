#include "OverlapPrinter.h"
#include <algorithm>
#include <set>

// Amount overhang has to exceed to be noted
// CA uses 15
#define TOLERANCE 15

typedef struct {
  int start; //Start and end wrt the base read, not the partner
  int end;
  bool forward;
  bool left_overhang;
  bool right_overhang;
  std::string partner_name;
} OvlForPrint_T;

bool by_start_then_end(const OvlForPrint_T& a, const OvlForPrint_T& b)
{
  if(a.start == b.start) {
    return a.end < b.end;
  } else {
    return a.start < b.start;
  }
}

void OverlapPrinter_format_overlap(const std::vector<Overlap_T>& ovl_list,
                                   const QnameToOvlsMap_T& qname_to_ovls,
                                   const std::string& query_name,
                                   int width,
                                   std::string* formatted_ovl)
{
  formatted_ovl->clear();

  // First gather the overlaps that we're going to print
  std::vector<int> ovl_indices = qname_to_ovls.at(query_name);
  if(ovl_indices.size() == 0) return;

  std::vector<OvlForPrint_T> ovls_for_printing;

  for(int ovl_i = 0; ovl_i < ovl_indices.size(); ovl_i++) {
    Overlap_T ovl = ovl_list[ovl_indices[ovl_i]];
    OvlForPrint_T ovl_print = {0,0,false,false,false,""};
    
    // Read a is the query
    if(ovl.name_a == query_name) {
      ovl_print.start = ovl.start_a;
      ovl_print.end = ovl.end_a;
      ovl_print.partner_name = ovl.name_b;
      
      // Set the overhangs
      if(ovl.forward) {
       ovl_print.left_overhang = ovl.start_b > TOLERANCE; 
       ovl_print.right_overhang = (ovl.length_b - ovl.end_b) > TOLERANCE; 
      } else {
       ovl_print.right_overhang = ovl.start_b > TOLERANCE; 
       ovl_print.left_overhang = (ovl.length_b - ovl.end_b) > TOLERANCE; 
      }
    }
    // Read b is the query
    else if (ovl.name_b == query_name) {
      ovl_print.start = ovl.start_b;
      ovl_print.end = ovl.end_b;
      ovl_print.partner_name = ovl.name_a;

      // Set the overhangs
      if(ovl.forward) {
       ovl_print.left_overhang = ovl.start_a > TOLERANCE; 
       ovl_print.right_overhang = (ovl.length_a - ovl.end_a) > TOLERANCE; 
      } else {
       ovl_print.right_overhang = ovl.start_a > TOLERANCE; 
       ovl_print.left_overhang = (ovl.length_a - ovl.end_a) > TOLERANCE; 
      }
    }

    ovl_print.forward = ovl.forward;
    ovls_for_printing.push_back(ovl_print);
  }
  
  std::sort(ovls_for_printing.begin(), ovls_for_printing.end(), by_start_then_end);

  int read_length = ovl_list[ovl_indices[0]].name_a == query_name ? 
    ovl_list[ovl_indices[0]].length_a : ovl_list[ovl_indices[0]].length_b;
  float bases_per_char = read_length / (float)width;

  formatted_ovl->append(width, '*');
  formatted_ovl->append("\n");
  formatted_ovl->append(query_name);
  formatted_ovl->append("\n");
  
  std::set<std::string> observed_partners;

  for(int print_i = 0; print_i < ovls_for_printing.size(); print_i++) {
    OvlForPrint_T ovl_print = ovls_for_printing[print_i];
    
    if(observed_partners.find(ovl_print.partner_name) != observed_partners.end()) continue;
    observed_partners.insert(ovl_print.partner_name);

    int pre_chars  = (int)(ovl_print.start / bases_per_char);
    int ovl_chars  = (int)((ovl_print.end - ovl_print.start) / bases_per_char);
    int post_chars = (int)((read_length - ovl_print.end) / bases_per_char);

    char ovl_char = ovl_print.forward ? '>' : '<'; 
    
    // Adjust the lengths to accomodate the overhang symbols 
    if(ovl_print.left_overhang) {
      if(pre_chars) {
        pre_chars--;
      } else if(post_chars) {
        post_chars--;
      } else if(ovl_chars) {
        ovl_chars--;
      }
    }

    if(ovl_print.right_overhang) {
      if(post_chars) {
        post_chars--;
      } else if(pre_chars) {
        pre_chars--;
      } else if(ovl_chars) {
        ovl_chars--;
      }
    }
    // Actually write the characters for the overlap 
    int written_chars = 0;
    formatted_ovl->append(pre_chars, ' ');
    written_chars += pre_chars;
   
    if(ovl_print.left_overhang) {
      formatted_ovl->append("\\");
      written_chars++;
    }

    formatted_ovl->append(ovl_chars, ovl_char);
    written_chars += ovl_chars;

    if(ovl_print.right_overhang) {
      formatted_ovl->append("/");
      written_chars++;
    }

    formatted_ovl->append(post_chars, ' ');
    written_chars += post_chars;
    
    formatted_ovl->append(width - written_chars, ' ');
    formatted_ovl->append(" ");
    formatted_ovl->append(Overlap_name_tail(ovl_print.partner_name));
    formatted_ovl->append("\n");
  }
}
