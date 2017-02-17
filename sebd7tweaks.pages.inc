<?php
##################
# Page callbacks #
##################

/**
 * Page callback: Custom taxonomy overview page.
 */
function sebd7tweaks_taxonomy_overview_page() {
  return _sebd7tweaks_overview_page(taxonomy_get_vocabularies(), array('list terms', 'edit vocabulary', 'add terms'),
                                    'sebd7tweaks_taxonomy_access', 'admin/structure/taxonomy', 'machine_name', 'name');
}

/**
 * Page callback: Custom menu overview page.
 */
function sebd7tweaks_menu_overview_page() {
  $items = menu_load_all();
  // Fix main menu title which do not use translation...
  $items['main-menu']['title'] = t($items['main-menu']['title']);

  return _sebd7tweaks_overview_page($items, array('list links', 'edit menu', 'add link'),
                                    'sebd7tweaks_menu_access', 'admin/structure/menu/manage', 'menu_name', 'title');
}

/**
 * Page callback: Replace tab "outline" page by the book structure page.
 */
function sebd7tweaks_book_outline($node) {
  drupal_goto("admin/content/book/{$node->book['bid']}");
}

/**
 * Page callback: Replace books list page callback by this version which support order.
 */
function sebd7tweaks_book_render() {
  $book_list = array();

  foreach (sebd7tweaks('book', 'get_books') as $book) {
    $book_list[] = l($book['title'], $book['href'], $book['options']);
  }

  return theme('item_list', array('items' => $book_list));
}

#####################
# Theming functions #
#####################

/**
 * Implements hook_preprocess_HOOK().
 */
function sebd7tweaks_preprocess_sebd7tweaks_contact_full_page_form(&$variables) {
  $form = $variables['form'];

  if (($default = variable_get('sebd7tweaks_contact_label_hidden', FALSE)) && $default['page']) {
    drupal_add_js(drupal_get_path('module', 'sebd7tweaks') .'/js/enhance-contact-form.js');
  }

  $variables['infos'] = '';
  $locked_elements    = array();

  foreach (element_children($form) as $key) {
    if (@$form[$key]['#type'] == 'textfield' && !in_array($key, array('message', 'actions'))) {
      if (@$form[$key]['#access'] === FALSE) {
        $locked_elements[] = '<strong>'. $form[$key]['#title'] .':</strong> <span>'. $form[$key]['#default_value'] .'</span>';
      }

      $variables['infos'] .= render($form[$key]);
    }
  }

  if (count($locked_elements) > 0) {
    $variables['infos'] = theme('item_list', array('items' => $locked_elements, 'attributes' => array('class' => array('infos'))))
                        . $variables['infos'];
  }

  $variables['message'] = drupal_render_children($form);
}

####################
# Helper functions #
####################

/**
 * Generate a page overview of the given structure.
 */
function _sebd7tweaks_overview_page($items, $action_names, $access_callback, $base_path, $machine_name, $human_name) {
  $rows = array();

  foreach ($items as $item) {
    $item = (array) $item;
    $row  = array(
      $access_callback($item, 'list')?     l(t($action_names[0]), "$base_path/{$item[$machine_name]}"):      '',
      $access_callback($item, 'edit')?     l(t($action_names[1]), "$base_path/{$item[$machine_name]}/edit"): '',
      $access_callback($item, 'item_add')? l(t($action_names[2]), "$base_path/{$item[$machine_name]}/add"):  '',
    );
    // If at least one action is permitted, adding the row
    if (count(array_filter($row)) > 0) {
      // Adding it now, no need to calculate the result if the row is not added
      array_unshift($row, theme('menu_admin_overview', array('title' => $item[$human_name], 'description' => $item['description'])));
      // And now, we can adding it to the table
      $rows[] = $row;
    }
  }

  return theme('table', array('rows' => $rows));
}

