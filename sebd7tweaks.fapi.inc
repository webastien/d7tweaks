<?php
####################
# Form alterations #
####################

/**
 * Implements hook_form_alter().
 */
function sebd7tweaks_form_alter(&$form, &$form_state, $form_id) {
  if ($form['#theme'] == 'confirm_form' && sebd7tweaks_feature('system.enhance_confirm_form') && $GLOBALS['theme'] == variable_get('admin_theme')) {
    drupal_set_message(drupal_get_title(), 'warning');
  }
  // Revisioning doesn't take user with "bypass node access" in consideration...
  if (module_exists('revisioning') && sebd7tweaks_feature('revisioning.tweak_form_alter')) {
    // The fact that $form['revision_moderation'] exists ensure other checks are ok
    $form = sebd7tweaks('revisioning', 'form_alter', $form);
  }
}

##### Relative to BLOCK

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_block_admin_display_form_alter(&$form, &$form_state, $form_id) {
  // Attach a callback to hide some block after the form has been build
  $form = sebd7tweaks('fapi', 'add_callback', $form, 'after_build', 'hide_blocks');
  // Attach a callback to disable those blocks before block_admin_display_form_submit() will be performed
  array_unshift($form['#submit'], 'sebd7tweaks_hide_blocks_submit');
}

/**
 * Implements block hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_block_add_block_form_alter(&$form, &$form_state, $form_id) {
  sebd7tweaks_form_block_admin_configure_alter($form, $form_state, $form_id);
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_block_admin_configure_alter(&$form, &$form_state, $form_id) {
  if (sebd7tweaks_feature('block.enhance_forms')) {
    // Place region selector to the top
    $form['regions']['#weight'] = -99;
    // Dispatch fields + create a custom tab
    $form = sebd7tweaks('fapi', 'dispatch_elements', $form, array(), array('regions', 'settings'),
                    array('visibility' => array($form['visibility_title']['#title'], 'visibility')));
    // No more needed (a dedicated tab replace it)
    unset($form['visibility_title']);
  }
  // Simplify region selector
  if (sebd7tweaks_feature('block.simplify_regions') && ($themes = element_children($form['regions'])) && count($themes) == 1) {
    $form['regions'][$themes[0]]['#title'] = t('Region');
    unset($form['regions']['#type'], $form['regions']['#title']);
  }
  // Call derived callbacks related to the module of this block
  $function = "sebd7tweaks_block_form_alter__{$form['module']['#value']}";

  if (function_exists($function)) {
    $function($form);
  }
}

/**
 * Derived function of hook_form_FORM_ID_alter().
 * @see sebd7tweaks_form_block_admin_configure_alter()
 */
function sebd7tweaks_block_form_alter__follow(&$form) {
  if ($form['delta']['#value'] == 'site' && sebd7tweaks_feature('follow.simplify_block')) {
    // Could be done w/o using element_children (by directly calling the keys) but BO so no performances pbs here...
    foreach (element_children($form['settings']) as $key) {
      // ... this ensure all "follow" module options will be hidden if the module evolve.
      if (strpos($key, 'follow_') === 0) {
        $form['settings'][$key]['#access'] = FALSE;
      }
    }
  }
}

/**
 * Derived function of hook_form_FORM_ID_alter().
 * @see sebd7tweaks_form_block_admin_configure_alter()
 */
function sebd7tweaks_block_form_alter__revisioning(&$form) {
  if ($form['delta']['#value'] == 'pending') {
    if (sebd7tweaks_feature('revisioning.locked_block_path')) {
      $form['settings']['revisioning_content_summary_page']['#access'] = user_access('administer site configuration');
    }
    // @see sebd7tweaks_block_view_revisioning_pending_alter()
    if (sebd7tweaks_feature('revisioning.more_logic_block')) {
      unset($form['settings']['revisioning_block_num_pending']['#description']);
      unset($form['settings']['revisioning_content_summary_page']['#description']);
      $form['settings']['revisioning_content_summary_page']['#title'] = t('Override path');
    }
  }
}

##### Relative to CONTACT

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_contact_site_form_alter(&$form, &$form_state, $form_id) {
  if (isset($form_state['build_info']['files']['menu'])) {
    array_unshift($form['#theme'], 'sebd7tweaks_contact_full_page_form');
  }

  if (sebd7tweaks_feature('contact.enhance_global_form')) {
    // Authenticated user: We already have those informations
    if ($GLOBALS['user']->uid) {
      $form['name']['#access'] = $form['mail']['#access'] = FALSE;
    }
    // Not very usefull (rarely used by users)
    $form['copy']['#access'] = FALSE;
    $form['message']['#title_display'] = 'invisible';
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_contact_personal_form_alter(&$form, &$form_state, $form_id) {
  if (sebd7tweaks_feature('contact.enhance_perso_form')) {
    $form['copy']['#access'] = FALSE;
    $form['to']['#access']   = FALSE;
  }
}

##### Relative to FEATURES

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_features_admin_form_alter(&$form, &$form_state, $form_id) {
  // If sebd7tweaks's option is enabled, check the example feature is listed and remove it from the display if real features exist
  if ( sebd7tweaks_feature('features.remove_example') && ($example = 'date_migrate_example') // in a var for code flexibility
    && isset($form['features']['#group']) && $form['features']['#group'] == 'packages' && isset($form['features']['status'][$example])
  ) {
    // Several features in the package "Features", we can hide the example
    if (count(element_children($form['features']['status'])) > 1) {
      // All FAPI element relative to the example will be hidden
      foreach (element_children($form['features']) as $key) {
        if (isset($form['features'][$key][$example])) {
          $form['features'][$key][$example]['#access'] = FALSE;
        }
      }
      // We have to suppress this checkbox to not generate a row
      unset($form['features']['status'][$example]);
    }
    // The example is the only feature in "Features", we have to check if other packages exist
    else {
      $packages = 0;
      // Count features packages
      foreach (element_children($form) as $key) {
        if (isset($form[$key]['#group']) && $form[$key]['#group'] == 'packages') {
          $packages++;
        }
      }
      // Several packages, so "Features" one is not needed
      if ($packages > 1) {
        $form['features']['#access'] = FALSE;
      }
    }
  }
}

##### Hooks relative to FOLLOW

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_follow_links_form_alter(&$form, &$form_state, $form_id) {
  if (sebd7tweaks_feature('follow.reorder_links')) {
    $disabled = array();

    foreach (element_children($form['follow_links']) as $key) {
      if (!isset($form['follow_links'][$key]['weight'])) {
        // This ensure a uniq key even if there is not a big risk of collisions + force lower because SORT_FLAG_CASE is only for php 5.4+
        $disabled[strtolower($form['follow_links'][$key]['name']['#markup']) .'___'. $key] = $form['follow_links'][$key];
        // Follow use a theme function, which doesn't take care about weight, so remove the row: It will be readd later
        unset($form['follow_links'][$key]);
      }
    }

    if (count($disabled) > 0) {
      ksort($disabled);
      // Reattach them
      foreach ($disabled as $key => $element) {
        $form['follow_links'][array_pop(explode('___', $key))] = $element;
      }
    }
  }
}

##### Hooks relative to LINKIT

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_linkit_dashboard_form_alter(&$form, &$form_state, $form_id) {
  if (sebd7tweaks_feature('linkit.enhance_dashboard')) {
    // No need to have a collapsed fieldset if there is only one field inside...
    if (count(element_children($form['linkit_attributes'])) == 1) {
      $form['linkit_attributes']['#collapsible'] = FALSE;
    }
    // And as url is automatically filled, move this attribute before it
    $form['linkit_attributes']['#weight'] = -2;
    // Prevent user of the importance of the link title...
    if (isset($form['linkit_attributes']['linkit_title'])) {
      $form['linkit_attributes']['linkit_title'] = array(
        '#size'        => $form['linkit_path']['#size'],
        '#title'       => t('Title of the link'),
        '#description' => t('Strongly recommanded for Search Engine Optimization'),
      ) + $form['linkit_attributes']['linkit_title'];
    }
    // Useless description which take some place in this form, so...
    unset($form['linkit_search']['#description']);
  }
}

##### Relative to LOCALE

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_locale_translate_edit_form_alter(&$form, &$form_state, $form_id) {
  if (sebd7tweaks_feature('locale.dynamic_parts_warning')) {
    $original = $form['original']['#markup'];

    preg_match_all('/![a-z_]+|%[a-z_]+|@[a-z_]+/i', $original, $matches);

    if (count($matches[0]) > 0) {
      foreach ($matches[0] as &$match) {
        $match = preg_replace('/[(),-]/', '', $match);
      }

      $warning = '<div class="messages warning">'.
          '<h2>'. t('Beware') .'</h2><br />'.
          t('This text contains the following dynamic parts:') .'<br />'.
          theme('item_list', array('items' => $matches[0])) .'<br />'.
          t('Be sure to include them in the translation!')
        .'</div>';

      $form['warning'] = array('#type' => 'markup', '#weight' => 1, '#markup' => $warning);
      $form['dynamic_parts'] = array('#type' => 'value', '#value' => $matches[0]);

      $form = sebd7tweaks('fapi', 'add_callback', $form, 'validate', 'locale_dynamic_part');
    }
  }
}

##### Relative to MENU

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_menu_overview_form_alter(&$form, &$form_state, $form_id) {
  if (sebd7tweaks_feature('menu.enhance_user_items')) {
    foreach (element_children($form) as $key) {
      if (strpos($key, 'mlid:') === 0 && isset($form[$key]['#item']['link_path']) && strpos($form[$key]['#item']['link_path'], 'user/') === 0) {
        switch (substr($form[$key]['#item']['link_path'], 5)) {
          case 'login':
          case 'password':
          case 'register':
            $form[$key]['title']['#markup'] .= ' ('. t('anonymous user') .')';
            break;

          case 'logout':
            $form[$key]['title']['#markup'] .= ' ('. t('logged in users only') .')';
            break;
        }
      }
    }
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_menu_edit_item_alter(&$form, &$form_state, $form_id) {
  if (sebd7tweaks_feature('menu.hide_structure')) {
    foreach (array('enabled', 'parent', 'weight') as $key) {
      $form[$key]['#access'] = FALSE;
    }
  }

  if (sebd7tweaks_feature('menu.simplify_path')) {
    array_unshift($form['#submit'], 'sebd7tweaks_simplify_item_path_submit');
  }
}

##### Relative to NODE

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_node_admin_content_alter(&$form, &$form_state, $form_id) {
  if ( variable_get('language_count', 1) == 1 && sebd7tweaks_feature('locale.hide_language_filter')
    && isset($form['filter']['filters']['status']['filters']['language'])
  ) {
    $form['filter']['filters']['status']['filters']['language']['#access'] = FALSE;
  }

  if ( ($hidden_operations = variable_get('sebd7tweaks_hidden_node_operations', array()))
    && count($hidden_operations) > 0 && isset($form['admin']['options']['operation']['#options'])
  ) {
    foreach ($hidden_operations as $operation) {
      unset($form['admin']['options']['operation']['#options'][$operation]);
    }
  }
}

/**
 * Implements hook_form_BASE_FORM_ID_alter().
 */
function sebd7tweaks_form_node_form_alter(&$form, &$form_state, $form_id) {
  if (sebd7tweaks_feature('node.forms_title')) {
    drupal_set_title($form['#node']->title);
  }

  if (sebd7tweaks_feature('node.enhance_forms')) {
    $advanceds = array('author', 'book', 'comment_settings', 'options', 'path', 'revision_information');

    if (module_exists('xmlsitemap')) {
      $advanceds[] = 'xmlsitemap';
    }

    $form = sebd7tweaks('fapi', 'dispatch_elements', $form, $advanceds);
    // Change weights of actions
    foreach (array('delete', 'preview', 'submit') as $weight => $key) {
      if (isset($form['actions'][$key])) {
        $form['actions'][$key]['#weight'] = $weight;
      }
    }
    // Attach a custom pre render callback to display only required tabs
    $form = sebd7tweaks_API_fapi_add_callback($form, 'pre_render', 'filter_node_tabs');
  }

  if (module_exists('book')) {
    if (book_type_is_allowed($form['#node']->type)) {
      if (sebd7tweaks_feature('book.required_bid')) {
        $form['#validate'][] = '_sebd7tweaks_book_bid_validate';
      }

      if (!@$form['#node']->nid && sebd7tweaks_feature('book.new_to_end')) {
        $form['book']['weight']['#default_value'] = 99;
      }
    }
    else {
      // Should be by default: Only for node types allowed for books
      $form['book']['#access'] = FALSE;
    }
  }
}

##### Relative to SEARCH

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_search_form_alter(&$form, &$form_state, $form_id) {
  if ($form['module']['#value'] == 'node' && sebd7tweaks_feature('search.enhance_node_search_form')) {
    $form = sebd7tweaks('search', 'parse_search_terms', $form);
  }
}

##### Relative to TAXONOMY

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_taxonomy_form_term_alter(&$form, &$form_state, $form_id) {
  if (in_array($form['#vocabulary']->machine_name, variable_get('sebd7tweaks_disabled_term_description', array()))) {
    $form['description']['#access'] = FALSE;
  }

  if (!isset($form_state['confirm_delete']) && sebd7tweaks_feature('taxonomy.enhance_forms')) {
    $advanceds = array('path', 'relations');

    if (module_exists('xmlsitemap')) {
      $advanceds[] = 'xmlsitemap';
    }

    $form = sebd7tweaks('fapi', 'dispatch_elements', $form, $advanceds);
  }
}

##### Relative to USER

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_user_admin_settings_alter(&$form, &$form_state, $form_id) {
  if (sebd7tweaks_feature('user.cancel_account_link')) {
    $form['email_cancel_account_request'] = array(
      '#group' => 'email', '#type' => 'fieldset', '#title' => t('Request for account deletion'), '#collapsible' => TRUE, '#collapsed' => TRUE,
    );
    $defaults = sebd7tweaks('mail', 'get_texts', 'sebd7tweaks', 'cancel_account_request');

    foreach (
      array(
        'subject' => array('#type' => 'textfield', '#maxlength' => 180),
        'body'    => array('#type' => 'textarea',  '#rows'      => 15),
      ) as $key => $infos
    ) {
      $form['email_cancel_account_request']["sebd7tweaks_mail_cancel_account_request_$key"]
        = $infos + array('#title' => t(ucfirst($key)), '#default_value' => $defaults[$key]);
    }
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_user_admin_account_alter(&$form, &$form_state, $form_id) {
  // I really don't understand why it's not by default...
  if (module_exists('pathauto') && !user_access('administer pathauto')) {
    unset($form['options']['operation']['#options']['pathauto_update_alias']);
    // In case of...
    $form['#validate'][] = 'sebd7tweaks_form_user_admin_account_validate';
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_user_filter_form_alter(&$form, &$form_state, $form_id) {
  if (sebd7tweaks_feature('user.hide_access_filter')) {
    $form['filters']['status']['filters']['permission']['#access'] = user_access('administer permissions');
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_user_register_form_alter(&$form, &$form_state, $form_id) {
  if ( module_exists('logintoboggan') && variable_get('logintoboggan_confirm_email_at_registration', FALSE)
    && sebd7tweaks_feature('logintoboggan.enhance_double_mail')
  ) {
    // Deferred to after build: Needed to get the correct ID
    $form = sebd7tweaks('fapi', 'add_callback', $form, 'after_build', 'enhance_double_mail');
  }
  // Should be a module options and activated by default!
  if (module_exists('user_field_privacy') && sebd7tweaks_feature('user_field_privacy.private_by_default')) {
    foreach (element_children($form) as $key) {
      if (isset($form[$key]['user_field_privacy'])) {
        $form[$key]['user_field_privacy']['#default_value'] = TRUE;
      }
    }
  }
  // Place administration fields in a custom wrapper
  _sebd7tweaks_separate_administration_fields($form);
  // Add an after build callback to prevent forms to be autocompleted (limit chain registering)
  if (sebd7tweaks_feature('user.no_auto_register')) {
    $form = sebd7tweaks('fapi', 'add_callback', $form, 'after_build', 'disable_autocomplete');
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_user_profile_form_alter(&$form, &$form_state, $form_id) {
  // Not present on not confirmed account...
  if (sebd7tweaks_feature('user.enhance_pass_fields') && isset($form['account']['current_pass'])) {
    // Deferred to after build: Needed to get the correct ID
    $form = sebd7tweaks('fapi', 'add_callback', $form, 'after_build', 'enhance_double_pass');
  }
  // Useless in a great majority of cases...
  if (sebd7tweaks_feature('overlay.lock_choice') && isset($form['overlay_control'])) {
    $form['overlay_control']['#access'] = FALSE;
  }
  // Add our custom condition on this field
  if ($form['#user_category'] == 'account' && module_exists('contact')) {
    $form['contact']['#access'] = (sebd7tweaks_feature('contact.hide_personal_choice') && user_access('administer contact forms'))?
                                    variable_get('contact_default_status', TRUE): user_access('manage own contact form');
  }
  // Place administration fields in a custom wrapper
  _sebd7tweaks_separate_administration_fields($form);
  // Add a "cancel account" link to edit account form
  if (@$form['actions']['cancel']['#access'] !== FALSE) {
    $form['actions']['cancel']['#weight'] = -99;
  }
  if (sebd7tweaks_feature('user.cancel_account_link') && $form['#user']->uid == $GLOBALS['user']->uid) {
    $form['actions']['cancel_account']
      = array('#type' => 'markup', '#markup' => l(t('Request for account deletion'), 'user/cancel-request'), '#weight' => -99);
  }
}

##### Hooks relative to module WEBFORM

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_webform_components_form_alter(&$form, &$form_state, $form_id) {
  if (sebd7tweaks_feature('webform.simplify_forms')) {
    drupal_add_js(drupal_get_path('module', 'sebd7tweaks') .'/js/webform-simplify-listing-table.js');
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_webform_component_edit_form_alter(&$form, &$form_state, $form_id) {
  if (sebd7tweaks_feature('webform.simplify_forms')) {
    $form = sebd7tweaks('fapi', 'remove_description', $form);
    // Those fields can be set from the components list, so no need to have them here to simplify this form
    $form['position']['#access'] = $form['validation']['#access'] = FALSE;
    // Wording...
    $form['name']['#title'] = t('Title'); unset($form['name']['#description']);
    // Common advanced settings unwanted in lots of cases
    $form['value']['#access'] = $form['form_key']['#access'] = FALSE;
    // Per type modifications...
    switch ($form['type']['#value']) {
      case 'select':
        $form['extra']['options_source']['#access'] = $form['display']['#collapsible'] = $form['display']['#collapsed'] = FALSE;
        $form['display']['#title'] = t('Options');

        foreach (element_children($form['display']) as $key) {
          $form['display'][$key]['#access'] = ($key == 'aslist');
        }
        // Attach a callback to manage form_key automatically
        $form = sebd7tweaks('fapi', 'add_callback', $form, 'after_build', 'webform_component_select');
        // Wording to explain the logic, embedding the lazy format we introduce [ @see sebd7tweaks_webform_component_validate() ].
        $form['extra']['items']['#description'] = t('Each possible value should be in the format "key|value". Example: "y|Yes, we can!". '.
          'If you only give the value, the key will be dynamically created. Keys must be less than 128 characters.');
        // Attach a callback to manage select items with "lazy" format
        $form['extra']['items']['#element_validate'] = array('sebd7tweaks_webform_select_element_validate');
        break;

      case 'email':
        $form['user_email']['#access']        = FALSE;
        $form['user_email']['#default_value'] = TRUE;
      case 'textarea':
      case 'textfield':
        $form['display']['#access'] = FALSE;
        break;
    }
    // Attach a callback to manage form_key automatically
    $form = sebd7tweaks('fapi', 'add_callback', $form, 'validate', 'webform_component', TRUE);
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function sebd7tweaks_form_webform_configure_form_alter(&$form, &$form_state, $form_id) {
  if (sebd7tweaks_feature('webform.simplify_forms')) {
    $form = sebd7tweaks('fapi', 'remove_description', $form);
  }

  if ($default = variable_get('sebd7tweaks_easier_webform', FALSE)) {
    $all_hidden = TRUE;

    foreach (array('block', 'teaser', 'allow_draft', 'auto_save', 'submit_notice', 'submit_text') as $field) {
      if (@$default["hide_settings_$field"]) {
        $form['advanced'][$field]['#access'] = FALSE;
      }
      else {
        $all_hidden = FALSE;
      }
    }

    if ($all_hidden) {
      $form['advanced']['#access'] = FALSE;
    }

    if (@$default['hide_access_settings']) {
      $form['role_control']['#access'] = FALSE;
    }
  }
}

####################
# Custom callbacks #
####################

/**
 * Plane a JS replacement for the file input fields allowing better theming.
 */
function sebd7tweaks_preprocess_file(&$vars) {
  $path = drupal_get_path('module', 'sebd7tweaks');
  drupal_add_css("$path/css/enhance-file-fields.css");
  drupal_add_js("$path/js/enhance-file-fields.js");
  drupal_add_js(array('sebd7tweaks' => array('file_fields' => array($vars['element']['#id'] => uniqid('sebd7tweaks-')))), 'setting');
}

/**
 * Attach HTML5 attributes required and placeholder.
 */
function sebd7tweaks_preprocess_password(&$vars) {
  _sebd7tweaks_preprocess_text_html5($vars);
}

/**
 * Attach HTML5 attributes required and placeholder.
 */
function sebd7tweaks_preprocess_textfield(&$vars) {
  _sebd7tweaks_preprocess_text_html5($vars);
}

/**
 * Attach HTML5 attributes required and placeholder.
 */
function sebd7tweaks_preprocess_textarea(&$vars) {
  _sebd7tweaks_preprocess_text_html5($vars);
}

/**
 * Tweak filter format selector.
 * @see sebd7tweaks_element_info_alter()
 */
function sebd7tweaks_filter_process_format($element) {
  if (sebd7tweaks_feature('filter.simplify_filter')) {
    unset($element['format']['#type'], $element['format']['help'], $element['format']['guidelines']);
    $element['#attached']['css'][] = drupal_get_path('module', 'sebd7tweaks') .'/css/enhance-filter-selectors.css';
  }

  return $element;
}

/**
 * Tweak password confirm form element.
 * @see sebd7tweaks_element_info_alter()
 */
function sebd7tweaks_enhance_password_confirm_process($element) {
  $element['#title']            = t('Password');
  $element['#theme_wrappers']   = array('fieldset');
  $element['#attached']['js'][] = drupal_get_path('module', 'sebd7tweaks') .'/js/enhance-password.js';
  // Required to not generate "Array" in the display
  $element = sebd7tweaks('fapi', 'add_callback', $element, 'pre_render', 'enhance_password_confirm');
  // Attach an after build callback to affect the display
  return sebd7tweaks('fapi', 'add_callback', $element, 'after_build', 'enhance_password_confirm');
}

/**
 * Tweak password confirm form element.
 * @see sebd7tweaks_enhance_password_confirm_process()
 */
function sebd7tweaks_enhance_password_confirm_after_build($element, &$form_state) {
  $element['pass2'] += array('#states' => array('visible' => array('#'. $element['pass1']['#id'] => array('!value' => ''))));

  return $element;
}

/**
 * Tweak password confirm form element.
 * @see sebd7tweaks_enhance_password_confirm_process()
 */
function sebd7tweaks_enhance_password_confirm_pre_render($element) {
  // Useless for fieldsets (display "Array" otherwise)
  unset($element['#value']);

  return $element;
}

/**
 * FAPI pre render callback: Dispatch some elements into a General tab
 */
function sebd7tweaks_dispatch_elements_pre_render($form) {
  return sebd7tweaks('fapi', 'dispatch_elements_pre_render', $form);
}

/**
 * FAPI pre render callback: Filter node tabs to only display not empty ones
 */
function sebd7tweaks_filter_node_tabs_pre_render($form) {
  $tabs = array();

  foreach (element_children($form) as $key) {
    $matches = array();

    if (isset($form[$key]['#group']) && preg_match('/sebd7tweaks_(.*)_content/', $form[$key]['#group'], $matches)) {
      $group = "sebd7tweaks_tab_{$matches[1]}";

      if (!isset($tabs[$group])) {
        $tabs[$group] = FALSE;
      }

      $tabs[$group] |= _sebd7tweaks_can_access_children($form[$key]);
    }
  }

  foreach ($tabs as $key => $access) {
    $form[$key]['#access'] = $access;
  }

  if (count(array_filter($tabs)) == 0) {
    $form['sebd7tweaks_tabs']['#attached']['js'][] = drupal_get_path('module', 'sebd7tweaks') .'/js/single-horizontal-panes.js';
  }

  return $form;
}

/**
 * FAPI validate callback: Ensure the operation is permitted.
 * @see sebd7tweaks_form_user_admin_account_alter() only applyed for user who don't have the permission to access pathauto
 */
function sebd7tweaks_form_user_admin_account_validate($form, &$form_state) {
  if ($form_state['values']['operation'] == 'pathauto_update_alias') {
    form_set_error('operation', t('Access denied'));
  }
}

/**
 * FAPI after build callback: Enhance the double pass fields.
 */
function sebd7tweaks_enhance_double_mail_after_build($form, &$form_state) {
  if (isset($form['account']['mail']) && isset($form['account']['conf_mail'])) {
    $form['account']['conf_mail']['#states']['visible'] = array('#'. $form['account']['mail']['#id'] => array('!value' => ''));
  }

  return $form;
}

/**
 * FAPI after build callback: Enhance the double pass fields.
 */
function sebd7tweaks_enhance_double_pass_after_build($form, &$form_state) {
  $pass_visibility = array('#'. $form['account']['current_pass']['#id'] => array('!value' => ''));
  // No need to let the user edit the mail or show the change password field if he has not fill his current pass
  $form['account']['mail'] += array('#states' => array('enabled' => $pass_visibility));
  $form['account']['pass'] += array('#states' => array('visible' => $pass_visibility));

  return $form;
}

/**
 * FAPI after build callback: Enhance the double pass fields.
 */
function sebd7tweaks_disable_autocomplete_after_build($form, &$form_state) {
  _sebd7tweaks_disable_autocomplete($form);
  return $form;
}

/**
 * FAPI after build callback: Hide some blocks.
 */
function sebd7tweaks_hide_blocks_after_build($form, &$form_state) {
  $hidden_blocks = variable_get('sebd7tweaks_hidden_blocks', array());

  if (count($hidden_blocks) > 0) {
    foreach (array_keys($hidden_blocks) as $key) {
      if (isset($form['blocks'][$key])) {
        $form['hidden_blocks'][$key] = array('#access' => FALSE) + $form['blocks'][$key];
        unset($form['blocks'][$key]);
      }
    }
  }

  return $form;
}

/**
 * FAPI submit callback: Disable hidden blocks.
 */
function sebd7tweaks_hide_blocks_submit($form, &$form_state) {
  if (isset($form['hidden_blocks']) && is_array($form['hidden_blocks'])) {
    foreach (element_children($form['hidden_blocks']) as $key) {
      $form_state['values']['blocks'][$key]['region'] = BLOCK_REGION_NONE;
    }
  }
}

/**
 * FAPI pre render callback: Move administration fields to a dedicated fieldset.
 */
function sebd7tweaks_separate_administration_fields_pre_render($form) {
  // Need to pass by a recursive function because elements to move can be in a fieldgroup...
  _sebd7tweaks_separate_administration_fields_proceed($form);
  // Send the modified form
  return $form;
}

/**
 * FAPI element process callback: Hide "show date" checkbox for date fields
 */
function sebd7tweaks_date_combo_element_process($element, &$form_state, $form) {
  $element['show_todate']['#access'] = FALSE;

  return $element;
}

/**
 * FAPI element after build callback: Tweak google_map_field fields
 */
function sebd7tweaks_google_map_field_after_build($form, &$form_state) {
  $settings = array();
  $module   = drupal_get_path('module', 'sebd7tweaks');
  // Populate the JS settings array
  foreach (element_children($form) as $key) {
    $settings[$key] = $form[$key]['#id'];
  }
  // Prevent error message on hidden fields
  $form['name']['#required'] = $form['lat']['#required'];
  $form['lat']['#required']  = $form['lon']['#required'] = FALSE;
  // Readd googleapi javascript here fix the bug which break the field UI when there are errors on validation or preview
  $form['#attached']['js'][]  = array('data' => array('sebd7tweaks' => array('gmaps' => array($form['#id'] => $settings))), 'type' => 'setting');
  $form['#attached']['js'][]  = "$module/js/enhance-gmap-widget.js";
  $form['#attached']['css'][] = "$module/css/enhance-gmap-widget.css";

  return $form;
}

/**
 * FAPI submit callback: Simplify path.
 */
function sebd7tweaks_simplify_item_path_submit($form, &$form_state) {
  $path = &$form_state['values']['link_path'];
  $path = trim($path);

  if ($path != '<front>') {
    $base = trim(url('', array('absolute' => TRUE)), '/');
    $path = str_replace($base, '', $path);
    $path = trim($path, '/');
    $path = (empty($path) || $path == variable_get('site_frontpage', '<front>'))? '<front>': drupal_get_normal_path($path);
  }
}

/**
 * FAPI validate callback: Ensure dynamic parts are keeped.
 */
function sebd7tweaks_locale_dynamic_part_validate($form, &$form_state) {
  if (isset($form_state['values']['dynamic_parts']) && isset($form_state['values']['translations'])
    && ($dynamics     = $form_state['values']['dynamic_parts']) && is_array($dynamics)     && count($dynamics)     > 0
    && ($translations = $form_state['values']['translations'])  && is_array($translations) && count($translations) > 0
  ) {
    $error = FALSE;

    foreach ($translations as $translation) {
      if ($error) {
        break;
      }

      foreach ($dynamics as $dynamic) {
        if (!$error && strpos($translation, $dynamic) === FALSE) {
          $error = TRUE;
          break;
        }
      }
    }

    if ($error) {
      form_set_error('translations', t('Dynamic part(s) missing!'));
    }
  }
}

/**
 * FAPI validate callback: Ensure a book ID has been selected.
 */
function _sebd7tweaks_book_bid_validate($form, &$form_state) {
  if (!$form_state['values']['book']['bid']) {
    form_set_error('bid', t('!name field is required.', array('!name' => t('Book'))));
  }
}

/**
 * FAPI validate callback: Automatize the webform component key and other stuffs.
 */
function sebd7tweaks_webform_component_validate($form, &$form_state) {
  $form_state['values']['form_key'] = _sebd7tweaks_string_to_machine_name($form_state['values']['name']);
  // Fix a bug when creating a component not required...
  $form_state['values']['mandatory'] = (int) @$form_state['values']['mandatory'];
}

/**
 * FAPI after build callback: Move webform select component "multiple" option into the "display" fieldset.
 */
function sebd7tweaks_webform_component_select_after_build($form, &$form_state) {
  $form['display']['multiple'] = $form['extra']['multiple'];
  unset($form['extra']['multiple']);

  return $form;
}

/**
 * FAPI validate callback: Automatize the webform component key and other stuffs.
 */
function sebd7tweaks_webform_select_element_validate($element, &$form_state) {
  if (!empty($element['#value'])) {
    $options = explode("\n", trim($element['#value']));

    foreach ($options as &$option) {
      if (!preg_match('/^([^|]*)\|(.*)$/', $option)) {
        $option = _sebd7tweaks_string_to_machine_name($option, 128) ."|$option";
      }
    }

    $options = implode("\n", $options);
    form_set_value($element, $options, $form_state);
    // Default validation... (we pass the modified element)
    $element['#value'] = $options;
    _webform_edit_validate_select($element, $form_state);
  }
}

####################
# Helper functions #
####################

/**
 * Helper function: Return a string usable for machine names, based of the given string.
 */
function _sebd7tweaks_string_to_machine_name($string, $length = 0) {
  $string = trim(preg_replace('@[^0-9a-z]@', '_', strtolower(transliteration_get($string))), '_');

  if ($length) {
    $string = substr($string, 0, $length);
  }

  return $string;
}

/**
 * Helper function: Parse a FAPI element to add "autocomplete" attribute to "off".
 */
function _sebd7tweaks_disable_autocomplete(&$element) {
  if (is_array($element)) {
    if (@$element['#type'] == 'textfield') {
      $element['#attributes']['autocomplete'] = 'off';
    }
    // Recurse
    foreach (element_children($element) as $key) {
      _sebd7tweaks_disable_autocomplete($element[$key]);
    }
  }
}

/**
 * Helper function: Prepare the form to receive an administrative fieldset.
 */
function _sebd7tweaks_separate_administration_fields(&$form) {
  if (user_access('administer users') && isset($form['account'])) {
    $form['sebd7tweaks_administration_wrapper']
      = array('#type' => 'fieldset', '#title' => t('Administration'), '#collapsible' => TRUE, '#weight' => -99);
    // Required in case of the form use fieldgroups
    if (isset($form['#group_children']['account'])) {
      $form['#group_children']['sebd7tweaks_administration_wrapper'] = $form['#group_children']['account'];
    }
    // Add a pre render callback to move administrative fields in the dedicated fieldset
    $form = sebd7tweaks('fapi', 'add_callback', $form, 'pre_render', 'separate_administration_fields');
  }
}

/**
 * Helper function: Move some fields in the administrative fieldset.
 */
function _sebd7tweaks_separate_administration_fields_proceed(&$element) {
  if (is_array($element)) {
    if (isset($element['sebd7tweaks_administration_wrapper']) && isset($element['account'])) {
      // Note that "roles_change" is only available if "role_delegation" module is enabled.
      foreach (array('notify', 'roles', 'roles_change', 'status') as $key) {
        if (isset($element['account'][$key])) {
          $element['sebd7tweaks_administration_wrapper'][$key] = array('#description' => '') + $element['account'][$key];
          unset($element['account'][$key]);
        }
      }
      // Used by this function, in recursive mode: Ensure to stop searching where the root element has been found
      return TRUE;
    }
    else {
      foreach (element_children($element) as $key) {
        if (_sebd7tweaks_separate_administration_fields_proceed($element[$key])) {
          return TRUE;
        }
      }
    }
  }
  // Used by this function, in recursive mode: Ensure to stop searching where the root element has been found
  return FALSE;
}

/**
 * Helper function: Attach HTML5 attributes required and placeholder.
 */
function _sebd7tweaks_preprocess_text_html5(&$vars) {
  $required = (@$vars['element']['#required'] === TRUE);

  if ($required) {
    //FIXME: Sometime a mysterious field is designed to be filled... So, for now, I disable this option
//     $vars['element']['#attributes']['required'] = 'required';
  }

  if (!empty($vars['element']['#title']) && empty($vars['element']['#attributes']['placeholder'])) {
    $vars['element']['#attributes']['placeholder'] = strip_tags($vars['element']['#title'] . ($required? ' *': ''));
  }
}

/**
 * Helper function: Check access to a sebd7tweaks node tab
 */
function _sebd7tweaks_can_access_children($element) {
  $access = (@$element['#access'] !== FALSE);

  if ($access && ($children = element_children($element)) && count($children) > 0) {
    foreach ($children as $key) {
      $access |= _sebd7tweaks_can_access_children($element[$key]);
    }
  }

  return $access;
}

