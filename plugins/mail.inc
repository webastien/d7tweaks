<?php
#################
# API functions #
#################

/**
 * API function: Return subject and body for the given mail key.
 */
function sebd7tweaks_API_mail_get_texts($module, $key, $vars = array(), $lang = NULL, $replace = TRUE) {
  $texts = "{$module}_mail_{$key}_";
  $texts = drupal_map_assoc(array($texts .'subject', $texts .'body'), 'variable_get');
  $texts = array('subject' => array_shift($texts), 'body' => array_shift($texts));

  if (count(array_filter($texts)) < 2) {
    // Strange: using module_load_include doesn't work ($mail is undefined), with the same code directly here it's ok...
    include DRUPAL_ROOT .'/'. drupal_get_path('module', $module) ."/mails/$key.inc"; // Even with "require_once".

    foreach ($texts as $text => &$value) {
      if (empty($value)) {
        $value = t($mail[$text], array(), array('langcode' => isset($lang)? $lang->language: NULL));
      }
    }
  }

  if ($replace && count($vars) > 0) {
    foreach ($texts as &$value) {
      $value = token_replace($value, $vars, array('language' => $lang, 'sanitize' => FALSE, 'clear' => TRUE));
    }
  }

  return $texts;
}

