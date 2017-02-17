<?php
function sebd7tweaks_API_revisioning_declare_features() {
  return array(
    'summary_admin_path' => t('Use admin theme for summary content page.'),
    'locked_block_path'  => t('Lock block path to non admin users.'),
    'more_logic_block'   => t('Tweak the block to be more logic.'),
    'tweak_form_alter'   => t('Take users with "bypass node access" in consideration for the form alter.'),
  );
}

function sebd7tweaks_API_revisioning_form_alter($form) {
  if (user_access('bypass node access') && !user_access('administer nodes') && isset($form['revision_moderation']['#value'])) {
    // The fact that $form['revision_moderation'] exists ensure other checks are ok
    _sebd7tweaks_API_revisioning_form_alter($form, $form['#node'], $form['revision_moderation']['#value']);
    unset($form['revision_moderation']);
  }

  return $form;
}

/**
 * Helper function: Do what reivisioning made on node forms for users with permission "administer nodes".
 * This is required when the user has "bypass node access" instead: Revisioning should proceed too.
 * So, this function replicates the part of code from revisioning_form_alter() between if (user_access('administer nodes')) condition.
 *
 * Important: When calling this function, you should pass to it :
 * - $node from $form['#node']
 * - $is_moderated_content which can be retrieve in $form['revision_moderation'].
 *
 * Conditions must be checked before to call it because it only reflects a part of code from revisioning without alterations.
 * So, $form['revision_moderation'] must be unset before / after invoking this function.
 *
 * @see sebd7tweaks_API_revisioning_form_alter() for an example usage.
 * Note that here, I've not affect the code style, var names, ... to simplify maintainance when revisioning will change is code.
 */
function _sebd7tweaks_API_revisioning_form_alter(&$form, $node, $is_moderated_content) {
  // Expand and move this vertical tab to top, so that it's in user's face
  $form['menu']['#collapsed'] = TRUE;
  $form['revision_information']['#collapsed'] = FALSE;
  $form['revision_information']['#weight'] = -3;

  $options = array();
  if (isset($node->nid)) {
    $options[NO_REVISION] = t('Modify current revision, no moderation');
  }
  $options[NEW_REVISION_NO_MODERATTION]  = t('Create new revision, no moderation');
  $options[NEW_REVISION_WITH_MODERATION] = t('Create new revision and moderate');

  // This radio selection will appear in hook_node_presave as $node->revision_operation
  $form['revision_information']['revision_operation'] = array(
    '#title' => t('Revision creation and moderation options'),
    '#description' => t('Moderation means that the new revision is not publicly visible until approved by someone with the appropriate permissions.'),
    '#type' => 'radios',
    '#options' => $options,
    '#default_value' => isset($node->nid)
      ? (int)$node->revision + (int)$is_moderated_content
      : ($is_moderated_content ? NEW_REVISION_WITH_MODERATION : NEW_REVISION_NO_MODERATTION)
  );
  unset($form['revision_information']['revision']);

  // Add javascript to show/hide the "Published" checkbox if the user
  // presses one of the first two radio buttons. Also updates summary tabs.
  $js_file = drupal_get_path('module', 'revisioning') . '/revision-radios.js';
  drupal_add_js($js_file, array('weight' => 1)); // after node.js
}

