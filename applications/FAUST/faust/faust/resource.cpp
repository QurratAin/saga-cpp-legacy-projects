/*
 *  resource.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 02/13/098.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/faust/resource.hpp>
#include <faust/impl/resource_impl.hpp>

using namespace faust;

////////////////////////////////////////////////////////////////////////////////
//
boost::shared_ptr <faust::impl::resource> resource::get_impl (void) const
{ 
  typedef faust::object base_type;
  return boost::static_pointer_cast <faust::impl::resource> (
                                                        this->base_type::get_impl ());
}

////////////////////////////////////////////////////////////////////////////////
//
resource::resource(faust::resource_description RD) 
: faust::object (new faust::impl::resource(RD), object::Resource)
{
}

////////////////////////////////////////////////////////////////////////////////
//
resource::resource(std::string XMLFileName) 
: faust::object (new faust::impl::resource(XMLFileName), object::Resource)
{
}

////////////////////////////////////////////////////////////////////////////////
//
resource::~resource() 
{
}

////////////////////////////////////////////////////////////////////////////////
//
faust::resource_description resource::get_description()
{
  return get_impl()->get_description();
}
