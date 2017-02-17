<?php
/**
 * Helper function: Bring the code of hook_admin_menu_output_alter().
 */
function _sebd7tweaks_admin_menu_alter(&$content) {
  // Remove the index link, which is never used...
  unset($content['menu']['admin/index']);
  // Set reference on menu sections and reorder them in the same time
  foreach (array('content', 'people', 'structure', 'config', 'appearance', 'modules', 'reports') as $weight => $key) {
    if (isset($content['menu']["admin/$key"])) {
      $content['menu']["admin/$key"]['#weight'] = $weight;
      ${"{$key}_section"} = &$content['menu']["admin/$key"];
    }
  }
  // That's all for users with super admin priviledges
  if (user_access('administer site configuration')) {
    return;
  }
  // Prepare a list of parts where we will add "list of existing" links
  $sections = array('content', 'people');
  // Create this ref manually to prevent it to be in the previously defined list (no "list of existing" link needed because children are in it)
  $taxonomy_section = &$structure_section['admin/structure/taxonomy'];
  // Again, set some references on structure sub-parts...
  foreach (element_children($structure_section) as $key) {
    // Suppress "admin/structure/" from the $key content
    $key = substr($key, 16);
    // In case of a sub-part get the same name than a previously created reference...
    if (!isset(${"{$key}_section"})) {
      ${"{$key}_section"} = &$structure_section["admin/structure/$key"];
      $sections[]         = $key;
    }
  }
  // Remove too detailled menu entries
  foreach (element_children($taxonomy_section) as $key) {
    _sebd7tweaks_admin_menu_alter_remove_children($taxonomy_section[$key]);
  }
  _sebd7tweaks_admin_menu_alter_remove_children($content_section['node/add']);
  _sebd7tweaks_admin_menu_alter_remove_children($menu_section, array('admin/structure/menu/add'));
  // Add "list of existing" links to be more understable
  foreach ($sections as $key) {
    ${"{$key}_section"}[${"{$key}_section"}['#href']] = array(
      '#title'  => t('List of @entities', array('@entities' => ${"{$key}_section"}['#title'])),
      '#href'   => ${"{$key}_section"}['#href'],
      '#weight' => -1001,
    );
  }
  // Simplify sections (we don't need sub-sections with a single link)
  foreach (array('config' => array('administration', 'user-interface', 'services')) as $section => $keys) {
    $section .= '_section';

    if (isset(${$section})) {
      foreach ($keys as $key) {
        $key = "{${$section}['#href']}/$key";

        if (isset(${$section}[$key])) {
          _sebd7tweaks_admin_menu_alter_simplify_sections(${$section}[$key]);
        }
      }
    }
  }
}

/**
 * Helper function: Suppress children of the given menu.
 */
function _sebd7tweaks_admin_menu_alter_remove_children(&$menu, $preserve = array()) {
  foreach (element_children($menu) as $key) {
    if (!in_array($key, $preserve)) {
      unset($menu[$key]);
    }
  }
}

/**
 * Helper function: Simplify the given menu by collapse sections with a single child.
 */
function _sebd7tweaks_admin_menu_alter_simplify_sections(&$menu) {
  $children = element_children($menu);

  if (count($children) == 1) {
    $menu = $menu[array_shift($children)];
  }
  // Apply the same for children by recursive call to this function
  elseif (count($children) > 1) {
    foreach ($children as $key) {
      _sebd7tweaks_admin_menu_alter_simplify_sections($menu[$key]);
    }
  }
}

