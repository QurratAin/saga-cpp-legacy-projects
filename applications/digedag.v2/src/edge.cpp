
#include <saga/saga.hpp>

#include "edge.hpp"
#include "scheduler.hpp"

namespace digedag
{
  edge::edge (const saga::url & src, 
              const saga::url & tgt) 
    : src_url_  (src)
    , tgt_url_  (tgt)
    , src_path_ (src_url_.get_path ())
    , tgt_path_ (tgt_url_.get_path ())
    , state_    (Incomplete)
    , is_void_  (false)
    , fired_    (false)
    , t_valid_  (false)
  {
    if ( tgt_url_ == "" )
    {
      tgt_url_ = src_url_;
      tgt_path_ = tgt_url_.get_path ();
    }
  }

  edge::edge (void)
    : state_    (Incomplete)
    , is_void_  (true)
    , fired_    (false)
    , t_valid_  (false)
  {
  }

  edge::~edge (void)
  {
  }

  bool edge::operator== (const edge & e)
  {
    if ( src_url_   == e.src_url_     &&
         tgt_url_   == e.tgt_url_     &&
         src_path_  == e.src_path_    &&
         tgt_path_  == e.tgt_path_    &&
         state_     == e.state_       &&
         src_node_  == e.src_node_    &&
         tgt_node_  == e.tgt_node_    &&
         scheduler_ == e.scheduler_   )
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  void edge::add_src_node (sp_t <node> src)
  {
    src_node_ = src;
  }

  void edge::add_tgt_node (sp_t <node> tgt)
  {
    tgt_node_ = tgt;
  }

  void edge::dryrun (void)
  {
    if ( state_ == Stopped )
      return;

    if ( Pending != get_state () )
      return;

    if ( src_url_ != tgt_url_ )
    {
      dump ();
    }

    state_ = Done;

    if ( tgt_node_ )
    {
      tgt_node_->dryrun ();
    }
  }


  void edge::reset (void)
  {
    state_ = Incomplete;

    if ( tgt_node_ )
    {
      tgt_node_->reset ();
    }
  }


  // fire() checks if there is still work to do, and if so, starts
  // a thread to do it.
  void edge::fire (void)
  {
    if ( state_ == Stopped )
      return;

    // update state
    get_state ();

    if ( Pending != state_ )
    {
      std::cout << " edge " << get_name_s () << " is not pending" << std::endl;
      return;
    }

    std::cout << " edge " << get_name_s () << " fired" << std::endl;

    // ### scheduler hook
    scheduler_->hook_edge_run_pre (shared_from_this ());
  }

  saga::task edge::work_start (void)
  {
    if ( state_ == Stopped )
      return t_;

    assert ( state_ == Pending );

    // we have work to do...
    state_ = Running;

    std::cout << std::string (" === edge run: ")
              << get_name_s () << std::endl;

    if ( is_void_ )
    {
      // FIXME: we can't fake a noop task :-(
      saga::session session = scheduler_->hook_saga_get_session ();

      saga::job::service js (session, "fork://localhost/");

      t_ = js.get_url <saga::task::Async> ();
      t_valid_ = true;
    }
    else
    {
      saga::session session = scheduler_->hook_saga_get_session ();

      saga::filesystem::file f_src (session, src_url_);

      t_ = f_src.copy <saga::task::Async> (tgt_url_, saga::filesystem::Overwrite
                                                   | saga::filesystem::CreateParents);
      t_valid_ = true;
    }
    
    return t_;
  }

  void edge::work_done (void)
  {
    if ( state_ == Stopped )
      return;

    assert ( state_ != Failed );
    assert ( state_ != Done   );

    state_ = Done;

    std::cout << std::string (" === edge done: ")
              << get_name_s () << std::endl;

    // if we are done copying data, we fire the dependend node
    // this fire may succeed or not - that depends on the availability
    // of _other_ input data to that node.  Only if all data are Done,
    // the fire will actually do anything.  Thus, only the last fire
    // called on a node (i.e. called from its last Pending Edge) will
    // result in a Running node.

    if ( tgt_node_ && ! fired_ )
    {
      fired_ = true;

      std::cout << " === firing dep node " << tgt_node_->get_name () << std::endl;
      tgt_node_->fire ();
    }

    // ### scheduler hook
    scheduler_->hook_edge_run_done (shared_from_this ());

    return;
  }

  void edge::work_failed (void)
  {
    if ( state_ == Stopped )
      return;

    assert ( state_ != Failed );
    assert ( state_ != Done   );

    state_ = Failed;
  }


  void edge::stop (void)
  {
    if ( t_valid_ )
    {
      t_.cancel ();
    }

    state_ = Stopped;
  }

  void edge::dump (void)
  {
    std::cout << std::string ("         edge : ")
              << get_name_s ()
              << " [" << src_url_.get_string ()       << "\t -> " << tgt_url_.get_string () << "] "
              << " (" << state_to_string (state_) << ")" 
              << std::endl;
  }

  void edge::erase_src (void)
  {
    // FIXME: remove the src data
  }

  void edge::erase_tgt (void)
  {
    // FIXME: remove the tgt data
  }

  void edge::set_state (state s)
  {
    state_ = s;
  }

  state edge::get_state (void)
  {
    if ( Incomplete == state_ && src_node_ )
    {
      state src_state = src_node_->get_state ();

      if ( Done == src_state )
      {
        state_ = Pending;
      }
      else if ( Failed == src_state )
      {
        state_ = Failed;
      }
    }

    return state_;
  }

  // FIXME: names are not unique, yet!  Collision occurs when multiple data are
  // exchanged between the same pair of nodes.
  std::string edge::get_name_s (void) const
  {
    std::string src_string = "???";
    std::string tgt_string = "???";

    if ( src_node_ )
      src_string = src_node_->get_name_s ();

    if ( tgt_node_ )
      tgt_string = tgt_node_->get_name_s ();

    return src_string + "->" + tgt_string;
  }

  edge_id_t edge::get_name (void) const
  {
    std::string src_string = "???";
    std::string tgt_string = "???";

    if ( src_node_ )
      src_string = src_node_->get_name_s ();

    if ( tgt_node_ )
      tgt_string = tgt_node_->get_name_s ();

    return edge_id_t (src_string, tgt_string);
  }

  void edge::set_pwd_src (std::string pwd)
  {
    src_url_.set_path (pwd  + src_path_);
  }

  void edge::set_pwd_tgt (std::string pwd)
  {
    tgt_url_.set_path (pwd  + tgt_path_);
  }

  void edge::set_host_src (std::string host) 
  {
    src_url_.set_host (host);
  }

  void edge::set_host_tgt (std::string host) 
  {
    tgt_url_.set_host (host);
  }


  void edge::set_scheduler (sp_t <scheduler> s)
  {
    scheduler_ = s;
  }

} // namespace digedag

