<?php
#################
# API functions #
#################

function sebd7tweaks_API_views_declare_features() {
  return array(
    'enhance_block_name' => t('Use display title as block title for blocks provided by views module'),
    'prevent_cache_bug'  => t('Prevent the "default views - cache bug" by forcing views cache invalidation in a custom hook_flush_caches().'),
  );
}

/**
 * API function: Return default views contained in <module>/views (each view are a separated file nammed "<name>.inc")
 */
function sebd7tweaks_API_views_default_views($module) {
  return _sebd7tweaks_API_read_views_from_folder(drupal_get_path('module', $module) .'/views');
}

/**
 * API function: Relace names of blocks provided by views with their display title
 */
function sebd7tweaks_API_views_alter_blocks($blocks) {
  $displays = array();
  // For better performance, prepare an array of display by view to ensure not load the view twice or more
  foreach (array_keys($blocks) as $key) {
    list($view_name, $display_name) = explode('-', $key);
    $displays[$view_name][] = $display_name;
  }
  // Use the display title for associated blocks
  foreach ($displays as $view_name => $display_names) {
    // Load the view
    $view = views_get_view($view_name);
    // Load its displays
    $view->init_display();
    // Parse all of them which provide a block
    foreach ($display_names as $display_name) {
      foreach (array($display_name, 'default') as $candidate) {
        if (isset($view->display[$candidate]->display_options['title'])) {
          $blocks["$view_name-$display_name"]['info'] = $view->display[$candidate]->display_options['title'];
          break;
        }
      }
    }
    // Save memory
    $view->destroy();
  }
  // Return blocks list
  return $blocks;
}

####################
# Helper functions #
####################

/**
 * API helper: Read views from the given folder.
 */
function _sebd7tweaks_API_read_views_from_folder($folder) {
  $views = array();
  // Rather than embedding long code from each views, each one have its own file...
  foreach(drupal_system_listing('/\.inc$/', $folder, 'name', 0) as $file) {
    include $file->uri;
    $views[$view->name] = $view;
  }
  // Return the generated views array
  return $views;
}

