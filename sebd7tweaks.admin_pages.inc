<?php
#####################
# Forms definitions #
#####################

/**
 * FAPI: Form to administrate the module settings.
 */
function sebd7tweaks_settings_form($form, &$form_state) {
  $form = $list = array();
  // List available plugins
  $plugins = drupal_system_listing('/\.inc$/', drupal_get_path('module', 'sebd7tweaks') .'/plugins', 'name', 0);
  unset($plugins['fapi']); // Even if "fapi" project exists, this plufin (wrongly named) is only a helper for Form API
  // Attach usable plugins options to the form
  foreach (array_filter(module_list(), 'module_exists') as $module) {
    _sebd7tweaks_settings_form_module_features($module, $form, $list);
    unset($plugins[$module]);
  }
  // This settings page is for advanced users, so let them manage cache clear and others things if needed
  $form['#prefix'] = '<div class="messages warning">'. t('Some options require a cache clear to take effect.') .'</div>';
  // Sort fieldsets by module name to easily find it when checking this page
  ksort($form);
  // Tell the user other options can be available by activate some other modules
  if (count($plugins) > 0) {
    array_walk($plugins, create_function('&$i', '$i = l($i->name, "http://drupal.org/project/{$i->name}");'));
    $form['disabled'] = array('#type' => 'fieldset', '#title' => t('Disabled options'), '#collapsible' => TRUE, '#collapsed' => TRUE, '#weight' => -99,
      'plugins' => array('#type' => 'markup', '#theme' => 'item_list', '#title' => t('When activated, the following modules will provide other options:'), '#items' => $plugins)
    );
  }
  // Get default values
  $enabled = variable_get('sebd7tweaks_features', FALSE);
  // Beware, an empty array (every features disabled) could be considered as FALSE, so "==="
  if ($enabled === FALSE) {
    $enabled = drupal_map_assoc(array_keys($list));
  }
  // Create the features list FAPI element
  $form['sebd7tweaks_features'] = array('#type' => 'checkboxes', '#options' => $list, '#default_value' => $enabled);
  // Attach a after build callback to generate a custom display
  $form = sebd7tweaks('fapi', 'add_callback', $form, 'after_build', 'settings_form');
  // Attach a submit callback to reduce variables in DB
  $form = sebd7tweaks('fapi', 'add_callback', $form, 'submit', 'settings_form');
  // Turn it to a settings form!
  return system_settings_form($form);
}

/**
 * FAPI after build callback: Display features in fieldsets named with the related module.
 */
function sebd7tweaks_settings_form_after_build($form) {
  foreach (element_children($form['sebd7tweaks_features']) as $key) {
    list($module, $feature) = explode('.', $key);

    $form["sebd7tweaks_features_$module"]['features'][$key] = $form['sebd7tweaks_features'][$key];
    unset($form['sebd7tweaks_features'][$key]);
  }

  return $form;
}

/**
 * FAPI submit callback: Reduce variables in DB.
 */
function sebd7tweaks_settings_form_submit($form, &$form_state) {
  // Get the number of features now, see bellow
  $available_features = count($form_state['values']['sebd7tweaks_features']);
  // Reduce each sebd7tweaks's variables before they are stored in DB
  foreach ($form_state['values'] as $key => &$value) {
    if (strpos($key, 'sebd7tweaks_') === 0 && is_array($value)) {
      $value = array_filter($value);
    }
  }
  // Special case: If all features are activated (default), remove from DB to be sure newly added ones will be activated by default.
  if (count($form_state['values']['sebd7tweaks_features']) == $available_features) {
    variable_del('sebd7tweaks_features');
    // Do not let it in the form state, otherwise the variable will be re-created
    unset($form_state['values']['sebd7tweaks_features']);
  }
}

/**
 * FAPI: Form to administrate basic site informations.
 */
function sebd7tweaks_basic_site_settings_form($form, &$form_state) {
  // Add informations relative to site details
  module_load_include('inc', 'system', 'system.admin');
  $base = drupal_retrieve_form('system_site_information_settings', $form_state);
  drupal_prepare_form('system_site_information_settings', $base, $form_state);
  $form['site_information'] = array('#collapsible' => TRUE) + $base['site_information'];
  unset($form['site_information']['site_slogan']['#description']);
  // Add user e-mail configurations
  module_load_include('inc', 'user', 'user.admin');
  $base = drupal_retrieve_form('user_admin_settings', $form_state);
  drupal_prepare_form('user_admin_settings', $base, $form_state);
  $nots = array();
  // Prepare to hide unneeded mails
  if (sebd7tweaks_feature('user.hide_unneeded_mail')) {
    switch (variable_get('user_register', USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL)) {
      case USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL: $nots = array('email_no_approval_required');                           break;
      case USER_REGISTER_ADMINISTRATORS_ONLY:              $nots = array('email_pending_approval', 'email_no_approval_required'); break;
      case USER_REGISTER_VISITORS:                         $nots = array('email_pending_approval');                               break;
    }
  }
  // Attach all elements relative to mails
  foreach (element_children($base) as $key) {
    if (strpos($key, 'email') === 0 && !in_array($key, $nots)) {
      $form[$key] = $base[$key];
      // To simplify (end users will ask "tokens??? WTF is it?")
      unset($form[$key]['#description']);
    }
  }
  // Return a form which store values in variables
  return system_settings_form($form);
}

/**
 * FAPI: Form to request own account cancelation
 */
function sebd7tweaks_cancel_account_request_form($form, &$form_state) {
  $form['account'] = array('#type' => 'value', '#value' => user_load($GLOBALS['user']->uid));
  $form['comment'] = array('#type' => 'textarea', '#title' => t('Comment'), '#required' => TRUE);
  $form['#theme']  = 'sebd7tweaks_cancel_account_request_form';
  $warning_message = t('Your account will be removed and all account information deleted. All of your content will also be deleted.');

  return confirm_form($form, t('Request for account deletion'), 'user', '<div class="messages warning">'. $warning_message .'</div>');
}

/**
 * FAPI submit callback: Send cancelation request
 */
function sebd7tweaks_cancel_account_request_form_submit($form, &$form_state) {
  $params = array('account' => $form_state['values']['account'], 'comment' => $form_state['values']['comment']);
  $admins = sebd7tweaks('user', 'get_by_permission', 'administer users', 'mail');
  $admins[] = variable_get('site_mail', ini_get('sendmail_from'));

  drupal_mail('sebd7tweaks', 'cancel_account_request', implode(',', array_unique($admins)), language_default(), $params);
  drupal_set_message(
    t('Sent account cancellation request to %name %email.',
    array('%name' => variable_get('site_name', 'Drupal'), '%email' => '('. t('administrator') .')'))
  );
  // Redirect to the homepage
  drupal_goto();
}

/**
 * FAPI: Form to order books.
 */
function sebd7tweaks_book_admin_overview_form($form, &$form_state) {
  $form['#theme'] = 'sebd7tweaks_book_admin_overview_form';
  $form['sebd7tweaks_books_weights'] = array('#tree' => TRUE);
  $default = variable_get('sebd7tweaks_books_weights', array());

  foreach (sebd7tweaks('book', 'get_books') as $book) {
    $form['sebd7tweaks_books_weights'][$book['bid']] = array(
      '#type' => 'weight',
      '#title' => l($book['title'], $book['href'], $book['options']),
      '#description' => l(t('edit order and titles'), 'admin/content/book/'. $book['nid']),
      '#default_value' => @$default[$book['bid']],
    );
  }

  return system_settings_form($form);
}

#####################
# Theming functions #
#####################

/**
 * Theme function: Returns HTML for our system settings form.
 */
function theme_sebd7tweaks_settings_menu_actions($variables) {
  $element = $variables['element'];
  $mapping = array(
    'edit'        => t('Edit'),
    'delete'      => t('Delete'),
    'list'        => t('List items'),
    'item_add'    => t('Add items'),
    'item_edit'   => t('Edit items'),
    'item_reset'  => t('Reset items'),
    'item_delete' => t('Delete items'),
  );
  // Build the table header
  $header = array_values($mapping);
  array_unshift($header, '');
  // Optimize mapping for rows building
  $mapping = array_keys($mapping);
  // Build table rows
  $rows = array();

  foreach ($element['#menus_list'] as $name => $menu) {
    $row = array($menu);

    foreach ($mapping as $key) {
      // The title is not needed in a table cel context
      unset($element["{$name}__$key"]['#title']);
      // Add the checkbox
      $row[] = drupal_render($element["{$name}__$key"]);
    }

    $rows[] = $row;
  }
  // Return the table
  return theme('table', array('header' => $header, 'rows' => $rows));
}

/**
 * Theme function: Return HTML for sebd7tweaks_cancel_account_request_form.
 */
function theme_sebd7tweaks_cancel_account_request_form(&$vars) {
  $infos = array('name', 'mail');

  foreach ($infos as &$info) {
    $info = '<b>'. t(ucfirst($info)) .':</b> '. $vars['form']['account']['#value']->$info;
  }

  return '<h2>'. t('Informations') .'</h2><p>'. implode('<br />', $infos) .'</p>'. drupal_render_children($vars['form']);
}

/**
 * Theme function: Returns HTML for order books form.
 */
function theme_sebd7tweaks_book_admin_overview_form($vars) {
  $header  = array(t('Book'), t('Operations'), t('Order'));
  $table   = array('header' => $header, 'attributes' => array('id' => 'book-overview-form-table'), 'empty' => t('No book content available.'));
  $weights = &$vars['form']['sebd7tweaks_books_weights'];

  foreach (element_children($weights) as $key) {
    $title = $weights[$key]['#title'];
    $links = $weights[$key]['#description'];
    unset($weights[$key]['#title'], $weights[$key]['#description']);
    // For tabledrag
    $weights[$key]['#attributes']['class'] = array('book-weight');
    // Attach the row
    $table['rows'][] = array('data' => array($title, $links, drupal_render($weights[$key])), 'class' => array('draggable'));
  }

  drupal_add_tabledrag($table['attributes']['id'], 'order', 'sibling', 'book-weight');

  return theme('table', $table) . drupal_render_children($vars['form']);
}

####################
# Helper functions #
####################

/**
 * Helper function: Build the features list for the given module and attach it to $form and $list
 */
function _sebd7tweaks_settings_form_module_features($module, &$form, &$list) {
  $features  = sebd7tweaks($module, 'declare_features');
  $additions = sebd7tweaks($module, 'declare_additions');
  // Only build the fieldset for plugins which declare features
  if ($features || $additions) {
    $form["sebd7tweaks_features_$module"] = array('#type' => 'fieldset', '#title' => $module, '#collapsible' => TRUE, '#collapsed' => TRUE);
    // Attach features to the list we will use to build a single FAPI element (checkboxes)
    if (is_array($features)) {
      $form["sebd7tweaks_features_$module"]['features'] = array('#type' => 'fieldset', '#title' => t('Features'));

      foreach ($features as $feature => $description) {
        $list[$module .'.'. $feature] = $description;
      }
    }
    // Attach additions to the fieldset
    if (is_array($additions)) {
      foreach ($additions as $key => $addition) {
        $form["sebd7tweaks_features_$module"]["sebd7tweaks_$key"] = $addition;
      }
    }
  }
}

