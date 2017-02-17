<?php
#################
# API functions #
#################

/**
 * API function: Dispatch form elements into tabs
 */
function sebd7tweaks_API_fapi_dispatch_elements($form, $advanced_fields = array(), $general_fields = NULL, $custom_tabs = array()) {
  // Init dispatcher array which will bring all deffered moving tasks to the pre_render callback
  $form['#sebd7tweaks_elements_to_dispatch'] = array('general' => array());
  // Get list of form elements (before we add ours)
  $elements = element_children($form);
  // Always create the general tab
  _sebd7tweaks_API_fapi_create_tab($form, 'general', t('General informations'));
  // Create the advanced tab if required
  if (count($advanced_fields) > 0) {
    _sebd7tweaks_API_fapi_create_tab($form, 'advanced', t('Advanced informations'), 'sebd7tweaks_advanced_content');
  }
  // ... and custom tabs if any
  foreach ($custom_tabs as $name => $infos) {
    _sebd7tweaks_API_fapi_create_tab($form, $name, isset($infos[0])? $infos[0]: $name, isset($infos[1])? $infos[1]: FALSE);
  }
  // Preare form elements to be moving after the form will be build
  foreach ($elements as $index => $key) {
    if (in_array($key, $advanced_fields)) {
      // Can be moved directly with field_group module
      $form[$key]['#group'] = 'sebd7tweaks_advanced_content';
    }
    elseif ( (is_array($general_fields) && in_array($key, $general_fields)) || (is_null($general_fields)
      && $key != 'actions' && isset($form[$key]['#type']) && !in_array($form[$key]['#type'], array('hidden', 'value', 'token')))
    ) {
      // Require a specific code, @see sebd7tweaks_dispatch_elements_after_build()
      $form['#sebd7tweaks_elements_to_dispatch']['general'][(int) @$form[$key]['#weight'] .'.'. $index] = $key;
    }
  }
  // Attach a custom pre render callback to dispatch form elements into General tab
  $form = sebd7tweaks_API_fapi_add_callback($form, 'pre_render', 'dispatch_elements');

  return $form;
}

/**
 * API function: Pre render part of forms enhancements
 */
function sebd7tweaks_API_fapi_dispatch_elements_pre_render($form) {
  if (isset($form['#sebd7tweaks_elements_to_dispatch']) && is_array($form['#sebd7tweaks_elements_to_dispatch'])) {
    foreach ($form['#sebd7tweaks_elements_to_dispatch'] as $tab => $elements) {
      $form["sebd7tweaks_tab_$tab"]['#access'] = (count($elements) > 0);

      if ($form["sebd7tweaks_tab_$tab"]['#access']) {
        // Sort elements by their keys (representing their weights) to keep them in order
        ksort($elements);
        // Move them in the appropriate tab
        foreach ($elements as $key) {
          $form["sebd7tweaks_tab_$tab"][$key] = $form[$key];
          unset($form[$key]);
        }
      }
    }
  }

  return $form;
}

/**
 * API function: Attach an alter build callback to the given form.
 */
function sebd7tweaks_API_fapi_add_callback($form, $type, $callback, $first = FALSE) {
  if (!array_key_exists("#$type", $form)) {
    $form["#$type"] = array();
  }

  if ($first) {
    array_unshift($form["#$type"], "sebd7tweaks_{$callback}_$type");
  }
  else {
    $form["#$type"][] = "sebd7tweaks_{$callback}_$type";
  }

  return $form;
}

/**
 * API function: Act on widget of type "number".
 * For better UI, limit allowed characters
 */
function sebd7tweaks_API_fapi_widget_number($element, $context) {
  if (sebd7tweaks_feature('number.restrict_characters')) {
    switch ($element['value']['#number_type']) {
      case 'float':
      case 'decimal':
        $separator = $context['field']['settings']['decimal_separator'];
        $pattern   = "[0-9{$separator}]+";
        break;

      case 'integer': $pattern = "[0-9]+"; $separator = ''; break;
      default:        $pattern = FALSE;
    }

    if ($pattern) {
      $element['value']['#attributes']['class'][] = 'sebd7tweaks-pattern';
      $element['value']['#attributes']['pattern'] = $pattern;
      drupal_add_js(array('sebd7tweaks' => array('patterns' => array($pattern => '[^0-9'. preg_quote($separator, '/') .']'))), 'setting');
      drupal_add_js(drupal_get_path('module', 'sebd7tweaks') .'/js/patterns.js');
    }
  }

  return $element;
}

/**
 * API function: Act on widget of type "taxonomy_term_reference".
 * For better UI, add link to the taxonomy terms list (for administrators only)
 */
function sebd7tweaks_API_fapi_widget_taxonomy_term_reference($element, $context) {
  if ( sebd7tweaks_feature('taxonomy.add_link_on_selector')
    && ($vocabulary = taxonomy_vocabulary_machine_name_load($context['field']['settings']['allowed_values'][0]['vocabulary']))
    && user_access("edit terms in {$vocabulary->vid}")
  ) {
    $element['#field_suffix'] = l(t('Edit terms in %vocabulary', array('%vocabulary' => $vocabulary->name)),
                                  "admin/structure/taxonomy/{$vocabulary->machine_name}", array('html' => TRUE));
    $element['#attached']['css'][] = drupal_get_path('module', 'sebd7tweaks') .'/css/enhance-term-selectors.css';
  }

  return $element;
}

/**
 * API function: Act on widget of type "google_map_field".
 * For better UI, simplify the widget
 */
function sebd7tweaks_API_fapi_widget_google_map_field($element, $context) {
  if (sebd7tweaks_feature('google_map_field.simplify_map_widget')) {
    unset($element['center_on']['#description']);
    // Restore title and description from the field settings (hard coded in google_map_field!)
    $element['#title']       = $element['name']['#title'] = $context['instance']['label'];
    $element['#description'] = $context['instance']['description'];
    // Add our after build callback to tweak the default UI
    $element['#after_build'][] = 'sebd7tweaks_google_map_field_after_build';
  }

  return $element;
}

/**
 * API function: Remove description for children of the given elements.
 */
function sebd7tweaks_API_fapi_remove_description($element, $recursive = TRUE) {
  if (($children = element_children($element)) && count($children) > 0) {
    foreach ($children as $key) {
      if (isset($element[$key]['#description'])) {
        unset($element[$key]['#description']);
      }
      $element[$key] = sebd7tweaks_API_fapi_remove_description($element[$key], $recursive);
    }
  }

  return $element;
}

####################
# Helper functions #
####################

/**
 * API helper: Create a custom tab.
 */
function _sebd7tweaks_API_fapi_create_tab(&$form, $name, $title, $container = FALSE) {
  if (!isset($form['sebd7tweaks_tabs'])) {
    $form['sebd7tweaks_tabs'] = array('#type' => 'horizontal_tabs');
  }

  $form["sebd7tweaks_tab_$name"] = array('#group' => 'sebd7tweaks_tabs', '#type' => 'fieldset', '#title' => $title);

  if ($container) {
    if (isset($form[$container])) {
      $form['#sebd7tweaks_elements_to_dispatch'][$name][] = $container;
    }
    else {
      $form["sebd7tweaks_tab_$name"] += array($container => array('#type' => 'vertical_tabs'));
    }
  }
}

