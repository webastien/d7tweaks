<?php
function sebd7tweaks_API_contact_declare_features() {
  return array(
    'hide_personal_choice' => t('Hide personal contact form management for admin if not activated by default for users'),
    'enhance_global_form'  => t('Hide some fields in the site contact form, mainly for authenticated users'),
    'enhance_perso_form'   => t('Hide some fields in the personal contact form'),
  );
}

function sebd7tweaks_API_contact_declare_additions() {
  return array(
    'contact_label_hidden' => array(
      '#type'          => 'checkboxes',
      '#title'         => t('Hide label on contact form (if placeholder is supported).'),
      '#options'       => array('block' => t('Block'), 'page' => t('Full page')),
      '#default_value' => variable_get('sebd7tweaks_contact_label_hidden', array()),
    ),
  );
}

