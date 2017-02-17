<?php
function sebd7tweaks_API_webform_declare_features() {
  return array(
    'change_tab_title'  => t('Replace the tab title by "Form"'),
    'rename_fields_tab' => t('Replace the sub-tab "Form components" by "Fields"'),
    'rename_params_tab' => t('Replace the sub-tab "Form settings" by "Settings"'),
    'simplify_forms'    => t('Simplify form to add/edit component'),
  );
}

function sebd7tweaks_API_webform_declare_additions() {
  $additions = array(
    'easier_webform' => array(
      '#type'          => 'checkboxes',
      '#title'         => t('Simplify webforms'),
      '#default_value' => variable_get('sebd7tweaks_easier_webform', array()),
      '#options'       => array(
        'hide_mail_settings'   => t('Hide mail settings'),
        'hide_access_settings' => t('Hide access settings in webform settings forms'),
      ),
    ),
  );

  foreach (array('block', 'teaser', 'allow_draft', 'auto_save', 'submit_notice', 'submit_text') as $field) {
    $additions['easier_webform']['#options']["hide_settings_$field"]
      = t('Hide option %option in webform settings (advanced fieldset)', array('%option' => $field));
  }

  return $additions;
}

