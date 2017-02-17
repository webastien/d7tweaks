<?php
function sebd7tweaks_API_block_declare_features() {
  return array(
    'enhance_forms'    => t('Add tabs on %entity forms to dispatch fields in general / advanced sections', array('%entity' => 'block')),
    'simplify_regions' => t('Simplify region selectors'),
    'hide_help'        => t('Hide help hook implementation on block pages'),
  );
}

function sebd7tweaks_API_block_declare_additions() {
  $additions = array(
    'hidden_blocks' => array(
      '#type'          => 'checkboxes',
      '#title'         => t('Disable those blocks'),
      '#options'       => array(),
      '#default_value' => variable_get('sebd7tweaks_hidden_blocks', array()),
    ),
  );
  // Required to get the same list than block_admin_display_form uses.
  module_load_include('inc', 'block', 'block.admin');
  // Populate checkboxes
  foreach (block_admin_display_prepare_blocks(variable_get('theme_default', 'bartik')) as $block) {
    $additions['hidden_blocks']['#options'][$block['module'] .'_'. $block['delta']] = $block['info'];
  }
  // Sort by block info
  asort($additions['hidden_blocks']['#options']);

  return $additions;
}

