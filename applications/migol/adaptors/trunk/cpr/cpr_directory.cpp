//  Copyright (c) 2005-2008 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <map>
#include <vector>

#include <boost/tokenizer.hpp>

#include <saga/saga.hpp>

#include <saga/impl/config.hpp>
#include <saga/impl/exception_list.hpp>

#include "config.hpp"
#include "cpr_directory.hpp"

namespace cpr
{

  ///////////////////////////////////////////////////////////////////////////////
  //  constructor
  cpr_directory_cpi_impl::cpr_directory_cpi_impl (proxy                * p, 
                                                  cpi_info const       & info,
                                                  saga::ini::ini const & glob_ini, 
                                                  saga::ini::ini const & adap_ini,
                                                  TR1::shared_ptr <saga::adaptor> adaptor)
    : base_cpi (p, info, adaptor, cpi::Noflags)
  {
    // first usage of this adaptor
    instance_data data (this);
    saga::url cpr_url (data->location_);


    // translate 'any' and 'cpr' url schemes to lfn, decline all others
    std::string scheme (cpr_url.get_scheme ());

    if ( ! scheme.empty () && 
           scheme != "cpr" &&
           scheme != "any")
    {
      SAGA_ADAPTOR_THROW (std::string ("cpr::cpr_directory_cpi_impl::init: "
                                       "cannot handle checkpoint dir name: ") + cpr_url.get_url (), 
                          saga::BadParameter);
    }

    cpr_url.set_scheme ("lfn");


    // create a logical directory, wich acts as container for the checkpoints
    //ld_ = new saga::replica::logical_directory (cpr_url);
  }

  ///////////////////////////////////////////////////////////////////////////////
  //  destructor
  cpr_directory_cpi_impl::~cpr_directory_cpi_impl (void)
  {
  }

  ///////////////////////////////////////////////////////////////////////////////
  void cpr_directory_cpi_impl::check_ld (void)
  {
    if ( ld_ == NULL )
    {
      SAGA_ADAPTOR_THROW ("cpr::cpr_directory_cpi_impl instance is not"
                          "initialized.", saga::IncorrectState);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////
  //  SAGA CPI functions 

  ///////////////////////////////////////////////////////////////////////////////
  // attribute functions
  void cpr_directory_cpi_impl::sync_attribute_exists (bool     & ret, 
                                                      std::string key)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented); 
    //ret = ld_->attribute_exists (key);
  }

  void cpr_directory_cpi_impl::sync_attribute_is_readonly (bool      & ret, 
                                                           std::string key)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_attribute_is_writable (bool     & ret, 
                                                           std::string key)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_attribute_is_vector (bool      & ret, 
                                                         std::string key)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_attribute_is_extended (bool      & ret, 
                                                           std::string key)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  ///////////////////////////////////////////////////////////////////////////////
  void cpr_directory_cpi_impl::sync_get_attribute (std::string & ret, 
                                                   std::string   key)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  ///////////////////////////////////////////////////////////////////////////////
  void cpr_directory_cpi_impl::sync_get_vector_attribute (std::vector <std::string> & ret, 
                                                          std::string                 key)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  ///////////////////////////////////////////////////////////////////////////////
  void cpr_directory_cpi_impl::sync_set_attribute (saga::impl::void_t &, 
                                                   std::string key, 
                                                   std::string val)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  ///////////////////////////////////////////////////////////////////////////////
  void cpr_directory_cpi_impl::sync_set_vector_attribute (saga::impl::void_t &, 
                                                          std::string               key, 
                                                          std::vector <std::string> val)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  ///////////////////////////////////////////////////////////////////////////////
  void cpr_directory_cpi_impl::sync_remove_attribute (saga::impl::void_t &, 
                                                      std::string key)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  ///////////////////////////////////////////////////////////////////////////////
  void cpr_directory_cpi_impl::sync_list_attributes (std::vector <std::string> & keys)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  ///////////////////////////////////////////////////////////////////////////////
  void cpr_directory_cpi_impl::sync_find_attributes (std::vector <std::string> & keys, 
                                                     std::string                 pattern)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  ///////////////////////////////////////////////////////////////////////////////
  // namespace_entry functions
  void cpr_directory_cpi_impl::sync_get_url (saga::url & url)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_get_cwd (saga::url & url)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_get_name (saga::url & url)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_is_dir (bool & ret)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_is_entry (bool & ret)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_is_link (bool & ret)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_remove (saga::impl::void_t &, int flags)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_close (saga::impl::void_t &, double) {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_list (std::vector <saga::url> & ret, 
                                          std::string pattern, int flags)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_find (std::vector <saga::url> & ret, 
                                          std::string pattern, int flags)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_exists (bool    & ret, 
                                            saga::url entry)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_is_dir (bool    & ret, 
                                            saga::url entry)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_is_entry (bool    & ret, 
                                              saga::url entry)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_is_link (bool    & ret, 
                                             saga::url entry)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_get_num_entries (unsigned int & num_entries)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_get_entry (saga::url  & entry, 
                                               unsigned int num)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_remove (saga::impl::void_t &, 
                                            saga::url entry, 
                                            int       flags)
  {
        SAGA_ADAPTOR_THROW ("Not implemented.", 
                            saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_open (saga::name_space::entry & ret, 
                                          saga::url entry, 
                                          int       flags)
  {
      SAGA_ADAPTOR_THROW ("Not implemented.", 
                          saga::NotImplemented);                        saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_open_dir (saga::name_space::directory & ret, 
                                              saga::url entry, 
                                              int       flags)
  {
      SAGA_ADAPTOR_THROW ("Not implemented.", 
                          saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_change_dir (saga::impl::void_t &, 
                                                saga::url dir)
  {
      SAGA_ADAPTOR_THROW ("Not implemented.", 
                          saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_make_dir (saga::impl::void_t &, 
                                              saga::url dir, 
                                              int       flags)
  {
      SAGA_ADAPTOR_THROW ("Not implemented.", 
                          saga::NotImplemented);
  }

  ///////////////////////////////////////////////////////////////////////////////
  //  logical_directory functions
  void cpr_directory_cpi_impl::sync_open (saga::cpr::checkpoint & ret, 
                                          saga::url entry, 
                                          int       flags)
  {
      SAGA_ADAPTOR_THROW ("Not implemented.", 
                          saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_open_dir (saga::cpr::directory & ret, 
                                              saga::url entry, 
                                              int       flags)
  {
      SAGA_ADAPTOR_THROW ("Not implemented.", 
                          saga::NotImplemented);
  }

  void cpr_directory_cpi_impl::sync_find (std::vector <saga::url> & ret, 
                                          std::string               pattern, 
                                          std::vector <std::string> patterns, 
                                          int                       flags, 
                                          std::string)
  {
      
      SAGA_ADAPTOR_THROW ("Not implemented.", 
                          saga::NotImplemented);
  }

}   // namespace cpr
///////////////////////////////////////////////////////////////////////////////


