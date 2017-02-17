<?php
/**
 * Implements hook_views_pre_view().
 */
function sebd7tweaks_views_pre_view(&$view, &$display_id, &$args) {
  if ( $view->name == 'taxonomy_term' && isset($args[0]) && ($tid = $args[0])  && is_numeric($tid)
    && ($disabled = variable_get('sebd7tweaks_disabled_term_access', array())) && count($disabled) > 0
    && ($term     = taxonomy_term_load($tid)) && in_array($term->vocabulary_machine_name, $disabled)
  ) {
    $args = array();
  }
}

