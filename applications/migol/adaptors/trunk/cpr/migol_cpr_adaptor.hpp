//  Copyright (c) 2005-2008 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ADAPTORS_MIGOL_CPR_ADAPTOR_HPP
#define ADAPTORS_MIGOL_CPR_ADAPTOR_HPP

#include <saga/saga/adaptors/adaptor.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace cpr
{
    ///////////////////////////////////////////////////////////////////////////////
    struct adaptor : public saga::adaptor
    {
        typedef saga::impl::v1_0::op_info         op_info;  
        typedef saga::impl::v1_0::cpi_info        cpi_info;
        typedef saga::impl::v1_0::preference_type preference_type;

        // This function registers the adaptor with the factory
        // @param factory the factory where the adaptor registers
        //        its maker function and description table
        saga::impl::adaptor_selector::adaptor_info_list_type 
                adaptor_register (saga::impl::session * s);

        std::string get_name (void) const
        { 
            return BOOST_PP_STRINGIZE (SAGA_ADAPTOR_NAME);
        }
        
        //data
        std::string migol_guid;
        //std::string migol_url;
        //std::string migol_name;
        //std::string migol_state;
        
        //methods
        //std::string  init_migol_context();
        
    };

///////////////////////////////////////////////////////////////////////////////
}   // namespace cpr

#endif // ADAPTORS_MIGOL_CPR_ADAPTOR_HPP

