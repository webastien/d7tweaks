<?php
/**
 * Helper function: Bring the code of hook_menu().
 */
function _sebd7tweaks_menu() {
  $items = array(
    'admin/config/sebd7tweaks' => array(
      'title'            => 'sebd7tweaks configuration',
      'page callback'    => 'drupal_get_form',
      'page arguments'   => array('sebd7tweaks_settings_form'),
      'access arguments' => array('administer sebd7tweaks settings'),
      'file'             => 'sebd7tweaks.admin_pages.inc',
      'weight'           => -99,
    ),
    'admin/config/basic-site-informations' => array(
      'title'            => "Site informations",
      'page callback'    => 'drupal_get_form',
      'page arguments'   => array('sebd7tweaks_basic_site_settings_form'),
      'access arguments' => array('administer basic site infos'),
      'file'             => 'sebd7tweaks.admin_pages.inc',
      'weight'           => -100,
    ),
  );

  if (module_exists('user_field_privacy')) {
    $items['admin/config/sebd7tweaks/user-field-privacy'] = array(
      'type'             => MENU_CALLBACK,
      'page callback'    => '_sebd7tweaks_API_user_field_privacy_apply_state',
      'access arguments' => array('administer sebd7tweaks settings'),
      'file'             => 'plugins/user_field_privacy.inc',
    );
  }
  // Add a "cancel account" request page
  if (sebd7tweaks_feature('user.cancel_account_link')) {
    $items['user/cancel-request'] = array(
      'type'             => MENU_CALLBACK,
      'page callback'    => 'drupal_get_form',
      'page arguments'   => array('sebd7tweaks_cancel_account_request_form'),
      'access callback'  => 'user_is_logged_in',
      'file'             => 'sebd7tweaks.admin_pages.inc',
    );
  }

  return $items;
}

/**
 * Helper function: Bring the code of hook_menu_alter() from derived functions.
 */
function _sebd7tweaks_menu_alter(&$items) {
  // Note that using module_list() one time is better than module_exists() by module because this uses module_list() to check it!
  foreach (module_list() as $module) {
    $function = "_sebd7tweaks_menu_alter_$module";
    // Not a real problem to check foreach activated module if a dedicated function exists because menu_alter hook is cached.
    if (function_exists($function)) {
      $function($items);
    }
  }
}

/**
 * Derived function of _sebd7tweaks_menu_alter()
 */
function _sebd7tweaks_menu_alter_book(&$items) {
  // Fix something I consider as a bug with book module: Access to outline on node types not configured to use it...
  $items['node/%node/outline']['access callback'] = 'sebd7tweaks_book_outline_access';
  // More logical to redirect to book structure instead of a not user friendly form
  if (sebd7tweaks_feature('book.tweak_outline')) {
    $items['node/%node/outline']
      = array('page callback' => 'sebd7tweaks_book_outline', 'file' => 'sebd7tweaks.pages.inc', 'module' => 'sebd7tweaks') + $items['node/%node/outline'];
  }
  // Replace by our custom page to allow order books
  if (sebd7tweaks_feature('book.ordered_books')) {
    $items['book'] = array('page callback' => 'sebd7tweaks_book_render', 'file' => 'sebd7tweaks.pages.inc', 'module' => 'sebd7tweaks') + $items['book'];

    $items['admin/content/book'] = array(
      'page callback'  => 'drupal_get_form',
      'page arguments' => array('sebd7tweaks_book_admin_overview_form'),
      'file'           => 'sebd7tweaks.admin_pages.inc',
      'module'         => 'sebd7tweaks',
    ) + $items['admin/content/book'];
  }
}

/**
 * Derived function of _sebd7tweaks_menu_alter()
 */
function _sebd7tweaks_menu_alter_block(&$items) {
  $base_path = 'admin/structure/block';
  // Use our custom permission to restrict access on the demo regions links
  foreach (array_keys(list_themes()) as $key) {
    $items["$base_path/demo/$key"]['access callback']  = 'user_access';
    $items["$base_path/demo/$key"]['access arguments'] = array('access demo regions link');
  }
  // Use our custom permission to restrict config on admin theme blocks for users who can manage blocks
  if (($admin_theme = variable_get('admin_theme', FALSE)) && $admin_theme != variable_get('theme_default', 'bartik')) {
    foreach (array("demo/$admin_theme", "list/$admin_theme", "list/$admin_theme/add") as $path) {
      $items["$base_path/$path"]['access callback']  = 'user_access';
      $items["$base_path/$path"]['access arguments'] = array('administer admin blocks');
    }
  }
}

/**
 * Derived function of _sebd7tweaks_menu_alter()
 */
function _sebd7tweaks_menu_alter_devel(&$items) {
  // Hide devel tabs if not needed
  foreach (array('node/%node', 'comment/%comment', 'user/%user', 'taxonomy/term/%taxonomy_term') as $path) {
    $entity = explode('/', $path);

    if (sebd7tweaks_feature('devel.hide_tab_'. array_shift($entity))) {
      $items["$path/devel"]['type'] = MENU_CALLBACK;
    }
  }
}

/**
 * Derived function of _sebd7tweaks_menu_alter()
 */
function _sebd7tweaks_menu_alter_follow(&$items) {
  // Hide user/follo tab if not needed
  if (sebd7tweaks_feature('follow.hide_personal')) {
    $items['user/%/follow']['access callback'] = FALSE;
  }
}

/**
 * Derived function of _sebd7tweaks_menu_alter()
 */
function _sebd7tweaks_menu_alter_google_map_field(&$items) {
  if (sebd7tweaks_feature('google_map_field.restrict_API_config')) {
    $items['admin/config/google-map-field-apikey']['access arguments'] = array('administer sebd7tweaks settings');
  }
}

/**
 * Derived function of _sebd7tweaks_menu_alter()
 */
function _sebd7tweaks_menu_alter_menu(&$items) {
  $base_path = 'admin/structure/menu';
  // Hide menu settings tab
  if (sebd7tweaks_feature('menu.hide_settings')) {
    $items["$base_path/settings"]['access arguments'] = array('administer site configuration');
  }
  // Allow our module to take control on menu actions' access (for better performances, only if there is at least one restriction)
  if (($disabled = variable_get('sebd7tweaks_disabled_menu_actions', array())) && count($disabled) > 0) {
    $mapping = array(
      'edit'        => 'manage/%menu/edit',
      'delete'      => 'manage/%menu/delete',
      'list'        => 'manage/%menu',
      'item_add'    => 'manage/%menu/add',
      'item_edit'   => 'item/%menu_link/edit',
      'item_reset'  => 'item/%menu_link/reset',
      'item_delete' => 'item/%menu_link/delete',
    );
    // Apply this mapping
    foreach ($mapping as $action => $path) {
      $items["$base_path/$path"]['access callback']  = 'sebd7tweaks_menu_access';
      $items["$base_path/$path"]['access arguments'] = array(4, $action);
    }
    // Implement our own access management
    $items[$base_path]['file']          = 'sebd7tweaks.pages.inc';
    $items[$base_path]['module']        = 'sebd7tweaks';
    $items[$base_path]['page callback'] = 'sebd7tweaks_menu_overview_page';
  }
}

/**
 * Derived function of _sebd7tweaks_menu_alter()
 */
function _sebd7tweaks_menu_alter_taxonomy(&$items) {
  $base_path = 'admin/structure/taxonomy';
  // Rename "taxonomy" as "categories"
  if (sebd7tweaks_feature('taxonomy.rename_menu')) {
    $items[$base_path]['title'] = 'Categories';
  }
  // Replace taxonomy overview page by our own
  if (sebd7tweaks_feature('taxonomy.custom_overview_page')) {
    $items[$base_path] = array(
      'page callback'    => 'sebd7tweaks_taxonomy_overview_page',
      'page arguments'   => array(),
      'access arguments' => array('access taxonomy overview'),
      'module'           => 'sebd7tweaks',
      'file'             => 'sebd7tweaks.pages.inc',
    ) + $items[$base_path];

    foreach (array('' => 'list', '/add' => 'item_add') as $path => $action) {
      $items["$base_path/%taxonomy_vocabulary_machine_name$path"]['access callback']  = 'sebd7tweaks_taxonomy_access';
      $items["$base_path/%taxonomy_vocabulary_machine_name$path"]['access arguments'] = array(3, $action);
    }
  }
  // Allow our module to take control on menu actions' access (for better performances, only if there is at least one restriction)
  if (($disabled = variable_get('sebd7tweaks_disabled_term_access', array())) && count($disabled) > 0) {
    foreach (array('', '/feed') as $path) {
      $items["taxonomy/term/%taxonomy_term$path"]['access callback']  = 'sebd7tweaks_term_access';
      $items["taxonomy/term/%taxonomy_term$path"]['access arguments'] = array(2);
    }
  }
}

/**
 * Derived function of _sebd7tweaks_menu_alter()
 */
function _sebd7tweaks_menu_alter_user(&$items) {
  // Disable access to people settings for user with permission "administer users" but not our custom one
  foreach (array('', '/accounts', '/accounts/settings') as $path) {
    $items["admin/config/people$path"]['access arguments'] = array('administer people settings');
  }
  // Enhance the user page title
  if (sebd7tweaks_feature('user.theme_account_title')) {
    $items['user/%user']['title callback'] = 'sebd7tweaks_user_page_title';
  }
}

/**
 * Derived function of _sebd7tweaks_menu_alter()
 */
function _sebd7tweaks_menu_alter_webform(&$items) {
  $features = array(
    'webform'            => array('change_tab_title'  => array('title' => 'Form')),
    'webform/components' => array('rename_fields_tab' => array('title' => 'Fields')),
    'webform/configure'  => array('rename_params_tab' => array('title' => 'Settings')),
  );

  foreach ($features as $path => $settings) {
    foreach ($settings as $condition => $overrides) {
      if (sebd7tweaks_feature("webform.$condition")) {
        $items["node/%webform_menu/$path"] = $overrides + $items["node/%webform_menu/$path"];
      }
    }
  }

  if (($default = variable_get('sebd7tweaks_easier_webform', FALSE)) && @$default['hide_mail_settings']) {
    $items['node/%webform_menu/webform/emails']['access callback'] = FALSE;
  }
}

