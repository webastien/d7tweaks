<?php
/**
 * API function: Return the value of the given field for nodes.
 *
 * @param   array   $nodes  List of nids
 * @param   string  $type   Node type
 * @param   string  $field  Field name
 *
 * @return  string  $nodes  List of values, keyed by the nid
 */
function sebd7tweaks_API_field_get_value($nodes, $type, $field) {
  $fields = field_info_instances('node', $type);
  $nodes  = array_flip($nodes);

  foreach ($nodes as $nid => &$node) {
    $node = (object) array('nid' => $nid, 'type' => $type);
  }

  field_attach_load('node', $nodes, FIELD_LOAD_CURRENT, array('field_id' => $fields[$field]['field_id']));

  foreach ($nodes as &$node) {
    $node = $node->{$field};
  }

  return $nodes;
}

