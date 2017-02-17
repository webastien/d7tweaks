<?php
/**
 * API function: Return default rules contained in <module>/rules (each rule are a separated file nammed "<name>.inc")
 */
function sebd7tweaks_API_rules_default_rules($module) {
  $rules = array();
  // Rather than embedding long code from each rules, each one have its own file...
  foreach(drupal_system_listing('/\.inc$/', drupal_get_path('module', $module) .'/rules', 'name', 0) as $file) {
    include $file->uri;
    $rules['rules_'. $file->name] = rules_import(trim($rule));
  }
  // Return the generated views array
  return $rules;
}

