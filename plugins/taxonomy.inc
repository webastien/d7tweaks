<?php
function sebd7tweaks_API_taxonomy_declare_features() {
  $features = array(
    'enhance_forms'        => t('Add tabs on %entity forms to dispatch fields in general / advanced sections', array('%entity' => 'term')),
    'add_link_on_selector' => t('Add a direct admin link beside taxonomy term selectors'),
    'hide_help'            => t('Hide help hook implementation on taxonomy pages'),
    'custom_overview_page' => t('Replace taxonomy overview page by our own, simplier and with permission verification on actions'),
    'rename_menu'          => t('Replace %term1 by %term2 in the menu entries to help editors understand what is it',
                                array('%term1' => t('Taxonomy'), '%term2' => t('Categories'))),
  );

  return $features;
}

function sebd7tweaks_API_taxonomy_declare_additions() {
  $additions = array(
    'disabled_term_access' => array(
      '#type'          => 'checkboxes',
      '#title'         => t('Disable terms page for those vocabularies'),
      '#options'       => array(),
      '#default_value' => variable_get('sebd7tweaks_disabled_term_access', array()),
    ),
    'disabled_term_description' => array(
      '#type'          => 'checkboxes',
      '#title'         => t('Disable terms description for those vocabularies'),
      '#options'       => array(),
      '#default_value' => variable_get('sebd7tweaks_disabled_term_description', array()),
    ),
  );
  // Provide a checkbox by vocabulary
  foreach (taxonomy_get_vocabularies() as $vocabulary) {
    $additions['disabled_term_access']['#options'][$vocabulary->machine_name] = $vocabulary->name;
    $additions['disabled_term_description']['#options'][$vocabulary->machine_name] = $vocabulary->name;
  }

  return $additions;
}

function sebd7tweaks_API_taxonomy_nested_tree($terms = array(), $max_depth = NULL, $parent = 0, $parents_index = array(), $depth = 0) {
  if (is_int($terms)) {
    $terms = taxonomy_get_tree($terms);
  }

  foreach($terms as $term) {
    foreach($term->parents as $term_parent) {
      if ($term_parent == $parent) {
        $return[$term->tid] = $term;
      }
      else {
        $parents_index[$term_parent][$term->tid] = $term;
      }
    }
  }

  foreach($return as &$term) {
    if (isset($parents_index[$term->tid]) && (is_null($max_depth) || $depth < $max_depth)) {
      $term->children = sebd7tweaks_API_taxonomy_nested_tree($parents_index[$term->tid], $max_depth, $term->tid, $parents_index, $depth + 1);
    }
  }

  return $return;
}

