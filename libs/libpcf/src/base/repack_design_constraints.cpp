#include "repack_design_constraints.h"

#include <algorithm>

#include "openfpga_port_parser.h"
#include "openfpga_tokenizer.h"
#include "vtr_assert.h"
#include "vtr_log.h"

/************************************************************************
 * Member functions for class RepackDesignConstraints
 ***********************************************************************/

/************************************************************************
 * Constructors
 ***********************************************************************/
RepackDesignConstraints::RepackDesignConstraints() { return; }

/************************************************************************
 * Public Accessors : aggregates
 ***********************************************************************/
RepackDesignConstraints::repack_design_constraint_range
RepackDesignConstraints::design_constraints() const {
  return vtr::make_range(repack_design_constraint_ids_.begin(),
                         repack_design_constraint_ids_.end());
}

/************************************************************************
 * Public Accessors : Basic data query
 ***********************************************************************/
RepackDesignConstraints::e_design_constraint_type RepackDesignConstraints::type(
  const RepackDesignConstraintId& repack_design_constraint_id) const {
  /* validate the design_constraint_id */
  VTR_ASSERT(valid_design_constraint_id(repack_design_constraint_id));
  return repack_design_constraint_types_[repack_design_constraint_id];
}

std::string RepackDesignConstraints::pb_type(
  const RepackDesignConstraintId& repack_design_constraint_id) const {
  /* validate the design_constraint_id */
  VTR_ASSERT(valid_design_constraint_id(repack_design_constraint_id));
  return repack_design_constraint_pb_types_[repack_design_constraint_id];
}

openfpga::BasicPort RepackDesignConstraints::pin(
  const RepackDesignConstraintId& repack_design_constraint_id) const {
  /* validate the design_constraint_id */
  VTR_ASSERT(valid_design_constraint_id(repack_design_constraint_id));
  return repack_design_constraint_pins_[repack_design_constraint_id];
}

std::string RepackDesignConstraints::net(
  const RepackDesignConstraintId& repack_design_constraint_id) const {
  /* validate the design_constraint_id */
  VTR_ASSERT(valid_design_constraint_id(repack_design_constraint_id));
  return repack_design_constraint_nets_[repack_design_constraint_id];
}

std::set<std::string> RepackDesignConstraints::ignore_net_on_pin(
  const std::string& net_name) const {
  std::map<std::string, std::set<std::string>>::const_iterator it =
    ignore_net_pin_map_.find(net_name);
  if (it != ignore_net_pin_map_.end()) {
    return it->second;
  } else {
    return std::set<std::string>();
  }
}

std::string RepackDesignConstraints::find_constrained_pin_net(
  const std::string& pb_type, const openfpga::BasicPort& pin) const {
  std::string constrained_net_name;
  for (const RepackDesignConstraintId& design_constraint :
       design_constraints()) {
    /* If found a constraint, record the net name */
    if ((pb_type == repack_design_constraint_pb_types_[design_constraint]) &&
        (pin == repack_design_constraint_pins_[design_constraint])) {
      constrained_net_name = repack_design_constraint_nets_[design_constraint];
      break;
    }
  }
  return constrained_net_name;
}

openfpga::BasicPort RepackDesignConstraints::net_pin(
  const std::string& net) const {
  openfpga::BasicPort constrained_pin;
  for (const RepackDesignConstraintId& design_constraint :
       design_constraints()) {
    if (net == repack_design_constraint_nets_[design_constraint]) {
      constrained_pin = pin(design_constraint);
      break;
    }
  }
  return constrained_pin;
}

bool RepackDesignConstraints::empty() const {
  return 0 == repack_design_constraint_ids_.size();
}

/************************************************************************
 * Public Mutators
 ***********************************************************************/
void RepackDesignConstraints::reserve_design_constraints(
  const size_t& num_design_constraints) {
  repack_design_constraint_ids_.reserve(num_design_constraints);
  repack_design_constraint_types_.reserve(num_design_constraints);
  repack_design_constraint_pb_types_.reserve(num_design_constraints);
  repack_design_constraint_pins_.reserve(num_design_constraints);
  repack_design_constraint_nets_.reserve(num_design_constraints);
}

RepackDesignConstraintId RepackDesignConstraints::create_design_constraint(
  const RepackDesignConstraints::e_design_constraint_type&
    repack_design_constraint_type) {
  /* Create a new id */
  RepackDesignConstraintId repack_design_constraint_id =
    RepackDesignConstraintId(repack_design_constraint_ids_.size());

  repack_design_constraint_ids_.push_back(repack_design_constraint_id);
  repack_design_constraint_types_.push_back(repack_design_constraint_type);
  repack_design_constraint_pb_types_.emplace_back();
  repack_design_constraint_pins_.emplace_back();
  repack_design_constraint_nets_.emplace_back();

  return repack_design_constraint_id;
}

void RepackDesignConstraints::set_pb_type(
  const RepackDesignConstraintId& repack_design_constraint_id,
  const std::string& pb_type) {
  /* validate the design_constraint_id */
  VTR_ASSERT(valid_design_constraint_id(repack_design_constraint_id));
  repack_design_constraint_pb_types_[repack_design_constraint_id] = pb_type;
}

void RepackDesignConstraints::set_pin(
  const RepackDesignConstraintId& repack_design_constraint_id,
  const openfpga::BasicPort& pin) {
  /* validate the design_constraint_id */
  VTR_ASSERT(valid_design_constraint_id(repack_design_constraint_id));
  repack_design_constraint_pins_[repack_design_constraint_id] = pin;
}

void RepackDesignConstraints::set_net(
  const RepackDesignConstraintId& repack_design_constraint_id,
  const std::string& net) {
  /* validate the design_constraint_id */
  VTR_ASSERT(valid_design_constraint_id(repack_design_constraint_id));
  repack_design_constraint_nets_[repack_design_constraint_id] = net;
}

void RepackDesignConstraints::set_ignore_net_pin_map_(
  const std::string& net_name, const std::string pin_ctx) {
  /* Extract the pb_type name and port name */
  openfpga::StringToken pin_tokenizer(pin_ctx);
  std::vector<std::string> pin_info = pin_tokenizer.split('.');
  /* Expect two contents, otherwise error out */
  if (pin_info.size() != 2) {
    std::string err_msg =
      std::string("Invalid content '") + pin_ctx +
      std::string("' to skip, expect <pb_type_name>.<pin>\n");
    VTR_LOG_ERROR(err_msg.c_str());
    return;
  }
  std::string pb_type_name = pin_info[0];
  openfpga::PortParser port_parser(pin_info[1]);
  openfpga::BasicPort curr_port = port_parser.port();
  if (!curr_port.is_valid()) {
    std::string err_msg =
      std::string("Invalid pin definition '") + pin_ctx +
      std::string("', expect <pb_type_name>.<pin_name>[int:int]\n");
    VTR_LOG_ERROR(err_msg.c_str());
    return;
  }
  ignore_net_pin_map_[net_name].insert(pin_ctx);
}
/************************************************************************
 * Internal invalidators/validators
 ***********************************************************************/
/* Validators */
bool RepackDesignConstraints::valid_design_constraint_id(
  const RepackDesignConstraintId& design_constraint_id) const {
  return (size_t(design_constraint_id) <
          repack_design_constraint_ids_.size()) &&
         (design_constraint_id ==
          repack_design_constraint_ids_[design_constraint_id]);
}

bool RepackDesignConstraints::unconstrained_net(const std::string& net) const {
  return net.empty();
}

bool RepackDesignConstraints::unmapped_net(const std::string& net) const {
  return std::string(REPACK_DESIGN_CONSTRAINT_OPEN_NET) == net;
}
