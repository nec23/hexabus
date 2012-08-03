#ifndef LIBHBC_AST_DATATYPES
#define LIBHBC_AST_DATATYPES

#include <libhbc/common.hpp>
#include <libhbc/hbc_enums.hpp>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include <boost/variant/recursive_variant.hpp>
#include <boost/spirit/home/qi.hpp> 
#include <boost/spirit/home/support/info.hpp> 
#include <boost/spirit/home/phoenix.hpp> 
#include <boost/spirit/home/phoenix/statement/sequence.hpp> 

namespace hexabus {

// Data structures that hold the syntax tree
  struct include_doc {
    unsigned int lineno;
    std::string filename;
  };

  struct access_level_doc {
    // TODO
  };

  struct placeholder_doc {
    std::string name;
  };

  typedef boost::variant<hexabus::placeholder_doc, float> constant_doc;

  struct ep_name_doc {
    std::string name;
  };

  struct ep_datatype_doc {
    datatype dtype;
  };

  struct ep_access_doc {
    std::vector<access_level_doc> access_levels;
  };

  typedef boost::variant<ep_name_doc, ep_datatype_doc, ep_access_doc> endpoint_cmd_doc;

  struct endpoint_doc {
    unsigned int lineno;
    unsigned int eid;
    std::vector<endpoint_cmd_doc> cmds;
  };

  struct eid_list_doc {
    std::vector<unsigned int> eids;
  };

  struct alias_ip_doc {
    std::string ipv6_address;
  };

  struct alias_eids_doc {
    eid_list_doc eid_list;
  };

  typedef boost::variant<alias_ip_doc, alias_eids_doc> alias_cmd_doc;

  struct alias_doc {
    unsigned int lineno;
    std::string device_name;
    std::vector<alias_cmd_doc> cmds;
  };

  typedef boost::variant<std::string, placeholder_doc> global_endpoint_id_element; // TODO name it _doc?

  struct global_endpoint_id_doc {
    global_endpoint_id_element device_alias;
    global_endpoint_id_element endpoint_name;
  };

  struct atomic_condition_doc {
    global_endpoint_id_doc geid;
    comp_operator comp_op;
    constant_doc constant;
  };

  struct compound_condition_doc;
  typedef boost::variant<atomic_condition_doc, boost::recursive_wrapper<compound_condition_doc> > condition_doc;

  struct compound_condition_doc {
    condition_doc condition_a;
    bool_operator bool_op;
    condition_doc condition_b;
  };

  struct write_command_doc {
    unsigned int lineno;
    global_endpoint_id_doc geid;
    constant_doc constant;
  };

  struct command_doc {
    write_command_doc write_command;
  };

  struct goto_command_doc {
    unsigned int lineno;
    std::string target_state;
  };

  struct command_block_doc {
    std::vector<command_doc> commands;
    goto_command_doc goto_command;
  };

  struct guarded_command_block_doc {
    condition_doc condition;
    command_block_doc command_block;
  };

  struct if_clause_doc {
    unsigned int lineno;
    guarded_command_block_doc if_block;
    std::vector<guarded_command_block_doc> else_if_blocks;
    command_block_doc else_block;
  };

  struct in_clause_doc {
    unsigned int lineno;
    std::string name;
    std::vector<if_clause_doc> if_clauses;
  };

  struct stateset_doc {
    std::vector<std::string> states;
  };

  struct statemachine_doc {
    std::string name;
    stateset_doc stateset;
    std::vector<in_clause_doc> in_clauses;
  };

  struct placeholder_list_doc {
    std::vector<placeholder_doc> placeholders;
  };

  struct module_doc {
    unsigned int lineno;
    std::string name;
    placeholder_list_doc placeholderlist;
    stateset_doc stateset;
    std::vector<in_clause_doc> in_clauses;
  };

  typedef boost::variant<constant_doc, std::string> inst_parameter_doc;

  struct instantiation_doc {
    unsigned int lineno;
    std::string name;
    std::string moduleclass;
    std::vector<inst_parameter_doc> parameters;
  };

  typedef boost::variant<include_doc, endpoint_doc, alias_doc, statemachine_doc, module_doc> hbc_block;

  struct hbc_doc {
    std::vector<hbc_block> blocks;
  };
};

// the structures are filled with data
BOOST_FUSION_ADAPT_STRUCT(
  hexabus::include_doc,
  (unsigned int, lineno)
  (std::string, filename)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::placeholder_doc,
  (std::string, name)
  (std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::ep_name_doc,
  (std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::ep_datatype_doc,
  (hexabus::datatype, dtype)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::ep_access_doc,
  (std::vector<hexabus::access_level_doc>, access_levels)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::endpoint_doc,
  (unsigned int, lineno)
  (unsigned int, eid)
  (std::vector<hexabus::endpoint_cmd_doc>, cmds)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::eid_list_doc,
  (std::vector<unsigned int>, eids)
  (std::vector<unsigned int>, eids)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::alias_ip_doc,
  (std::string, ipv6_address)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::alias_eids_doc,
  (hexabus::eid_list_doc, eid_list)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::alias_doc,
  (unsigned int, lineno)
  (std::string, device_name)
  (std::vector<hexabus::alias_cmd_doc>, cmds)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::global_endpoint_id_doc,
  (hexabus::global_endpoint_id_element, device_alias)
  (hexabus::global_endpoint_id_element, endpoint_name)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::statemachine_doc,
  (std::string, name)
  (hexabus::stateset_doc, stateset)
  (std::vector<hexabus::in_clause_doc>, in_clauses)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::placeholder_list_doc,
  (std::vector<hexabus::placeholder_doc>, placeholders)
  (std::vector<hexabus::placeholder_doc>, placeholders)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::module_doc,
  (unsigned int, lineno)
  (std::string, name)
  (hexabus::placeholder_list_doc, placeholderlist)
  (hexabus::stateset_doc, stateset)
  (std::vector<hexabus::in_clause_doc>, in_clauses)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::in_clause_doc,
  (unsigned int, lineno)
  (std::string, name)
  (std::vector<hexabus::if_clause_doc>, if_clauses)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::stateset_doc,
  (std::vector<std::string>, states)
  (std::vector<std::string>, states)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::command_block_doc,
  (std::vector<hexabus::command_doc>, commands)
  (hexabus::goto_command_doc, goto_command)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::guarded_command_block_doc,
  (hexabus::condition_doc, condition)
  (hexabus::command_block_doc, command_block)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::if_clause_doc,
  (unsigned int, lineno)
  (hexabus::guarded_command_block_doc, if_block)
  (std::vector<hexabus::guarded_command_block_doc>, else_if_blocks)
  (hexabus::command_block_doc, else_block)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::command_doc,
  (hexabus::write_command_doc, write_command)   // TODO variant if there's more
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::atomic_condition_doc,
  (hexabus::global_endpoint_id_doc, geid)
  (hexabus::comp_operator, comp_op)
  (hexabus::constant_doc, constant)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::compound_condition_doc,
  (hexabus::condition_doc, condition_a)
  (hexabus::bool_operator, bool_op)
  (hexabus::condition_doc, condition_b)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::goto_command_doc,
  (unsigned int, lineno)
  (std::string, target_state)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::write_command_doc,
  (unsigned int, lineno)
  (hexabus::global_endpoint_id_doc, geid)
  (hexabus::constant_doc, constant)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::instantiation_doc,
  (unsigned int, lineno)
  (std::string, name)
  (std::string, moduleclass)
  (std::vector<hexabus::inst_parameter_doc>, parameters)
)

BOOST_FUSION_ADAPT_STRUCT(
  hexabus::hbc_doc,
  (std::vector<hexabus::hbc_block>, blocks)
)

#endif // LIBHBC_AST_DATATYPES

