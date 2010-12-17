//  Copyright (c) 2005-2007 Andre Merzky (andre@merzky.net)
//  Copyright (c) 2008, 2009 João Abecasis
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// System Header Files
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <list>
#include <vector>
#include <iostream>
#include <fstream>

#include "ini.hpp"

#include <saga/saga.hpp>
#include <saga/saga/adaptors/utils.hpp>

///////////////////////////////////////////////////////////////////////////////
// example ini line: line # comment
const char pattern_comment[] =  "^([^#]*)(#.*)$";

// example uses ini line: [sec.ssec]
const char pattern_section[] = "^\\[([^\\]]+)\\]$";

// example uses ini line: key = val
const char pattern_entry[] = "^([^\\s=]+)\\s*=\\s*(.*[^\\s])?\\s*$";

///////////////////////////////////////////////////////////////////////////////

namespace
{
  /////////////////////////////////////////////////////////////////////////////
  inline std::string
  trim_whitespace (std::string const &s)
  {
    typedef std::string::size_type size_type;

    size_type first = s.find_first_not_of (" \t\r\n");

    if ( std::string::npos == first )
      return (std::string ());

    size_type last = s.find_last_not_of (" \t\r\n");
    return s.substr (first, last - first + 1);
  }

} // namespace
///////////////////////////////////////////////////////////////////////////////

mb_util::impl::ini::section::section (std::string filename, section* root)
  : name  ("")
  , fname (filename)
  , root  (root ? root : this_())
{
  if (!filename.empty())
    read (filename);
}

void mb_util::impl::ini::section::read (std::string filename)
{

  std::ifstream input;

  // build ini - open file and parse each line
  input.open (filename.c_str (), std::ios::in);

  if ( ! input.is_open () )
  {
    saga_ini_line_msg ("Cannot open file ", filename);
  }

  // parse file
  std::string                line;
  std::vector <std::string>  lines;

  while ( std::getline (input, line) )
  {
    lines.push_back (line);
  }

  parse (filename, lines);

  input.close ();
}

void mb_util::impl::ini::section::parse (std::string sourcename,
        std::vector <std::string> lines)
{
  int linenum = 0;

  // NOTE: Can't use shared_ptr here, because we're called from the constructor!
  section * current = this;

  for ( unsigned int i = 0; i < lines.size (); i++ )
  {
    std::string line = lines[i];

    ++linenum;

    // remove trailing new lines and white spaces
    line = trim_whitespace (line);

    // skip if empty line
    if ( line.empty () )
      continue;

    // weep out comments
    TR1::smatch what_comment;
    TR1::regex regex_comment (pattern_comment,
            TR1::regex::ECMAScript | TR1::regex::icase);
    if ( TR1::regex_match (line, what_comment, regex_comment) )
    {
      line = trim_whitespace (what_comment[1]);

      if ( line.empty () )
        continue;
    }

    // no comments anymore: line is either section, key=val,
    // or garbage/empty
    TR1::smatch what;
    TR1::regex regex_section (pattern_section,
            TR1::regex::ECMAScript | TR1::regex::icase);
    TR1::regex regex_entry   (pattern_entry,
            TR1::regex::ECMAScript | TR1::regex::icase);

    if ( TR1::regex_match (line, what, regex_section) )
    {
      // found a section line
      if ( 2 != what.size () )
      {
        saga_ini_line_msg ("Cannot parse sec in ", sourcename, linenum);
      }

      current = get_section (what[1]).get ();
    }

    // did not match section, so might be key/val entry
    else if ( TR1::regex_match (line, what, regex_entry) )
    {
      // found a entry line
      if ( 3 != what.size () )
      {
        saga_ini_line_msg ("Cannot parse key/value in ", sourcename, linenum);
      }

      // add key/val to current section
      current->add_entry (what[1], what[2]);
    }
    else
    {
      // Hmm, is not a section, is not an entry, is not empty - must be an
      // error!
      saga_ini_line_msg ("Cannot parse line at ", sourcename, linenum);
    }
  } // loop over lines
}

bool mb_util::impl::ini::section::has_section (std::string sec_name) const
{
  if ( sections.find (sec_name) == sections.end () )
  {
    return false;
  }

  return (true);
}

bool mb_util::impl::ini::section::has_section_full (std::string sec_name) const
{
  std::string::size_type i  = sec_name.find (".");

  if ( i != std::string::npos )
  {
    std::string cor_sec_name = sec_name.substr (0,  i);
    std::string sub_sec_name = sec_name.substr (1 + i);

    section_map::const_iterator it = sections.find (cor_sec_name);

    if ( it != sections.end () )
    {
      return (*it).second->has_section_full (sub_sec_name);
    }

    return false;
  }

  return has_section (sec_name);
}

TR1::shared_ptr<mb_util::impl::ini::section>
mb_util::impl::ini::section::get_section (std::string sec_name)
{
    std::string sub_sec_name;

    std::string::size_type i = sec_name.find (".");
    if ( i != std::string::npos )
    {
        sub_sec_name = sec_name.substr (i + 1);
        sec_name.erase(i);
    }

    TR1::shared_ptr<section> & cor_sec = sections [sec_name];
    if ( !cor_sec )
    {
        TR1::shared_ptr<section> newsec (new section());
        newsec->name  = sec_name;
        newsec->fname = get_fname ();
        newsec->root  = root;

        cor_sec = newsec;
    }

    if (sub_sec_name.empty())
        return cor_sec;
    else
        return cor_sec->get_section (sub_sec_name);
}

void mb_util::impl::ini::section::add_entry (std::string key,
                                          std::string val)
{
  entries[key] = val;
}

#define INVALID "-this-value-is-never-valid-"
bool mb_util::impl::ini::section::has_entry (std::string key) const
{
    std::string result = get_entry(key, INVALID);

    return (result != INVALID);
}

std::string mb_util::impl::ini::section::get_entry (std::string key) const
{
    std::string result = get_entry(key, INVALID);

    if (result == INVALID)
        throw saga::does_not_exist ("No such key (" + key + ") in section " + get_name ());

    return result;
}

std::string mb_util::impl::ini::section::get_entry (std::string key,
                                                 std::string default_val) const
{
  typedef std::vector<std::string> string_vector;

  string_vector split_key = saga::adaptors::utils::split (key, '.');

  key = split_key.back();
  split_key.pop_back();

  section const * cur_section = this;
  string_vector::const_iterator end = split_key.end();
  for (string_vector::const_iterator it = split_key.begin(); it!= end; ++it)
  {
    section_map::const_iterator next = cur_section->sections.find (*it);
    if (cur_section->sections.end () == next)
      return default_val;

    cur_section = next->second.get ();
  }

  entry_map::const_iterator entry = cur_section->entries.find (key);
  if (cur_section->entries.end () == entry)
    return default_val;

  std::string result = expand_entry (entry->second);
  if (result.empty ())
    return default_val;

  return result;
}

void mb_util::impl::ini::section::dump (int ind, std::ostream& strm, std::string parent) const
{
  // ind is ignored, and should be deprcated
  dump (strm, parent);
}


void mb_util::impl::ini::section::dump (std::ostream& strm, std::string parent) const
{
  std::string parent_dot = parent;

  if ( ! parent.empty () )
  {
    parent_dot += ".";
  }

  std::string full_name = parent_dot + get_name ();

  if ( ! full_name.empty () )
  {
    strm << "[" << full_name << "]\n";
  }

  for (entry_map::const_iterator it = entries.begin(); it != entries.end(); ++it)
  {
    std::string key = (*it).first;
    std::string raw = (*it).second;
    std::string exp = expand_entry (raw);

    // try to align the '=' at position tab stops
    std::string key_s = key;
    while ( ((key_s.size () - 1) % 8) != 0 ) { key_s += " "; }
    key_s += " = ";

    strm << "  " << key_s << raw << std::endl;

    if ( raw != exp )
    {
      strm << "# " << key_s << exp << std::endl;
    }
  }

  for (section_map::const_iterator it = sections.begin(); it != sections.end(); ++it)
  {
    (*it).second->dump (strm, full_name);
  }

}

void mb_util::impl::ini::section::merge (std::string filename)
{
    TR1::shared_ptr<section> sec (new section(filename, root));
    merge (sec);
}

void mb_util::impl::ini::section::merge (TR1::shared_ptr<section> other)
{
    // merge entries: keep own entries, and add other entries
    entry_map::const_iterator end = other->entries.end();
    for (entry_map::const_iterator it = other->entries.begin(); it != end; ++it)
    {
        entries[(*it).first] = (*it).second;
    }

    // merge subsections
    section_map::const_iterator ends = other->sections.end();
    for (section_map::const_iterator it = other->sections.begin(); it != ends; ++it)
    {
        TR1::shared_ptr<section> sec = get_section ((*it).first);
        sec->merge ((*it).second);
    }
}

///////////////////////////////////////////////////////////////////////////////

void
mb_util::impl::ini::section::saga_ini_line_msg (std::string msg, std::string file, int lnum)
{
  if ( lnum > 0 )
  {
    std::stringstream ss;
    ss << lnum;
    throw saga::no_success (msg + " " + file + ":" + ss.str ());
  }

  throw saga::no_success (msg + " " + file);
}

///////////////////////////////////////////////////////////////////////////////
void mb_util::impl::ini::section::expand_entry(std::string& value,
        std::string::size_type begin) const
{
    std::string::size_type p = value.find_first_of("$", begin+1);
    while (p != std::string::npos && value.size()-1 != p) {
        if ('[' == value[p+1])
            expand_bracket(value, p);
        else if ('{' == value[p+1])
            expand_brace(value, p);
        p = value.find_first_of("$", p+1);
    }
}

///////////////////////////////////////////////////////////////////////////////
// find the matching closing brace starting from 'begin', escaped braces will 
// be un-escaped
inline std::string::size_type 
find_next(char const* ch, std::string& value, 
    std::string::size_type begin = (std::string::size_type)(-1))
{
    std::string::size_type end = value.find_first_of(ch, begin+1);
    while (end != std::string::npos) {
        if (end != 0 && value[end-1] != '\\')
            break;
        value.replace(end-1, 2, ch);
        end = value.find_first_of(ch, end);
    } 
    return end;
}

///////////////////////////////////////////////////////////////////////////////
void mb_util::impl::ini::section::expand_bracket(std::string& value,
        std::string::size_type begin) const
{
    // expand all keys embedded inside this key
    expand_entry(value, begin);

    // now expand the key itself
    std::string::size_type end = find_next("]", value, begin+1);
    if (end != std::string::npos) 
    {
        std::string to_expand = value.substr(begin+2, end-begin-2);
        std::string::size_type colon = find_next(":", to_expand);

        std::string deflt;
        if (colon != std::string::npos)
        {
            deflt = to_expand.substr(colon + 1);
            to_expand.erase(colon);
        }

        value.replace(begin, end-begin+1, root->get_entry(to_expand, deflt));
    }
}

void mb_util::impl::ini::section::expand_brace(std::string& value,
        std::string::size_type begin) const
{
    // expand all keys embedded inside this key
    expand_entry(value, begin);

    // now expand the key itself
    std::string::size_type end = find_next("}", value, begin+1);
    if (end != std::string::npos) 
    {
        std::string to_expand = value.substr(begin+2, end-begin-2);
        std::string::size_type colon = find_next(":", to_expand);
        if (colon == std::string::npos) {
            char* env = ::getenv(to_expand.c_str());
            value.replace(begin, end-begin+1, 0 != env ? env : "");
        }
        else {
            char* env = ::getenv(to_expand.substr(0, colon).c_str());
            value.replace(begin, end-begin+1, 
                0 != env ? std::string(env) : to_expand.substr(colon+1));
        }
    }
}

std::string mb_util::impl::ini::section::expand_entry (std::string value) const
{
    expand_entry(value, (std::string::size_type)(-1));
    return value;
}

