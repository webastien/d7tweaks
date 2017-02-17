<?php
function sebd7tweaks_API_search_declare_features() {
  return array(
    'enhance_node_search_form' => t('Prefill the search node form with posted values instead of the complex search string.'),
    'expand_advanced_options'  => t('Expand advanced options if at least one of them is filled.'),
  );
}

function sebd7tweaks_API_search_parse_search_terms($form) {
  $terms  = array('keys' => array(), 'types' => array(), 'languages' => array(), 'or' => array(), 'phrase' => array(), 'negative' => array());
  $helper = '±±±';
  $search = str_replace(' OR ', $helper .'OR'. $helper, $form['basic']['keys']['#default_value']);
  $search = explode(' ', preg_replace_callback('/"[^"]+"/', create_function('$matches',
    '$matches[0] = preg_replace("/  ([^ ]+)  /", " '. $helper .'$1'. $helper .' ", $matches[0]);'.
    'return str_replace(" ", "\" \"", $matches[0]);'), $search));

  foreach ($search as $term) {
    if (strpos($term, $helper .'OR'. $helper) !== FALSE) {
      $terms['or'] = array_merge($terms['or'], explode($helper .'OR'. $helper, $term));
    }
    elseif (strpos($term, 'type:') === 0) {
      $terms['types'] = array_merge($terms['types'], explode(',', substr($term, 5)));
    }
    elseif (strpos($term, 'language:') === 0 && isset($form['advanced']['language'])) {
      $terms['languages'] = array_merge($terms['languages'], explode(',', substr($term, 8)));
    }
    elseif (substr($term, 0, 1) == '-') {
      $terms['negative'][] = substr($term, 1);
    }
    elseif (substr($term, 0, 1) == '"') {
      $terms['phrase'][] = strtr($term, array('"' => '', $helper => '"'));
    }
    else {
      $terms['keys'][] = $term;
    }
  }

  $form['basic']['keys']['#default_value'] = implode(' ', $terms['keys']);
  $form['advanced']['type']['#default_value'] = $terms['types'];
  $form['advanced']['keywords']['or']['#default_value'] = implode(' ', $terms['or']);
  $form['advanced']['keywords']['phrase']['#default_value'] = implode(' ', $terms['phrase']);
  $form['advanced']['keywords']['negative']['#default_value'] = implode(' ', $terms['negative']);
  // Only if the option exists [ @see node_form_search_form_alter() ]
  if (isset($form['advanced']['language'])) {
    $form['advanced']['language']['#default_value'] = $terms['languages'];
  }

  if (sebd7tweaks_feature('search.expand_advanced_options')) {
    unset($terms['keys']);
    $form['advanced']['#collapsed'] = (count(array_filter($terms)) == 0);
  }

  return $form;
}

