<?php
function sebd7tweaks_API_less_declare_additions() {
  $patch = drupal_get_path('module', 'sebd7tweaks') .'/patches/less.txt';
  return array('omega_less_responsive_bug_fix' => array('#type' => 'fieldset', '#title' => t('Patch for omega + LESS'), '#collapsible' => TRUE,
    'patch' => array('#type' => 'markup', '#markup' => highlight_string(file_get_contents($patch), TRUE))),
  );
}

