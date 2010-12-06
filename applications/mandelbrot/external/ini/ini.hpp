//  Copyright (c) 2005-2007 Andre Merzky (andre@merzky.net)
//  Copyright (c) 2009 João Abecasis
// 
//  Use, modification and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef _MB_UTIL_INI_H_
#define _MB_UTIL_INI_H_ 1

#include <map>
#include <list>
#include <vector>
#include <iostream>

#include <boost/shared_ptr.hpp>

//////////////////////////////////////////
//
// C++ interface
//
namespace mb_util
{
  // forward declaration
  namespace impl 
  {
    namespace ini
    {
      class section;
    }
  }

  namespace ini
  {
    class   section;

    typedef section ini;

    typedef std::map <std::string, std::string> entry_map;
    typedef std::map <std::string, section>     section_map;

#define SAGA_INI_EXPORT /**/

    class section
    {
      private:
        typedef mb_util::impl::ini::section impl_sec;
        typedef boost::shared_ptr       <impl_sec> shared_sec;

        shared_sec impl_;

        shared_sec get_impl (void) const { return (impl_); }

        explicit section (shared_sec impl);
        explicit section (impl_sec * sec);

        void debug (std::string = "") const;


      public:
        section (std::string filename = "");
        section (const       section  & in);
       ~section (void);


        void           read              (std::string     filename);
        void           parse             (std::string     sourcename, 
                                          std::vector <std::string> lines);
        void           merge             (std::string     second);           
        void           merge             (
                                    const section       & second);           
        void           dump              (int             ind = 0, 
                                          std::ostream&   strm = std::cout)  const;

        void           add_section       (std::string     sec_name, 
                                    const section       & sec);
        bool           has_section       (std::string     sec_name) const;
        bool           has_section_full  (std::string     sec_name) const;
        section        get_section       (std::string     sec_name) const; 
        section_map    get_sections      (void)                     const;

        void           add_entry         (std::string     key,                 
                                          std::string     val);                      
        bool           has_entry         (std::string     key)      const;
        std::string    get_entry         (std::string     key)      const;
        std::string    get_entry         (std::string     key,
                                          std::string     dflt_val) const;
        entry_map      get_entries       (void)                     const;

        section        get_root          (void)                     const;
        std::string    get_name          (void)                     const;
    };

  } // namespace ini

} // namespace mb_util

#endif // MB_UTIL_INI

