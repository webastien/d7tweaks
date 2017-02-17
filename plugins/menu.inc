<?php
function sebd7tweaks_API_menu_declare_features() {
  return array(
    'enhance_user_items' => t('Enhance user module relative items by adding "only for anonymous" / "only for logged in users".'),
    'simplify_path'      => t('Simplify menu item path by replace alias with normal path'),
    'hide_structure'     => t('Hide enable, parent and weight options in the menu item creation / modification form'),
    'hide_settings'      => t('Hide menu settings tab to users which have not the permission %permission',
                              array('%permission' => t('Administer site configuration'))),
  );
}

function sebd7tweaks_API_menu_declare_additions() {
  $additions = array(
    'disabled_menu_actions' => array(
      '#type'          => 'checkboxes',
      '#title'         => t('Disable menu actions'),
      '#options'       => array(),
      '#theme'         => 'sebd7tweaks_settings_menu_actions',
      '#menus_list'    => array(),
      '#default_value' => variable_get('sebd7tweaks_disabled_menu_actions', array()),
    ),
  );
  // Provide a checkbox by actions for available menus
  foreach (menu_get_menus(TRUE) as $name => $menu) {
    foreach (array('list', 'edit', 'delete', 'item_add', 'item_edit', 'item_reset', 'item_delete') as $action) {
      $additions['disabled_menu_actions']['#options']["{$name}__$action"] = "$menu: $action";
    }
    // Used in theming
    $additions['disabled_menu_actions']['#menus_list'][$name] = $menu;
  }

  return $additions;
}

function sebd7tweaks_API_menu_enhance_trailing_classes($menu, $prevnext = TRUE, $in_trail = FALSE) {
  if (is_array($menu)) {
    $items = element_children($menu);
    $found = FALSE;

    foreach ($items as $key) {
      if (isset($menu[$key]['#attributes']['class']) && in_array('active-trail', $menu[$key]['#attributes']['class'])) {
        if (count($menu[$key]['#below']) > 0) {
          $menu[$key]['#below'] = sebd7tweaks_API_menu_enhance_trailing_classes($menu[$key]['#below'], $prevnext, TRUE);
          // Add previous / next classes
          if ($prevnext) {
            $class = 'previous';
            // Current item switch from previous to next
            foreach ($items as $sibling) {
              if ($sibling == $key) {
                $class = 'next';
              }
              else {
                $menu[$sibling]['#localized_options']['attributes']['class'][] = $class;
              }
            }
          }
        }
        else {
          foreach ($items as $sibling) {
            if ($sibling != $key) {
              $menu[$sibling]['#localized_options']['attributes']['class'][] = 'siblings';
            }
          }
        }
        // Active trail has been found, no need to parse other menu children
        $found = TRUE;
        break;
      }
    }

    if (!$found && $in_trail) {
      foreach ($items as $key) {
        $menu[$key]['#localized_options']['attributes']['class'][] = 'children';
      }
    }
  }

  return $menu;
}

function sebd7tweaks_API_menu_move_list_classes_to_links($menu, $exclude = array(), $only = array(), $verified = FALSE) {
  if (is_array($menu)) {
    if (!$verified) {
      $exclude = (array) $exclude;
      $only    = (is_array($only) && count($only))? $only: FALSE;
    }

    if (isset($menu['#attributes']['class']) || isset($menu['#localized_options']['attributes']['class'])) {
      $classes = array();

      foreach (array((array) @$menu['#attributes']['class'], (array) @$menu['#localized_options']['attributes']['class']) as $list) {
        foreach ($list as $class) {
          if (!in_array($class, $classes) && !in_array($class, $exclude) && (!$only || in_array($class, $only))) {
            $classes[] = $class;
          }
        }
      }

      unset($menu['#attributes']['class']);
      $menu['#localized_options']['attributes']['class'] = $classes;
    }

    if (isset($menu['#below']) && is_array($menu['#below'])) {
      foreach (element_children($menu['#below']) as $submenu) {
        $menu['#below'][$submenu] = sebd7tweaks_API_menu_move_list_classes_to_links($menu['#below'][$submenu], $exclude, $only, TRUE);
      }
    }

    foreach (element_children($menu) as $key) {
      $menu[$key] = sebd7tweaks_API_menu_move_list_classes_to_links($menu[$key], $only, $exclude, TRUE);
    }
  }

  return $menu;
}

/**
 * Helper function: Return the current path, with numeric parts replaced by "%".
 * It allows easier checks than current_path().
 */
function sebd7tweaks_API_menu_current_path() {
  static $path = FALSE;
  // A static var is used in case of multiple calls in the same bootstrap.
  if (!$path) {
    $path = explode('/', current_path());

    if ($path[0] == 'search' && count($path) == 3) {
      $path[2] = '%';
    }
    else {
      foreach ($path as &$part) {
        if (is_numeric($part)) {
          $part = '%';
        }
      }
    }

    $path = implode('/', $path);
  }

  return $path;
}

