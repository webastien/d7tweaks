<?php
function sebd7tweaks_API_node_declare_features() {
  return array(
    'enhance_forms'  => t('Add tabs on %entity forms to dispatch fields in general / advanced sections', array('%entity' => 'node')),
    'forms_title'    => t('Use node title instead of "Edit title" in nodes edit forms.'),
    'revs_title'     => t('Use node title instead of "Revision of title from date" on node revision pages.'),
    'i18n_node_list' => t('Use transliteration for order node list (/node/add).'),
  );
}

function sebd7tweaks_API_node_declare_additions() {
  $options = array();

  module_load_include('inc', 'node', 'node.admin');

  foreach (node_node_operations() + module_invoke_all('node_operations') as $operation => $array) {
    $options[$operation] = $array['label'];
  }

  return array(
    'hidden_node_operations' => array(
      '#title'         => t('Hide those node operations'),
      '#type'          => 'checkboxes',
      '#options'       => $options,
      '#default_value' => variable_get('sebd7tweaks_hidden_node_operations', array()),
    ),
  );
}

