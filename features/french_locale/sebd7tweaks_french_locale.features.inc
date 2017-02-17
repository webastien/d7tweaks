<?php
/**
 * @file
 * sebd7tweaks_french_locale.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function sebd7tweaks_french_locale_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}
